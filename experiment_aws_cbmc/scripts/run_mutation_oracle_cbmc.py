#!/usr/bin/env python3
"""
run_mutation_oracle_cbmc.py — CBMC-based mutation oracle.

Uses CBMC (not ESBMC) so harnesses don't need migration.
Avoids nondet_bool / ESBMC compatibility errors.

For each mutant m of function f:
  GT: cbmc(H_GT, m)  → expect FAIL (catches bug)
  LLM: cbmc(H_LLM, m) → UNSAT/SUCCESS = silenced, FAIL = caught

Usage:
    python3 run_mutation_oracle_cbmc.py --dataset feedback_loop_A_gptoss120b
    python3 run_mutation_oracle_cbmc.py --dataset feedback_loop_A_gptoss120b --workers 8
    python3 run_mutation_oracle_cbmc.py --dataset feedback_loop_A_gptoss120b --func aws_byte_buf_cat
"""

import json, re, subprocess, argparse
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
from collections import defaultdict

SCRIPT_DIR     = Path(__file__).parent
EXPERIMENT_DIR = Path("/root/experiment_aws_cbmc")
MUTANTS_DIR    = EXPERIMENT_DIR / "mutants"
RESULTS_DIR    = EXPERIMENT_DIR / "results"
EVAL_DIR       = EXPERIMENT_DIR / "evaluation"
GT_PROOFS_DIR  = Path("/root/aws-c-common/verification/cbmc/proofs")

SRCDIR   = Path("/root/aws-c-common")
PROOFDIR = SRCDIR / "verification/cbmc"

TIMEOUT  = 120  # seconds per CBMC call

# Import FUNC_CONFIGS and COMMON_FLAGS from cbmc_runner
import sys
sys.path.insert(0, str(EXPERIMENT_DIR / "scripts"))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS


# ── helpers ──────────────────────────────────────────────────────────────────

def get_mutated_source_idx(func: str):
    meta_path = MUTANTS_DIR / func / "metadata.json"
    if not meta_path.exists():
        return None
    meta = json.loads(meta_path.read_text())
    mutated_basename = Path(meta.get("source_file", "")).name
    if not mutated_basename:
        return None
    cfg = FUNC_CONFIGS.get(func, {})
    for i, src in enumerate(cfg.get("project_sources", [])):
        if Path(src).name == mutated_basename:
            return i
    return None


def get_final_harness(func_result_dir: Path):
    harnesses = sorted(func_result_dir.glob("iter_*_harness.c"))
    return harnesses[-1] if harnesses else None


def run_cbmc_on_mutant(func: str, mutant_c: Path, harness_c: Path,
                       timeout: int, mutated_src_idx: int) -> str:
    """Run CBMC with harness_c, substituting project_sources[mutated_src_idx] with mutant_c."""
    cfg = FUNC_CONFIGS.get(func)
    if cfg is None:
        return "COMPILE_ERROR"

    proof_sources   = [str(p) for p in cfg.get("proof_sources", [])]
    project_sources = [str(p) for p in cfg.get("project_sources", [])]
    project_sources[mutated_src_idx] = str(mutant_c)

    obj_bits = cfg.get("object_bits", 8)
    if obj_bits != 8:
        base_flags = [
            f"-I{SRCDIR}/include",
            f"-I{PROOFDIR}/include",
            "-DCBMC",
            f"-DCBMC_OBJECT_BITS={obj_bits}",
            "-DCBMC_MAX_OBJECT_SIZE=(SIZE_MAX>>(CBMC_OBJECT_BITS+1))",
            "--object-bits", str(obj_bits),
        ]
        defines = [d for d in cfg.get("defines", []) if "CBMC_OBJECT_BITS" not in d]
    else:
        base_flags = list(COMMON_FLAGS)
        defines = list(cfg.get("defines", []))

    remove_flags = []
    for fn in cfg.get("remove_function_body", []):
        remove_flags += ["--remove-function-body", fn]

    cmd = (
        ["cbmc"] +
        base_flags +
        defines +
        list(cfg.get("unwind", [])) +
        list(cfg.get("unwindset", []) or []) +
        remove_flags +
        ["--function", f"{func}_harness"] +
        proof_sources +
        [str(harness_c)] +
        project_sources
    )

    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        stdout = proc.stdout
        stderr = proc.stderr
    except subprocess.TimeoutExpired:
        return "TIMEOUT"

    # Compilation check
    bad = ("PARSING ERROR", "CONVERSION ERROR", "Invalid User Input",
           "fatal error", "compilation terminated")
    if any(b in (stdout + stderr) for b in bad):
        return "COMPILE_ERROR"

    if "VERIFICATION SUCCESSFUL" in stdout:
        return "SUCCESS"
    if "VERIFICATION FAILED" in stdout:
        return "FAIL"
    return "UNKNOWN"


# ── worker ───────────────────────────────────────────────────────────────────

