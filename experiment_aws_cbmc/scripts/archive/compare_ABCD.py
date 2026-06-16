#!/usr/bin/env python3
"""
compare_ABCD.py
===============
Compare all four null-hypothesis conditions A/B/C/D for one or both models.

Key hypothesis: A ≈ B ≈ C ≈ D — standard prompt engineering has no effect on recall.

Usage:
    python compare_ABCD.py              # Claude only
    python compare_ABCD.py --model qwen # Qwen only
    python compare_ABCD.py --model both # Both models
    python compare_ABCD.py --save
"""
import sys
import json
import argparse
from pathlib import Path
from collections import defaultdict
from scipy import stats
import numpy as np

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))

from feedback_loop import RESULTS_DIR
from iter_recall import run_variant

EVAL_DIR = SCRIPT_DIR.parent / "evaluation"

FAMILIES = {
    "array_list":  lambda f: "array_list" in f,
    "linked_list": lambda f: "linked_list" in f,
    "byte_buf":    lambda f: "byte_buf" in f,
    "byte_cursor": lambda f: "byte_cursor" in f,
    "math":        lambda f: f in {
        "aws_add_size_checked","aws_add_size_saturating","aws_mul_size_checked",
        "aws_mul_size_saturating","aws_is_power_of_two","aws_round_up_to_power_of_two",
        "aws_nospec_mask","aws_ptr_eq","aws_array_eq","aws_array_eq_c_str"},
    "string":      lambda f: "string" in f,
    "ring_buffer": lambda f: "ring_buffer" in f,
}


def load_best(variant: str) -> dict:
    """Load best-iter recall per function from saved JSON or compute on the fly."""
    json_path = EVAL_DIR / f"iter_recall_{variant}.json"
    if json_path.exists():
        data = json.loads(json_path.read_text())
        best = {}
        for r in data["rows"]:
            fn = r["func"]
            if fn not in best or r["recall_fuzzy"] > best[fn]:
                best[fn] = r["recall_fuzzy"]
        return best

    # Fall back to computing from results directory
    variant_dir = RESULTS_DIR / variant
    if not variant_dir.exists():
        return {}
    # Compute by calling run_variant (slow — calls CBMC on each harness)
    print(f"  [INFO] Computing recall for {variant} (no cached JSON found)...")
    agg = run_variant(variant, save=True)
    if agg is None:
        return {}
    # Reload from newly saved JSON
    if json_path.exists():
        data = json.loads(json_path.read_text())
        best = {}
        for r in data["rows"]:
            fn = r["func"]
            if fn not in best or r["recall_fuzzy"] > best[fn]:
                best[fn] = r["recall_fuzzy"]
        return best
    return {}


def kruskal_wallis_test(*groups):
    """Kruskal-Wallis H test for multiple groups."""
    if any(len(g) < 3 for g in groups):
        return None, None
    try:
        stat, p = stats.kruskal(*groups)
        return stat, p
    except Exception:
        return None, None


def compare_conditions(model_suffix: str, label: str, save: bool = False):
    """Compare conditions A/B/C/D for one model."""
    sfx = f"_{model_suffix}" if model_suffix else ""

    conds = {}
    for cond in ["A", "B", "C", "D"]:
        variant = f"feedback_loop_{cond}{sfx}"
        best = load_best(variant)
        if best:
            conds[cond] = best
        else:
            print(f"  [{label}] Condition {cond} not found — skipping.")

    if len(conds) < 2:
        print(f"  [{label}] Not enough conditions to compare.")
        return None

    # Find common functions across all available conditions
    common = set.intersection(*[set(v.keys()) for v in conds.values()])
    n = len(common)

    print(f"\n{'='*65}")
    print(f"  [{label}] Null-hypothesis check: A ≈ B ≈ C ≈ D  (n={n})")
    print(f"{'='*65}")

    means = {}
    for cond, best in sorted(conds.items()):
        vals = [best[f] for f in common]
        means[cond] = sum(vals) / n
        zeros = sum(1 for v in vals if v == 0)
        print(f"  Cond {cond}: {means[cond]:.1%}  zeros={zeros}/{n}")

    # Kruskal-Wallis omnibus test
    groups = [[conds[c][f] for f in common] for c in sorted(conds)]
    h, p_kw = kruskal_wallis_test(*groups)
    if h is not None:
        print(f"\n  Kruskal-Wallis H={h:.3f}, p={p_kw:.4f}")
        if p_kw > 0.05:
            print(f"  → No significant difference among A/B/C/D (p={p_kw:.4f} > 0.05)")
        else:
            print(f"  → Significant difference detected (p={p_kw:.4f} < 0.05) — investigate")

    # Per-family breakdown
    print(f"\n  Per-family (best-iter):")
    header = "  " + f"{'Family':<14}" + "".join(f"  {'Cond '+c:^7}" for c in sorted(conds))
    print(header)
    print("  " + "-" * (14 + 10 * len(conds)))
    for fam, pred in FAMILIES.items():
        fns = [f for f in common if pred(f)]
        if not fns:
            continue
        row = f"  {fam:<14}"
        for cond in sorted(conds):
            m = sum(conds[cond][f] for f in fns) / len(fns)
            row += f"  {m:^7.0%}"
        row += f"  (n={len(fns)})"
        print(row)

    result = {"model": label, "n": n, "conditions": {c: means[c] for c in conds},
              "kw_h": h, "kw_p": p_kw}

    if save:
        out = EVAL_DIR / f"recall_comparison_ABCD{'_' + model_suffix if model_suffix else ''}.json"
        out.write_text(json.dumps(result, indent=2))
        print(f"\nSaved → {out}")

    return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", choices=["claude", "qwen", "both"], default="claude")
    parser.add_argument("--save", action="store_true")
    args = parser.parse_args()

    if args.model in ("claude", "both"):
        compare_conditions("claude", "Claude", save=args.save)
    if args.model in ("qwen", "both"):
        compare_conditions("", "Qwen", save=args.save)


if __name__ == "__main__":
    main()
