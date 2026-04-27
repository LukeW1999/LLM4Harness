#!/usr/bin/env python3
"""
compare_recall_AvsE.py
======================
Compare recall between Condition A and Condition E at:
  - iter 1 (initial generation quality)
  - best-iter (after feedback loop)

Condition E excludes "example functions" (those shown their own GT as the
few-shot example) to avoid data leakage.

Usage:
    python compare_recall_AvsE.py
    python compare_recall_AvsE.py --save
"""
import sys
import json
import argparse
from pathlib import Path
from collections import defaultdict

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))

from feedback_loop import RESULTS_DIR, PILOT_FUNCTIONS, _COND_E_EXAMPLE_FUNCS
from iter_recall import compute_iter_recall

EVAL_DIR = SCRIPT_DIR.parent / "evaluation"

# Functions excluded from condition E evaluation (shown their own GT as example)
EXCLUDED_E = _COND_E_EXAMPLE_FUNCS


def load_best_iter_recall(variant: str, exclude: set[str] = None) -> dict[str, float]:
    """Load best-iteration recall per function for a given variant."""
    data_path = EVAL_DIR / f"iter_recall_{variant}.json"
    if not data_path.exists():
        return {}
    data = json.loads(data_path.read_text())
    best = {}
    for r in data["rows"]:
        fn = r["func"]
        if exclude and fn in exclude:
            continue
        if fn not in best or r["recall_fuzzy"] > best[fn]:
            best[fn] = r["recall_fuzzy"]
    return best


def load_iter1_recall(variant: str, exclude: set[str] = None) -> dict[str, float]:
    """Load iter-1 recall per function for a given variant."""
    data_path = EVAL_DIR / f"iter_recall_{variant}.json"
    if not data_path.exists():
        return {}
    data = json.loads(data_path.read_text())
    iter1 = {}
    for r in data["rows"]:
        if r["iter"] != 1:
            continue
        fn = r["func"]
        if exclude and fn in exclude:
            continue
        iter1[fn] = r["recall_fuzzy"]
    return iter1


def summarize(recalls: dict[str, float], label: str):
    n = len(recalls)
    if not n:
        print(f"  {label}: no data")
        return
    mean = sum(recalls.values()) / n
    zeros = sum(1 for v in recalls.values() if v == 0)
    print(f"  {label}: n={n}, mean={mean:.1%}, zeros={zeros}/{n}")


