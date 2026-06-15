#!/usr/bin/env python3
"""
RQ2 statistical analysis:
1. Wilcoxon signed-rank for H vs A (function-level silence rates)
2. Equivalent mutant sampling from Oracle condition
3. Cross-condition per-function breakdown
4. Vacuity check summary across all conditions
"""
import json
import os
import sys
import random
from collections import defaultdict

EVAL_DIR = "/root/experiment_aws_cbmc/evaluation"

ORACLE_FILES = {
    "A_gptoss":  f"{EVAL_DIR}/mutation_oracle_cbmc_feedback_loop_A_gptoss120b.json",
    "A_claude":  f"{EVAL_DIR}/mutation_oracle_cbmc_feedback_loop_A_claude.json",
    "G_gptoss":  f"{EVAL_DIR}/mutation_oracle_cbmc_feedback_loop_G_gptoss120b.json",
    "H_gptoss":  f"{EVAL_DIR}/mutation_oracle_cbmc_feedback_loop_H_gptoss120b.json",
    "Oracle":    f"{EVAL_DIR}/mutation_oracle_cbmc_feedback_loop_Oracle_gptoss120b.json",
    "M_gptoss":  f"{EVAL_DIR}/mutation_oracle_cbmc_feedback_loop_M_gptoss120b.json",
}


def load_oracle(path):
    with open(path) as f:
        raw = json.load(f)
    # Oracle JSON may be {"results": [...]} or a flat list
    if isinstance(raw, dict):
        return raw.get("results", [])
    return raw


def per_func_silence(data):
    """Returns {func: (silenced, gt_fail, sil_rate)} for each function."""
    func_gt = defaultdict(int)
    func_sil = defaultdict(int)
    for item in data:
        func = item.get("func", "")
        # Fields are "gt" and "llm" (or "gt_result"/"llm_result" in older format)
        gt_res = item.get("gt") or item.get("gt_result", "")
        llm_res = item.get("llm") or item.get("llm_result", "")
        if gt_res == "FAIL":
            func_gt[func] += 1
            if item.get("silenced") or llm_res == "SUCCESS":
                func_sil[func] += 1
    result = {}
    for f in sorted(set(func_gt) | set(func_sil)):
        gt = func_gt[f]
        sil = func_sil[f]
        rate = sil / gt if gt > 0 else 0.0
        result[f] = (sil, gt, rate)
    return result


def wilcoxon_h_vs_a():
    """Wilcoxon signed-rank test: H vs A silence rates at function level."""
    try:
        from scipy import stats
    except ImportError:
        print("scipy not available; skipping Wilcoxon")
        return

    a = load_oracle(ORACLE_FILES["A_gptoss"])
    h = load_oracle(ORACLE_FILES["H_gptoss"])

    a_rates = per_func_silence(a)
    h_rates = per_func_silence(h)

    common = sorted(set(a_rates) & set(h_rates))
    a_vals = [a_rates[f][2] for f in common]
    h_vals = [h_rates[f][2] for f in common]

    # Wilcoxon requires non-zero differences
    diffs = [a - b for a, b in zip(a_vals, h_vals)]
    nonzero_diffs = sum(1 for d in diffs if d != 0)

    print(f"\n=== Wilcoxon H vs A (function-level silence rate) ===")
    print(f"  Common functions: {len(common)}")
    print(f"  Non-zero differences: {nonzero_diffs}")
    if nonzero_diffs < 2:
        print("  Too few differences for Wilcoxon test — distributions are identical")
        return

    stat, pval = stats.wilcoxon(a_vals, h_vals, zero_method="wilcox")
    print(f"  Wilcoxon: stat={stat:.1f}, p={pval:.4f} {'**' if pval<0.01 else '*' if pval<0.05 else 'n.s.'}")
    print(f"  H0: no difference between H and A silence rates")
    print(f"  Mean A silence rate: {sum(a_vals)/len(a_vals):.4f}")
    print(f"  Mean H silence rate: {sum(h_vals)/len(h_vals):.4f}")

    # functions where they differ
    diffs_by_func = [(f, a_rates[f], h_rates[f]) for f in common
                     if abs(a_rates[f][2] - h_rates[f][2]) > 0.01]
    diffs_by_func.sort(key=lambda x: abs(x[1][2] - x[2][2]), reverse=True)
    if diffs_by_func:
        print(f"\n  Functions with |A-H| > 0.01:")
        for f, av, hv in diffs_by_func[:8]:
            print(f"    {f}: A={av[2]:.3f} ({av[0]}/{av[1]})  H={hv[2]:.3f} ({hv[0]}/{hv[1]})")


