#!/usr/bin/env python3
"""
Manipulation check for Condition H (R2 from revision roadmap).

Checks: Does Condition A produce a significantly higher gratuitous deletion
rate than Condition H on correct-code functions?

Method:
  For each function in conditions A and H:
    - Load iteration logs from feedback_loop_A_gptoss120b and feedback_loop_H_gptoss120b
    - Track assertions across iterations using Jaccard token similarity
    - Compute per-function sacrifice rate:
        sacrifice_rate = n_sacrificed / (n_sacrificed + n_retained)
        where "sacrificed" = appeared in iter k but absent in final iter, for any k
    - Run Wilcoxon signed-rank test on paired (A, H) sacrifice rates

If p < 0.05 (A > H): manipulation succeeded; H is a valid control
If p >= 0.05 (A ≈ H): manipulation failed; H is functionally equivalent to A
                      (sacrifice is emergent, not instructed — supports conformance-pressure hypothesis)

Usage:
  python3 run_manipulation_check.py
"""

import json
import os
import re
import sys
from pathlib import Path
from collections import defaultdict
import statistics

RESULTS_DIR = Path("/root/experiment_aws_cbmc/results")
EVAL_DIR = Path("/root/experiment_aws_cbmc/evaluation")


def jaccard_similarity(s1, s2):
    t1 = set(re.findall(r'\w+', s1))
    t2 = set(re.findall(r'\w+', s2))
    if not t1 or not t2:
        return 0.0
    return len(t1 & t2) / len(t1 | t2)


def extract_asserts(harness_text):
    """Extract all assert() statements from a C harness."""
    pattern = r'(?<![/\*])\s*(assert\s*\([^;]+\)\s*;)'
    lines = harness_text.split('\n')
    asserts = []
    for line in lines:
        stripped = line.strip()
        if stripped.startswith('assert(') and not stripped.startswith('//'):
            asserts.append(stripped)
        elif 'assert(' in stripped and not stripped.startswith('//') and not stripped.startswith('*'):
            # handle mid-line asserts
            m = re.search(r'assert\s*\([^;]+\)\s*;', stripped)
            if m:
                asserts.append(m.group(0))
    return asserts


def load_iteration_harnesses(func_dir):
    """Load all iteration harnesses in order. Returns [(iter_num, asserts), ...]"""
    func_path = Path(func_dir)
    harness_files = sorted(func_path.glob("iter_*_harness.c"),
                           key=lambda p: int(re.search(r'iter_(\d+)', p.name).group(1)))
    iterations = []
    for path in harness_files:
        iter_num = int(re.search(r'iter_(\d+)', path.name).group(1))
        try:
            text = path.read_text()
            asserts = extract_asserts(text)
            iterations.append((iter_num, asserts))
        except Exception:
            pass
    return iterations


def compute_sacrifice_rate(iterations):
    """
    Compute sacrifice rate for a function's iteration log.
    Returns (n_sacrificed, n_retained, sacrifice_rate)

    Sacrifice: assertion appeared in at least one iteration, absent in final iteration.
    Retained: assertion appeared and also in final iteration.
    """
    if not iterations:
        return 0, 0, 0.0

    # Get final iteration assertions
    final_asserts = iterations[-1][1]

    # Collect all assertions that appeared in any iteration
    all_appeared = set()
    for _, asserts in iterations:
        all_appeared.update(asserts)

    if not all_appeared:
        return 0, 0, 0.0

    # Match each appeared assertion against final assertions
    def is_in_final(a):
        for fa in final_asserts:
            if jaccard_similarity(a, fa) >= 0.45:
                return True
        return False

    # Count sacrificed: appeared but not in final
    sacrificed = [a for a in all_appeared if not is_in_final(a)]
    retained = [a for a in all_appeared if is_in_final(a)]

    n_sac = len(sacrificed)
    n_ret = len(retained)
    total = n_sac + n_ret
    rate = n_sac / total if total > 0 else 0.0

    return n_sac, n_ret, rate


def compute_condition_sacrifice_rates(condition):
    """Compute per-function sacrifice rates for a condition."""
    cond_dir = RESULTS_DIR / f"feedback_loop_{condition}_gptoss120b"
    if not cond_dir.exists():
        print(f"  [WARN] Directory not found: {cond_dir}")
        return {}

    results = {}
    for func_dir in sorted(cond_dir.iterdir()):
        if not func_dir.is_dir():
            continue
        func_name = func_dir.name
        iterations = load_iteration_harnesses(func_dir)
        if len(iterations) < 2:
            # Single iteration = no feedback loop = no chance to sacrifice
            continue
        n_sac, n_ret, rate = compute_sacrifice_rate(iterations)
        results[func_name] = {
            "n_iterations": len(iterations),
            "n_sacrificed": n_sac,
            "n_retained": n_ret,
            "sacrifice_rate": rate,
        }

    return results


