#!/usr/bin/env python3
"""
run_mutation_oracle.py - RQ2 mutation oracle.

For each mutant in mutants/<func>/, runs:
  1. GT harness under ESBMC  → expected SAT (detects bug)
  2. LLM harness (final iter) under ESBMC → SAT or UNSAT

Silenced mutant: GT=SAT (FAIL) and LLM=UNSAT (SUCCESS).
This is the paper's primary safety metric.

Usage:
    python run_mutation_oracle.py --dataset feedback_loop_A_gptoss120b --workers 6
    python run_mutation_oracle.py --dataset feedback_loop_A_gptoss120b --func aws_array_list_push_back
"""

import sys, json, argparse, subprocess, shutil, tempfile
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

SCRIPT_DIR    = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent

sys.path.insert(0, str(SCRIPT_DIR))
from esbmc_runner import run_gt, run_esbmc, FUNC_CONFIGS

MUTANTS_DIR = EXPERIMENT_DIR / "mutants"
RESULTS_DIR = EXPERIMENT_DIR / "results"
EVAL_DIR    = EXPERIMENT_DIR / "evaluation"

TIMEOUT = 120  # seconds per mutant run (shorter than GT timeout)


def get_final_harness(func_dir: Path) -> Path | None:
    harnesses = sorted(func_dir.glob("iter_*_harness.c"),
                       key=lambda p: int(p.stem.split("_")[1]))
    return harnesses[-1] if harnesses else None


def get_mutated_source_idx(func: str) -> int | None:
    """Return index into project_sources that the mutant replaces, or None if unresolvable.

    Reads mutants/<func>/metadata.json to find which source file was mutated,
    then matches by basename against project_sources. Returns None (skip) when
    the mutant was generated from a file not in the function's project sources —
    which happens for functions defined in .inl headers where gen_mutants.py
    incorrectly found a call site in an unrelated .c file.
    """
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


def run_esbmc_on_mutant(func: str, mutant_c: Path, harness_c: Path,
                        timeout: int, mutated_src_idx: int) -> str:
    """Run ESBMC with harness_c as the harness, replacing project_sources[mutated_src_idx] with mutant_c."""
    if func not in FUNC_CONFIGS:
        return "SKIP"

    from esbmc_runner import ESBMC_BIN, ESBMC_BASE_FLAGS, _filter_sources
    cfg = FUNC_CONFIGS[func]

    proof_sources   = _filter_sources(cfg["proof_sources"])
    project_sources = _filter_sources(cfg["project_sources"])

    if not project_sources or mutated_src_idx >= len(project_sources):
        return "COMPILE_ERROR"

    # Replace only the specific source file that was mutated
    mutant_sources = list(project_sources)
    mutant_sources[mutated_src_idx] = mutant_c

    all_sources = proof_sources + [harness_c] + mutant_sources

    cmd = (
        [str(ESBMC_BIN)] +
        ESBMC_BASE_FLAGS +
        cfg.get("defines", []) +
        cfg.get("unwind", []) +
        ["--function", f"{func}_harness"] +
        [str(s) for s in all_sources]
    )

    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        combined = proc.stderr + proc.stdout
        if "PARSING ERROR" in combined or "CONVERSION ERROR" in combined:
            return "COMPILE_ERROR"
        if "VERIFICATION SUCCESSFUL" in combined:
            return "SUCCESS"
        if "VERIFICATION FAILED" in combined:
            return "FAIL"
        return "UNKNOWN"
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    except Exception as e:
        return f"ERROR:{str(e)[:80]}"