def equivalent_mutant_analysis(n_sample=50):
    """Analyze GT-passing mutants to estimate equivalent mutant rate."""
    print(f"\n=== Equivalent Mutant Analysis (Oracle condition) ===")

    oracle = load_oracle(ORACLE_FILES["Oracle"])

    def _gt(x): return x.get("gt") or x.get("gt_result", "")

    total = len(oracle)
    gt_fail = sum(1 for x in oracle if _gt(x) == "FAIL")
    gt_success = sum(1 for x in oracle if _gt(x) == "SUCCESS")
    gt_unknown = sum(1 for x in oracle if _gt(x) == "UNKNOWN")
    gt_ce = sum(1 for x in oracle if _gt(x) == "COMPILE_ERROR")

    print(f"  Total mutants: {total}")
    print(f"  GT FAIL (detected):      {gt_fail} ({100*gt_fail/total:.1f}%)")
    print(f"  GT SUCCESS (not caught): {gt_success} ({100*gt_success/total:.1f}%)")
    print(f"  GT UNKNOWN:              {gt_unknown} ({100*gt_unknown/total:.1f}%)")
    print(f"  GT COMPILE_ERROR:        {gt_ce} ({100*gt_ce/total:.1f}%)")

    # GT-SUCCESS mutants are the ceiling candidates
    gt_success_items = [x for x in oracle if _gt(x) == "SUCCESS"]
    print(f"\n  {len(gt_success_items)} GT-SUCCESS mutants to characterize:")

    # Group by function
    by_func = defaultdict(list)
    for item in gt_success_items:
        by_func[item.get("func", "?")].append(item)
    print(f"  Spread across {len(by_func)} functions")
    top_funcs = sorted(by_func.items(), key=lambda x: len(x[1]), reverse=True)[:5]
    print(f"  Top functions (most GT-SUCCESS mutants):")
    for f, items in top_funcs:
        print(f"    {f}: {len(items)} mutants")

    # Sample 20 random GT-SUCCESS mutants for manual inspection
    sample = random.sample(gt_success_items, min(n_sample, len(gt_success_items)))
    out_path = f"{EVAL_DIR}/equivalent_mutant_sample.json"
    with open(out_path, "w") as f:
        json.dump(sample, f, indent=2)
    print(f"\n  Saved {len(sample)} sample mutants to {out_path}")
    print(f"  (Manual inspection needed to classify as equivalent vs. unreachable)")


def cross_condition_per_func():
    """Per-function breakdown of silence across all conditions."""
    print(f"\n=== Per-Function Silence Rate (Cross-Condition) ===")

    conditions = {}
    for label, path in ORACLE_FILES.items():
        if os.path.exists(path):
            conditions[label] = per_func_silence(load_oracle(path))
        else:
            print(f"  Skipping {label}: file not found")

    # Find functions that appear in multiple conditions
    all_funcs = set()
    for rates in conditions.values():
        all_funcs.update(rates.keys())

    # Functions with any silencing in any condition
    active_funcs = set()
    for label, rates in conditions.items():
        for f, (sil, gt, rate) in rates.items():
            if sil > 0:
                active_funcs.add(f)

    print(f"  Total functions: {len(all_funcs)}")
    print(f"  Functions with ≥1 silenced mutant in any condition: {len(active_funcs)}")
    print(f"\n  Per-function breakdown (functions with silencing):")
    print(f"  {'Function':<45} " + "  ".join(f"{k[:8]:<8}" for k in conditions))
    print(f"  {'-'*45} " + "  ".join("-"*8 for _ in conditions))

    for f in sorted(active_funcs):
        row = []
        for label in conditions:
            if f in conditions[label]:
                sil, gt, rate = conditions[label][f]
                row.append(f"{sil}/{gt}")
            else:
                row.append("-")
        print(f"  {f:<45} " + "  ".join(f"{v:<8}" for v in row))


def run_all():
    wilcoxon_h_vs_a()
    equivalent_mutant_analysis()
    cross_condition_per_func()


if __name__ == "__main__":
    run_all()