def run_manipulation_check():
    print("=== Manipulation Check: Condition H vs A (sacrifice rates) ===\n")

    # Compute sacrifice rates for A and H
    print("Computing Condition A sacrifice rates...")
    a_rates = compute_condition_sacrifice_rates("A")
    print(f"  {len(a_rates)} functions with ≥2 iterations")

    print("Computing Condition H sacrifice rates...")
    h_rates = compute_condition_sacrifice_rates("H")
    print(f"  {len(h_rates)} functions with ≥2 iterations")

    # Find common functions
    common = sorted(set(a_rates) & set(h_rates))
    print(f"\nCommon functions: {len(common)}")

    if not common:
        print("No common functions — cannot run Wilcoxon test")
        return

    a_vals = [a_rates[f]["sacrifice_rate"] for f in common]
    h_vals = [h_rates[f]["sacrifice_rate"] for f in common]

    # Summary stats
    print(f"\nSacrifice rate summary:")
    print(f"  A: mean={statistics.mean(a_vals):.3f}, median={statistics.median(a_vals):.3f}")
    print(f"  H: mean={statistics.mean(h_vals):.3f}, median={statistics.median(h_vals):.3f}")

    a_nonzero = sum(1 for v in a_vals if v > 0)
    h_nonzero = sum(1 for v in h_vals if v > 0)
    print(f"  A: {a_nonzero}/{len(common)} functions with any sacrifice")
    print(f"  H: {h_nonzero}/{len(common)} functions with any sacrifice")

    # Wilcoxon signed-rank (one-sided: A > H)
    try:
        from scipy import stats
        diffs = [a - b for a, b in zip(a_vals, h_vals)]
        nonzero = sum(1 for d in diffs if d != 0)
        print(f"\nNon-zero differences: {nonzero}/{len(common)}")

        if nonzero < 2:
            print("Insufficient differences for Wilcoxon test")
            print("INTERPRETATION: A and H are functionally identical — manipulation failed")
            print("  → Sacrifice is EMERGENT under verifier pressure, not instructed by prompt")
        else:
            # Two-sided test (then interpret direction)
            stat, pval_two = stats.wilcoxon(a_vals, h_vals, zero_method="wilcox", alternative="two-sided")
            # One-sided (A > H)
            stat_g, pval_greater = stats.wilcoxon(a_vals, h_vals, zero_method="wilcox", alternative="greater")

            print(f"\nWilcoxon signed-rank test:")
            print(f"  Two-sided: stat={stat:.1f}, p={pval_two:.4f}")
            print(f"  One-sided A>H: stat={stat_g:.1f}, p={pval_greater:.4f}")

            sig_level = "**" if pval_greater < 0.01 else "*" if pval_greater < 0.05 else "n.s."
            print(f"\nResult: {sig_level} (α=0.05, one-sided)")

            if pval_greater < 0.05:
                print("INTERPRETATION: A > H sacrifice rate (p<0.05, one-sided)")
                print("  → Sacrifice instruction in A successfully increased deletion behavior")
                print("  → H is a valid control; H≈A silence rate shows something else matters")
            else:
                print("INTERPRETATION: No significant difference in sacrifice rates (A ≈ H)")
                print("  → Sacrifice instruction does NOT significantly increase deletions")
                print("  → Manipulation FAILED: H is functionally equivalent to A for sacrifice behavior")
                print("  → BUT: this means sacrifice is EMERGENT — LLMs sacrifice under pressure")
                print("    regardless of prompt wording. Supports conformance-pressure hypothesis.")

    except ImportError:
        print("scipy not available; manual Wilcoxon needed")

    # Show top functions with biggest A-H differences
    diffs_by_func = [(f, a_rates[f], h_rates[f]) for f in common
                     if abs(a_rates[f]["sacrifice_rate"] - h_rates[f]["sacrifice_rate"]) > 0.05]
    diffs_by_func.sort(key=lambda x: abs(x[1]["sacrifice_rate"] - x[2]["sacrifice_rate"]), reverse=True)

    if diffs_by_func:
        print(f"\nTop 10 functions with |A-H| sacrifice rate > 0.05:")
        print(f"  {'Function':<45} {'A-rate':<8} {'H-rate':<8} {'diff':<8} A-iters H-iters")
        print(f"  {'-'*45} {'-'*8} {'-'*8} {'-'*8} ------- -------")
        for f, av, hv in diffs_by_func[:10]:
            print(f"  {f:<45} {av['sacrifice_rate']:.3f}   {hv['sacrifice_rate']:.3f}   "
                  f"{av['sacrifice_rate']-hv['sacrifice_rate']:+.3f}  "
                  f"{av['n_iterations']:<7} {hv['n_iterations']}")

    # Save results
    out_path = EVAL_DIR / "manipulation_check_A_vs_H.json"
    save_data = {
        "common_functions": len(common),
        "a_mean_sacrifice": statistics.mean(a_vals),
        "h_mean_sacrifice": statistics.mean(h_vals),
        "a_nonzero_functions": a_nonzero,
        "h_nonzero_functions": h_nonzero,
        "per_function": {
            f: {"a": a_rates[f], "h": h_rates[f], "diff": a_rates[f]["sacrifice_rate"] - h_rates[f]["sacrifice_rate"]}
            for f in common
        }
    }
    with open(out_path, "w") as f:
        json.dump(save_data, f, indent=2)
    print(f"\nResults saved to {out_path}")


if __name__ == "__main__":
    run_manipulation_check()
