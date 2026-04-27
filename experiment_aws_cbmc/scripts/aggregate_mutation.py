#!/usr/bin/env python3
"""
aggregate_mutation.py
=====================
Read per-function iter_N_mutation_report.json files and produce a
combined evaluation/mutation_results_<variant>_iterN.json.

Useful when mutation_test.py --all was run but --save aggregate was
not yet in the script, or to re-aggregate from partial results.

Usage:
    python aggregate_mutation.py --variant feedback_loop_C_claude --iter 1
    python aggregate_mutation.py --all-variants --iter 1
"""

import sys
import json
import argparse
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))

from feedback_loop import RESULTS_DIR, PILOT_FUNCTIONS
from cbmc_runner   import FUNC_CONFIGS

EVAL_DIR = SCRIPT_DIR.parent / "evaluation"


def _find_best_mutation_report(func_dir: Path) -> Path | None:
    """
    Return the mutation report for the best (highest-numbered) iteration.
    Returns None if no report exists.
    """
    reports = sorted(
        func_dir.glob("iter_*_mutation_report.json"),
        key=lambda p: int(p.stem.split("_")[1])
    )
    return reports[-1] if reports else None


def aggregate_variant(variant: str, iteration: int | None = None) -> dict | None:
    """
    Aggregate per-function mutation reports.
    If iteration is None, use the best available iteration per function.
    """
    variant_dir = RESULTS_DIR / variant
    if not variant_dir.exists():
        print(f"[SKIP] {variant}: directory not found")
        return None

    _, func_names = zip(*PILOT_FUNCTIONS)
    summaries = []

    for func_name in func_names:
        func_dir = variant_dir / func_name
        if not func_dir.exists():
            continue
        if iteration is not None:
            report_path = func_dir / f"iter_{iteration}_mutation_report.json"
        else:
            report_path = _find_best_mutation_report(func_dir)
        if report_path is None or not report_path.exists():
            continue
        data = json.loads(report_path.read_text())
        if data.get("total_mutants", 0) > 0:
            summaries.append(data)

    if not summaries:
        print(f"[SKIP] {variant} iter{iteration}: no mutation reports found")
        return None

    total_mutants   = sum(s["total_mutants"] for s in summaries)
    total_gt_killed = sum(s.get("gt_killed", 0) or
                          round(s.get("gt_score", 0) * s["total_mutants"])
                          for s in summaries)
    total_llm_killed = sum(s.get("llm_killed", 0) or
                           round(s.get("llm_score", 0) * s["total_mutants"])
                           for s in summaries)
    n = len(summaries)
    avg_gt  = sum(s["gt_score"]  for s in summaries) / n
    avg_llm = sum(s["llm_score"] for s in summaries) / n

    result = {
        "variant":   variant,
        "iteration": iteration,
        "functions": summaries,
        "aggregate": {
            "n":               n,
            "avg_gt_score":    round(avg_gt,  3),
            "avg_llm_score":   round(avg_llm, 3),
            "total_mutants":   total_mutants,
        },
    }

    iter_label = iteration if iteration is not None else "best"
    out_path = EVAL_DIR / f"mutation_results_{variant}_iter{iter_label}.json"
    EVAL_DIR.mkdir(exist_ok=True)
    result["iteration"] = iter_label
    out_path.write_text(json.dumps(result, indent=2))

    print(f"{variant} iter{iter_label}: n={n} funcs, "
          f"avg GT={avg_gt:.0%}, avg LLM={avg_llm:.0%}  → {out_path.name}")
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--variant",      default="feedback_loop_C_claude")
    parser.add_argument("--all-variants", action="store_true")
    parser.add_argument("--iter",         type=int, default=None,
                        help="Specific iteration (default: best available per function)")
    args = parser.parse_args()

    variants = (
        ["feedback_loop_A_claude", "feedback_loop_B_claude",
         "feedback_loop_C_claude", "feedback_loop_D_claude"]
        if args.all_variants else [args.variant]
    )

    for v in variants:
        aggregate_variant(v, args.iter)


if __name__ == "__main__":
    main()
