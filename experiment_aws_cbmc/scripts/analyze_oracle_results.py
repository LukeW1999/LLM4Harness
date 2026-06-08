#!/usr/bin/env python3
"""
analyze_oracle_results.py - Post-hoc analysis of mutation oracle output.

Usage:
    python3 analyze_oracle_results.py --dataset feedback_loop_A_gptoss120b
    python3 analyze_oracle_results.py --all   # compare all conditions
"""

import json, sys, argparse
from pathlib import Path
from collections import defaultdict, Counter

SCRIPT_DIR    = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent
EVAL_DIR      = EXPERIMENT_DIR / "evaluation"


def load_oracle(dataset: str) -> list[dict]:
    path = EVAL_DIR / f"mutation_oracle_{dataset}.json"
    if not path.exists():
        print(f"Not found: {path}")
        return []
    data = json.loads(path.read_text())
    return data.get("results", [])


def print_summary(results: list[dict], label: str):
    if not results:
        return
    total = len(results)
    gt_fail = sum(1 for r in results if r["gt"] in ("FAIL", "SAT"))
    llm_fail = sum(1 for r in results if r["llm"] in ("FAIL", "SAT"))
    silenced = sum(1 for r in results if r["silenced"])
    gt_timeout = sum(1 for r in results if r["gt"] == "TIMEOUT")
    llm_timeout = sum(1 for r in results if r["llm"] == "TIMEOUT")
    gt_success = sum(1 for r in results if r["gt"] in ("SUCCESS", "UNSAT"))
    compile_err = sum(1 for r in results if r["gt"] == "COMPILE_ERROR")

    print(f"\n{'='*60}")
    print(f"  {label}")
    print(f"{'='*60}")
    print(f"  Total mutants       : {total}")
    print(f"  GT=FAIL (detects)   : {gt_fail} ({100*gt_fail/total:.1f}%)")
    print(f"  GT=SUCCESS          : {gt_success} ({100*gt_success/total:.1f}%)")
    print(f"  GT=TIMEOUT          : {gt_timeout} ({100*gt_timeout/total:.1f}%)")
    print(f"  GT=COMPILE_ERROR    : {compile_err}")
    print(f"  LLM=FAIL (detects)  : {llm_fail} ({100*llm_fail/total:.1f}%)")
    print(f"  LLM=TIMEOUT         : {llm_timeout} ({100*llm_timeout/total:.1f}%)")
    print(f"  SILENCED (GT=F,LLM=S): {silenced} ({100*silenced/max(1,gt_fail):.1f}% of GT-detectable)")
    print()

    # Per-function breakdown (silenced > 0 or GT > 0)
    per_func = defaultdict(lambda: {"total": 0, "gt_fail": 0, "llm_fail": 0,
                                    "silenced": 0, "llm_timeout": 0})
    for r in results:
        f = r["func"]
        per_func[f]["total"] += 1
        if r["gt"] in ("FAIL", "SAT"):
            per_func[f]["gt_fail"] += 1
        if r["llm"] in ("FAIL", "SAT"):
            per_func[f]["llm_fail"] += 1
        if r["silenced"]:
            per_func[f]["silenced"] += 1
        if r["llm"] == "TIMEOUT":
            per_func[f]["llm_timeout"] += 1

    print("  Per-function (functions with GT-detectable mutants):")
    print(f"  {'Function':<45} {'mut':>4} {'GT%':>5} {'sil':>4} {'LLM_TO':>7}")
    for func, d in sorted(per_func.items(), key=lambda x: -x[1]["silenced"]):
        if d["gt_fail"] > 0:
            print(f"  {func:<45} {d['total']:>4} "
                  f"{100*d['gt_fail']/d['total']:>4.0f}% "
                  f"{d['silenced']:>4} "
                  f"{d['llm_timeout']:>7}")
    print()

    # Outcome distribution
    outcomes = Counter((r["gt"], r["llm"]) for r in results)
    print("  Outcome pairs (top 8):")
    for (gt, llm), n in sorted(outcomes.items(), key=lambda x: -x[1])[:8]:
        print(f"    GT={gt:<10} LLM={llm:<10} : {n}")


def compare_all():
    datasets = sorted(EVAL_DIR.glob("mutation_oracle_*.json"))
    if not datasets:
        print("No oracle result files found in", EVAL_DIR)
        return

    print(f"\n{'Condition':<40} {'total':>6} {'GT%':>6} {'sil':>5} {'sil%GT':>8} {'LLM_TO%':>8}")
    print("-" * 75)
    for path in datasets:
        data = json.loads(path.read_text())
        results = data.get("results", [])
        if not results:
            continue
        label = path.stem.replace("mutation_oracle_", "")
        total = len(results)
        gt_fail = sum(1 for r in results if r["gt"] in ("FAIL", "SAT"))
        silenced = sum(1 for r in results if r["silenced"])
        llm_to = sum(1 for r in results if r["llm"] == "TIMEOUT")
        sil_pct = 100 * silenced / max(1, gt_fail)
        to_pct = 100 * llm_to / total
        print(f"  {label:<38} {total:>6} {100*gt_fail/total:>5.1f}% {silenced:>5} {sil_pct:>7.1f}% {to_pct:>7.1f}%")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dataset", default=None)
    parser.add_argument("--all", action="store_true")
    args = parser.parse_args()

    if args.all:
        compare_all()
        return

    dataset = args.dataset or "feedback_loop_A_gptoss120b"
    results = load_oracle(dataset)
    if results:
        print_summary(results, dataset)


if __name__ == "__main__":
    main()