def process_mutant(task):
    func, mutant_name, mutant_c, gt_harness, llm_harness, timeout, mutated_src_idx = task
    gt_result  = run_cbmc_on_mutant(func, mutant_c, gt_harness,  timeout, mutated_src_idx)
    llm_result = run_cbmc_on_mutant(func, mutant_c, llm_harness, timeout, mutated_src_idx)
    silenced = (gt_result in ("FAIL", "SAT") and llm_result in ("SUCCESS", "UNSAT"))
    return {
        "func":     func,
        "mutant":   mutant_name,
        "gt":       gt_result,
        "llm":      llm_result,
        "silenced": silenced,
    }


# ── main ─────────────────────────────────────────────────────────────────────

def get_ce_funcs(dataset: str) -> set[str]:
    """Return functions with 100% COMPILE_ERROR in a previous oracle run."""
    oracle = EVAL_DIR / f"mutation_oracle_cbmc_{dataset}.json"
    if not oracle.exists():
        return set()
    data = json.loads(oracle.read_text())
    from collections import defaultdict
    stats = defaultdict(lambda: {"total": 0, "ce": 0})
    for r in data.get("results", []):
        fn = r["func"]
        stats[fn]["total"] += 1
        if r.get("llm") == "COMPILE_ERROR":
            stats[fn]["ce"] += 1
    return {fn for fn, s in stats.items() if s["total"] > 0 and s["ce"] == s["total"]}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dataset", required=True,
                        help="Condition name, e.g. feedback_loop_A_gptoss120b")
    parser.add_argument("--workers", type=int, default=6)
    parser.add_argument("--func", default=None, help="Single function (smoke test)")
    parser.add_argument("--only-ce-funcs", action="store_true",
                        help="Only rerun functions with 100%% COMPILE_ERROR from prior run")
    parser.add_argument("--merge-into", default=None,
                        help="Merge new results into existing JSON (for patched reruns)")
    args = parser.parse_args()

    dataset_dir = RESULTS_DIR / args.dataset

    ce_funcs = get_ce_funcs(args.dataset) if args.only_ce_funcs else set()
    if args.only_ce_funcs:
        print(f"Restricting to {len(ce_funcs)} previously-CE functions")

    tasks = []
    for func_dir in sorted(MUTANTS_DIR.iterdir()):
        func = func_dir.name
        if args.func and func != args.func:
            continue
        if args.only_ce_funcs and func not in ce_funcs:
            continue

        mutated_src_idx = get_mutated_source_idx(func)
        if mutated_src_idx is None:
            continue  # .inl function — skip

        gt_harness = GT_PROOFS_DIR / func / f"{func}_harness.c"
        if not gt_harness.exists():
            continue

        func_result_dir = dataset_dir / func
        llm_harness = get_final_harness(func_result_dir) if func_result_dir.exists() else None
        if llm_harness is None:
            continue

        for mutant_c in sorted(func_dir.glob("mutant_*.c")):
            tasks.append((func, mutant_c.stem, mutant_c, gt_harness,
                          llm_harness, TIMEOUT, mutated_src_idx))

    print(f"Total tasks: {len(tasks)}  workers: {args.workers}")

    results = []
    done = 0
    silenced_count = 0
    with ProcessPoolExecutor(max_workers=args.workers) as ex:
        futures = {ex.submit(process_mutant, t): t for t in tasks}
        for fut in as_completed(futures):
            r = fut.result()
            results.append(r)
            done += 1
            if r["silenced"]:
                silenced_count += 1
            if done % 50 == 0:
                print(f"  [{done:4d}/{len(tasks)}] ...  silenced so far: {silenced_count}")

    print(f"\n  Total mutants run : {len(results)}")
    print(f"  GT caught (FAIL)  : {sum(1 for r in results if r['gt'] in ('FAIL','SAT'))}")
    print(f"  LLM caught (FAIL) : {sum(1 for r in results if r['llm'] in ('FAIL','SAT'))}")
    print(f"  Silenced          : {silenced_count}")
    print(f"  Silence rate      : {silenced_count/max(1,len(results))*100:.1f}%")

    # Merge into existing oracle JSON if requested (for patched-function reruns)
    if args.merge_into and Path(args.merge_into).exists():
        existing = json.loads(Path(args.merge_into).read_text())
        old_results = existing.get("results", [])
        new_funcs = {r["func"] for r in results}
        # Keep old results for functions not in new run
        kept = [r for r in old_results if r["func"] not in new_funcs]
        merged = kept + results
        silenced_count = sum(1 for r in merged if r.get("silenced"))
        results = merged
        print(f"  Merged: kept {len(kept)} old + {len(results)-len(kept)} new results")

    out_path = EVAL_DIR / f"mutation_oracle_cbmc_{args.dataset}.json"
    EVAL_DIR.mkdir(parents=True, exist_ok=True)
    with open(out_path, "w") as f:
        json.dump({
            "summary": {
                "dataset": args.dataset,
                "total_mutants": len(results),
                "silenced": silenced_count,
                "backend": "cbmc",
            },
            "results": results,
        }, f, indent=2)
    print(f"  Saved → {out_path}")


if __name__ == "__main__":
    main()
