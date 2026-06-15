#!/usr/bin/env python3
"""
analyze_oracle_results.py — Compare silencing rates across oracle conditions.

Usage:
    python3 analyze_oracle_results.py --all
    python3 analyze_oracle_results.py --datasets feedback_loop_A_gptoss120b feedback_loop_G_gptoss120b
"""

import json, argparse
from pathlib import Path
from collections import defaultdict

EVAL_DIR = Path("/root/experiment_aws_cbmc/evaluation")

CONDITION_LABELS = {
    "feedback_loop_A_gptoss120b":      "A (full pipeline)",
    "feedback_loop_G_gptoss120b":      "G (no CBMC loop)",
    "feedback_loop_H_gptoss120b":      "H (no sacrifice inst)",
    "feedback_loop_M_gptoss120b":      "M (manual GT)",
    "feedback_loop_K_gptoss120b":      "K (keep all asserts)",
    "feedback_loop_Oracle_gptoss120b": "Oracle (GT harness as LLM)",
}


def load_oracle(dataset: str) -> dict | None:
    p = EVAL_DIR / f"mutation_oracle_cbmc_{dataset}.json"
    if not p.exists():
        return None
    return json.loads(p.read_text())


def summarize(dataset: str, data: dict) -> dict:
    results = data.get("results", [])
    total   = len(results)
    gt_fail = sum(1 for r in results if r.get("gt")  in ("FAIL", "SAT"))
    llm_fail= sum(1 for r in results if r.get("llm") in ("FAIL", "SAT"))
    silenced= sum(1 for r in results if r.get("silenced"))
    ce      = sum(1 for r in results if r.get("llm") == "COMPILE_ERROR")
    timeout = sum(1 for r in results if r.get("llm") == "TIMEOUT")

    gt_detectable_covered = sum(
        1 for r in results
        if r.get("gt") in ("FAIL", "SAT") and r.get("llm") != "COMPILE_ERROR"
    )

    return {
        "dataset": dataset,
        "total": total,
        "gt_fail": gt_fail,
        "llm_fail": llm_fail,
        "silenced": silenced,
        "ce": ce,
        "timeout": timeout,
        "silence_rate_total": silenced / max(1, total),
        "silence_rate_gt": silenced / max(1, gt_fail),
        "silence_rate_covered": silenced / max(1, gt_detectable_covered),
    }


def per_func_stats(results: list) -> dict:
    by_func = defaultdict(lambda: {"total": 0, "gt_fail": 0, "silenced": 0, "ce": 0})
    for r in results:
        fn = r["func"]
        by_func[fn]["total"] += 1
        if r.get("gt") in ("FAIL", "SAT"):
            by_func[fn]["gt_fail"] += 1
        if r.get("silenced"):
            by_func[fn]["silenced"] += 1
        if r.get("llm") == "COMPILE_ERROR":
            by_func[fn]["ce"] += 1
    return dict(by_func)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--all", action="store_true", help="Load all available datasets")
    parser.add_argument("--datasets", nargs="+", help="Specific datasets to compare")
    parser.add_argument("--per-func", action="store_true", help="Show per-function breakdown")
    args = parser.parse_args()

    if args.all:
        datasets = [p.stem.removeprefix("mutation_oracle_cbmc_")
                    for p in sorted(EVAL_DIR.glob("mutation_oracle_cbmc_*.json"))]
    else:
        datasets = args.datasets or []

    if not datasets:
        print("No datasets specified. Use --all or --datasets.")
        return

    summaries = []
    for ds in datasets:
        data = load_oracle(ds)
        if data is None:
            print(f"  [MISSING] {ds}")
            continue
        summaries.append(summarize(ds, data))

    print(f"\n{'='*80}")
    print(f"  ORACLE COMPARISON — {len(summaries)} conditions")
    print(f"{'='*80}")
    print(f"  {'Condition':<35} {'Total':>6} {'GT-fail':>7} {'Silenced':>8} "
          f"{'CE%':>5} {'Sil/GT':>8} {'Sil/All':>8}")
    print(f"  {'-'*35} {'-'*6} {'-'*7} {'-'*8} {'-'*5} {'-'*8} {'-'*8}")
    for s in summaries:
        label = CONDITION_LABELS.get(s["dataset"], s["dataset"])
        ce_pct = 100 * s["ce"] / max(1, s["total"])
        print(f"  {label:<35} {s['total']:>6} {s['gt_fail']:>7} {s['silenced']:>8} "
              f"{ce_pct:>4.1f}% {s['silence_rate_gt']:>7.1%} {s['silence_rate_total']:>7.1%}")

    print(f"\n  KEY: Sil/GT = silenced/GT-detectable, Sil/All = silenced/total mutants")

    # Per-function breakdown for silenced mutants
    if args.per_func:
        print(f"\n{'='*80}")
        print("  PER-FUNCTION SILENCED MUTANTS")
        print(f"{'='*80}")
        for ds in datasets:
            data = load_oracle(ds)
            if data is None: continue
            label = CONDITION_LABELS.get(ds, ds)
            stats = per_func_stats(data.get("results", []))
            sil_funcs = {fn: s for fn, s in stats.items() if s["silenced"] > 0}
            if sil_funcs:
                print(f"\n  {label}:")
                for fn, s in sorted(sil_funcs.items(), key=lambda x: -x[1]["silenced"]):
                    print(f"    {fn}: {s['silenced']} silenced / {s['gt_fail']} GT-detectable")


if __name__ == "__main__":
    main()