def _worker(args):
    func, mutant_path, gt_harness, llm_harness, mutated_src_idx = args
    mutant_name = mutant_path.stem

    gt_result  = run_esbmc_on_mutant(func, mutant_path, gt_harness,  TIMEOUT, mutated_src_idx)
    llm_result = run_esbmc_on_mutant(func, mutant_path, llm_harness, TIMEOUT, mutated_src_idx)

    silenced = (gt_result in ("FAIL", "SAT") and llm_result in ("SUCCESS", "UNSAT"))
    return {
        "func":     func,
        "mutant":   mutant_name,
        "gt":       gt_result,
        "llm":      llm_result,
        "silenced": silenced,
    }


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dataset", default="feedback_loop_A_gptoss120b")
    parser.add_argument("--workers", type=int, default=6)
    parser.add_argument("--func",    default=None, help="Run single function only")
    args = parser.parse_args()

    dataset_dir = RESULTS_DIR / args.dataset
    if not dataset_dir.exists():
        print(f"Dataset not found: {dataset_dir}"); sys.exit(1)

    # Collect tasks
    tasks = []
    funcs_to_run = [args.func] if args.func else sorted(FUNC_CONFIGS.keys())

    for func in funcs_to_run:
        mutant_dir = MUTANTS_DIR / func
        if not mutant_dir.exists():
            continue

        # Skip functions whose mutants target a file not in project_sources
        # (happens for functions defined in .inl headers — gen_mutants found call sites)
        mutated_src_idx = get_mutated_source_idx(func)
        if mutated_src_idx is None:
            print(f"  SKIP {func}: mutant source_file not in project_sources (bad mutant gen)")
            continue

        func_result_dir = dataset_dir / func
        llm_harness = get_final_harness(func_result_dir) if func_result_dir.exists() else None
        if llm_harness is None:
            print(f"  SKIP {func}: no LLM harness found")
            continue

        # GT harness path
        gt_harness = (Path("/root/aws-c-common") / "verification/cbmc/proofs" /
                      func / f"{func}_harness.c")
        if not gt_harness.exists():
            print(f"  SKIP {func}: no GT harness at {gt_harness}")
            continue

        mutants = sorted(mutant_dir.glob("*.c"))
        for m in mutants:
            tasks.append((func, m, gt_harness, llm_harness, mutated_src_idx))

    print(f"Mutation oracle: {len(tasks)} mutant runs across "
          f"{len(set(t[0] for t in tasks))} functions")
    print(f"Dataset: {args.dataset} | Workers: {args.workers} | Timeout: {TIMEOUT}s/run")
    print()

    results = []
    silenced_count = 0
    done = 0

    with ProcessPoolExecutor(max_workers=args.workers) as ex:
        futures = {ex.submit(_worker, t): t for t in tasks}
        for fut in as_completed(futures):
            r = fut.result()
            results.append(r)
            done += 1
            if r["silenced"]:
                silenced_count += 1
                print(f"  [{done:4d}/{len(tasks)}] SILENCED  {r['func']}/{r['mutant']}"
                      f"  GT={r['gt']} LLM={r['llm']}")
            elif done % 50 == 0:
                print(f"  [{done:4d}/{len(tasks)}] ...  silenced so far: {silenced_count}")
            sys.stdout.flush()

    # Summary
    print(f"\n{'='*60}")
    print(f"MUTATION ORACLE RESULTS — {args.dataset}")
    print(f"{'='*60}")
    print(f"  Total mutants run : {len(results)}")
    print(f"  GT caught (SAT)   : {sum(1 for r in results if r['gt'] in ('FAIL','SAT'))}")
    print(f"  LLM caught (SAT)  : {sum(1 for r in results if r['llm'] in ('FAIL','SAT'))}")
    print(f"  SILENCED          : {silenced_count}")
    print(f"  Silence rate      : {silenced_count/max(1,len(results))*100:.1f}%")

    # Per-function breakdown
    from collections import defaultdict
    per_func = defaultdict(lambda: {"total": 0, "silenced": 0})
    for r in results:
        per_func[r["func"]]["total"] += 1
        if r["silenced"]:
            per_func[r["func"]]["silenced"] += 1
    print(f"\nPer-function silenced mutants (non-zero only):")
    for fn, d in sorted(per_func.items(), key=lambda x: -x[1]["silenced"]):
        if d["silenced"] > 0:
            print(f"  {fn:<45} {d['silenced']:>3}/{d['total']}")

    # Save results
    out_path = EVAL_DIR / f"mutation_oracle_{args.dataset}.json"
    EVAL_DIR.mkdir(exist_ok=True)
    with open(out_path, "w") as f:
        json.dump({"summary": {
            "dataset": args.dataset,
            "total_mutants": len(results),
            "silenced": silenced_count,
        }, "results": results}, f, indent=2)
    print(f"\nSaved to {out_path}")


if __name__ == "__main__":
    main()
