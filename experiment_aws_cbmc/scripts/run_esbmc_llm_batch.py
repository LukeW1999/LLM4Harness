#!/usr/bin/env python3
"""
run_esbmc_llm_batch.py - Run LLM-generated harnesses (final iteration) through ESBMC.

For each function in a feedback_loop result directory, takes the LAST iteration
harness and runs it through ESBMC. Compares against CBMC results from summary.json.

Usage:
    python run_esbmc_llm_batch.py --dataset feedback_loop_D_claude
    python run_esbmc_llm_batch.py --dataset feedback_loop_A_claude --workers 4
"""

import sys, json, argparse
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

sys.path.insert(0, str(Path(__file__).parent))
from esbmc_runner import run_esbmc, FUNC_CONFIGS

RESULTS_DIR = Path(__file__).parent.parent / "results"

TIMEOUT = 300

def get_final_harness(func_dir: Path) -> Path:
    """Return the last iter_N_harness.c in the directory."""
    harnesses = sorted(func_dir.glob("iter_*_harness.c"),
                       key=lambda p: int(p.stem.split("_")[1]))
    return harnesses[-1] if harnesses else None

def get_cbmc_result(func_dir: Path) -> dict:
    summary = func_dir / "summary.json"
    if summary.exists():
        return json.loads(summary.read_text())
    return {}

def _run_one(fn: str, harness: Path) -> tuple:
    try:
        r = run_esbmc(fn, harness, timeout=TIMEOUT)
        return fn, r.verification_result, r.compilation_ok, r.num_checks, r.error_summary[:200]
    except Exception as e:
        return fn, "ERROR", False, 0, str(e)[:200]

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dataset", default="feedback_loop_D_claude")
    parser.add_argument("--workers", type=int, default=6)
    args = parser.parse_args()

    dataset_dir = RESULTS_DIR / args.dataset
    if not dataset_dir.exists():
        print(f"Dataset not found: {dataset_dir}")
        sys.exit(1)

    # Load ESBMC GT results for reference
    gt_file = RESULTS_DIR / "esbmc_gt_results.json"
    esbmc_gt = json.loads(gt_file.read_text()) if gt_file.exists() else {}

    # Collect tasks: only functions in ESBMC FUNC_CONFIGS
    tasks = []
    skipped = []
    for func_dir in sorted(dataset_dir.iterdir()):
        if not func_dir.is_dir():
            continue
        fn = func_dir.name
        if fn not in FUNC_CONFIGS:
            skipped.append(fn)
            continue
        harness = get_final_harness(func_dir)
        if harness is None:
            skipped.append(fn)
            continue
        cbmc = get_cbmc_result(func_dir)
        tasks.append((fn, harness, cbmc))

    print(f"Running ESBMC on {len(tasks)} LLM harnesses from '{args.dataset}'")
    print(f"Skipped (no ESBMC config or harness): {len(skipped)}")
    print()

    results = {}
    counts = {"SUCCESS": 0, "FAIL": 0, "TIMEOUT": 0,
              "COMPILE_ERROR": 0, "UNKNOWN": 0, "ERROR": 0}

    with ThreadPoolExecutor(max_workers=args.workers) as pool:
        futures = {pool.submit(_run_one, fn, h): (fn, cbmc)
                   for fn, h, cbmc in tasks}
        done = 0
        for fut in as_completed(futures):
            fn, cbmc = futures[fut]
            fn2, verdict, comp_ok, n_checks, err = fut.result()
            cbmc_verdict = "SUCCESS" if cbmc.get("verification_equivalent") else \
                           cbmc.get("iterations", [{}])[-1].get("verify", "UNKNOWN") \
                           if cbmc.get("iterations") else "UNKNOWN"
            esbmc_gt_v = esbmc_gt.get(fn, {}).get("verdict", "N/A")
            results[fn] = {
                "esbmc_verdict":    verdict,
                "cbmc_verdict":     cbmc_verdict,
                "esbmc_gt_verdict": esbmc_gt_v,
                "compilation_ok":   comp_ok,
                "num_checks":       n_checks,
                "error":            err,
                "harness_iter":     get_final_harness(dataset_dir / fn).stem,
            }
            counts[verdict] = counts.get(verdict, 0) + 1
            done += 1
            agree = "✓" if (verdict == "SUCCESS") == (cbmc_verdict == "SUCCESS") else "✗"
            print(f"  [{done:3d}/{len(tasks)}] {agree} {fn:<50s} "
                  f"CBMC={cbmc_verdict:<8s} ESBMC={verdict}")
            sys.stdout.flush()

    # Summary
    print("\n── Summary ──────────────────────────────────────────")
    for k, v in sorted(counts.items()):
        if v:
            print(f"  {k:<15s}: {v}")
    print(f"  TOTAL         : {len(tasks)}")

    # Agreement analysis
    agree_both_pass  = sum(1 for r in results.values()
                           if r["esbmc_verdict"] == "SUCCESS" and r["cbmc_verdict"] == "SUCCESS")
    agree_both_fail  = sum(1 for r in results.values()
                           if r["esbmc_verdict"] != "SUCCESS" and r["cbmc_verdict"] != "SUCCESS")
    disagree_cb_pass = sum(1 for r in results.values()
                           if r["cbmc_verdict"] == "SUCCESS" and r["esbmc_verdict"] != "SUCCESS")
    disagree_es_pass = sum(1 for r in results.values()
                           if r["cbmc_verdict"] != "SUCCESS" and r["esbmc_verdict"] == "SUCCESS")

    print("\n── Cross-verifier Agreement ─────────────────────────")
    print(f"  Both PASS     : {agree_both_pass}")
    print(f"  Both FAIL     : {agree_both_fail}")
    print(f"  CBMC✓ ESBMC✗ : {disagree_cb_pass}")
    print(f"  CBMC✗ ESBMC✓ : {disagree_es_pass}")
    total = len(tasks)
    agree_rate = (agree_both_pass + agree_both_fail) / total * 100 if total else 0
    print(f"  Agreement rate: {agree_rate:.1f}%")

    out_path = RESULTS_DIR / f"esbmc_llm_{args.dataset}.json"
    with open(out_path, "w") as f:
        json.dump(results, f, indent=2)
    print(f"\nSaved to {out_path}")

if __name__ == "__main__":
    main()