def per_family(recalls: dict[str, float]) -> dict[str, list[float]]:
    """Group recalls by function family."""
    families = {
        "linked_list": [],
        "array_list":  [],
        "byte_buf":    [],
        "byte_cursor": [],
        "math":        [],
        "string":      [],
        "ring_buffer": [],
        "other":       [],
    }
    math_funcs = {
        "aws_add_size_checked", "aws_add_size_saturating",
        "aws_mul_size_checked", "aws_mul_size_saturating",
        "aws_is_power_of_two", "aws_round_up_to_power_of_two",
        "aws_nospec_mask", "aws_ptr_eq",
        "aws_array_eq", "aws_array_eq_c_str",
    }
    for fn, rec in recalls.items():
        if "linked_list" in fn:
            families["linked_list"].append(rec)
        elif "array_list" in fn:
            families["array_list"].append(rec)
        elif "byte_buf" in fn:
            families["byte_buf"].append(rec)
        elif "byte_cursor" in fn:
            families["byte_cursor"].append(rec)
        elif fn in math_funcs:
            families["math"].append(rec)
        elif "string" in fn:
            families["string"].append(rec)
        elif "ring_buffer" in fn:
            families["ring_buffer"].append(rec)
        else:
            families["other"].append(rec)
    return families


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--save", action="store_true")
    args = parser.parse_args()

    print("=" * 65)
    print("CONDITION A vs E RECALL COMPARISON")
    print(f"(Condition E excludes {len(EXCLUDED_E)} example functions)")
    print("=" * 65)

    a_iter1 = load_iter1_recall("feedback_loop_A_claude")
    e_iter1 = load_iter1_recall("feedback_loop_E_claude", exclude=EXCLUDED_E)
    a_best  = load_best_iter_recall("feedback_loop_A_claude")
    e_best  = load_best_iter_recall("feedback_loop_E_claude", exclude=EXCLUDED_E)

    # Remove excluded functions from A for fair comparison
    a_iter1_fair = {k: v for k, v in a_iter1.items() if k not in EXCLUDED_E}
    a_best_fair  = {k: v for k, v in a_best.items()  if k not in EXCLUDED_E}

    print("\n--- ITERATION 1 (initial generation) ---")
    summarize(a_iter1_fair, "Cond A iter1")
    summarize(e_iter1,      "Cond E iter1")

    print("\n--- BEST ITERATION (after feedback loop) ---")
    summarize(a_best_fair,  "Cond A best")
    summarize(e_best,       "Cond E best")

    # Per-family comparison
    print("\n--- PER-FAMILY BEST-ITER RECALL ---")
    a_fam = per_family(a_best_fair)
    e_fam = per_family(e_best)

    print(f"\n{'Family':<15} {'Cond A':^10} {'Cond E':^10} {'Delta':^8}")
    print("-" * 45)
    for fam in ["linked_list", "array_list", "byte_buf", "byte_cursor", "math", "string", "ring_buffer"]:
        a_vals = a_fam.get(fam, [])
        e_vals = e_fam.get(fam, [])
        a_m = sum(a_vals) / len(a_vals) if a_vals else float("nan")
        e_m = sum(e_vals) / len(e_vals) if e_vals else float("nan")
        delta = e_m - a_m if not (a_m != a_m or e_m != e_m) else float("nan")
        a_str = f"{a_m:.0%}" if a_m == a_m else "N/A"
        e_str = f"{e_m:.0%}" if e_m == e_m else "N/A"
        d_str = f"{delta:+.0%}" if delta == delta else "N/A"
        n_a = len(a_vals)
        n_e = len(e_vals)
        print(f"  {fam:<13}  {a_str:^10} {e_str:^10} {d_str:^8}  (n={n_a}/{n_e})")

    # Functions where E improved most
    if e_best and a_best_fair:
        common = set(e_best) & set(a_best_fair)
        deltas = {fn: e_best[fn] - a_best_fair[fn] for fn in common}
        improved = sorted(deltas.items(), key=lambda x: -x[1])[:10]
        regressed = sorted(deltas.items(), key=lambda x: x[1])[:5]
        print("\n--- TOP 10 IMPROVEMENTS (Cond E vs A, best-iter) ---")
        for fn, d in improved:
            print(f"  {fn}: {a_best_fair[fn]:.0%} → {e_best[fn]:.0%} ({d:+.0%})")
        print("\n--- TOP 5 REGRESSIONS ---")
        for fn, d in regressed:
            print(f"  {fn}: {a_best_fair[fn]:.0%} → {e_best[fn]:.0%} ({d:+.0%})")

    if args.save:
        out = {
            "a_iter1_mean": sum(a_iter1_fair.values()) / len(a_iter1_fair) if a_iter1_fair else 0,
            "e_iter1_mean": sum(e_iter1.values()) / len(e_iter1) if e_iter1 else 0,
            "a_best_mean":  sum(a_best_fair.values()) / len(a_best_fair) if a_best_fair else 0,
            "e_best_mean":  sum(e_best.values()) / len(e_best) if e_best else 0,
            "excluded_e":   list(EXCLUDED_E),
            "n_a": len(a_best_fair),
            "n_e": len(e_best),
        }
        out_path = EVAL_DIR / "recall_comparison_A_vs_E.json"
        out_path.write_text(json.dumps(out, indent=2))
        print(f"\nSaved → {out_path}")


if __name__ == "__main__":
    main()
