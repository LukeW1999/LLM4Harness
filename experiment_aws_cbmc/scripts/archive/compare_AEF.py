#!/usr/bin/env python3
"""
compare_AEF.py
==============
Three-way recall comparison: Condition A vs E vs F
  A = NL prompt, no example
  E = NL prompt + same-family GT example  (should improve array_list/linked_list)
  F = NL prompt + wrong-family GT example (ablation: controls for "seeing any CBMC example")

Key hypothesis:
  If E >> F ≈ A  → improvement is predicate-specific (family vocabulary matters)
  If E ≈ F >> A  → any CBMC example helps (generic CBMC style learning)

Also supports Qwen model comparison: run with --model qwen to compare Qwen A vs E vs F.

Usage:
    python compare_AEF.py                   # Claude only
    python compare_AEF.py --model qwen      # Qwen only
    python compare_AEF.py --model both      # Both models side by side
    python compare_AEF.py --save
"""
import sys
import json
import argparse
from pathlib import Path
from scipy import stats
import numpy as np

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))

from feedback_loop import RESULTS_DIR, PILOT_FUNCTIONS, _COND_E_EXAMPLE_FUNCS
from iter_recall import compute_iter_recall

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


def load_best(variant: str, exclude: set = None) -> dict:
    """Load best-iter recall per function from saved JSON or compute on the fly."""
    json_path = EVAL_DIR / f"iter_recall_{variant}.json"
    if json_path.exists():
        data = json.loads(json_path.read_text())
        best = {}
        for r in data["rows"]:
            fn = r["func"]
            if exclude and fn in exclude:
                continue
            if fn not in best or r["recall_fuzzy"] > best[fn]:
                best[fn] = r["recall_fuzzy"]
        return best

    # Fall back to computing from results directory
    variant_dir = RESULTS_DIR / variant
    if not variant_dir.exists():
        return {}
    best = {}
    for fn in sorted(variant_dir.iterdir()):
        if not (fn / "summary.json").exists():
            continue
        name = fn.name
        if exclude and name in exclude:
            continue
        rows = compute_iter_recall(name, variant_dir)
        if rows:
            best[name] = max(r.recall_fuzzy for r in rows)
    return best


def wilcoxon_test(a_vals, b_vals):
    """Paired Wilcoxon signed-rank test on matched function lists."""
    pairs = [(a_vals[f], b_vals[f]) for f in a_vals if f in b_vals]
    if len(pairs) < 10:
        return None, None, len(pairs)
    x = [p[0] for p in pairs]
    y = [p[1] for p in pairs]
    diffs = [yi - xi for xi, yi in zip(x, y)]
    if all(d == 0 for d in diffs):
        return 1.0, 0.0, len(pairs)
    try:
        stat, p = stats.wilcoxon(diffs, alternative="two-sided")
        # rank-biserial r as effect size
        n = len(diffs)
        r_effect = stat / (n * (n + 1) / 2)  # approximate
        return p, r_effect, len(pairs)
    except Exception:
        return None, None, len(pairs)


def compare_model(model_suffix: str, label: str):
    """Run the three-way comparison for one model."""
    sfx = f"_{model_suffix}" if model_suffix else ""

    # Load all three conditions
    a_best = load_best(f"feedback_loop_A{sfx}")
    e_best = load_best(f"feedback_loop_E{sfx}", exclude=_COND_E_EXAMPLE_FUNCS)
    f_best = load_best(f"feedback_loop_F{sfx}")

    # Align to functions present in all three
    common = set(a_best) & set(e_best) & set(f_best)
    # Also remove E's anchor functions from A and F for fair comparison
    common -= _COND_E_EXAMPLE_FUNCS

    if not common:
        print(f"  [{label}] No common functions found (F may still be running)")
        return None

    a = {f: a_best[f] for f in common}
    e = {f: e_best[f] for f in common}
    f = {f: f_best[f] for f in common}

    n = len(common)
    ma = sum(a.values()) / n
    me = sum(e.values()) / n
    mf = sum(f.values()) / n

    print(f"\n{'='*65}")
    print(f"  [{label}] A vs E vs F  (n={n} functions, E excludes 7 anchor funcs)")
    print(f"{'='*65}")
    print(f"  Cond A (no example):         {ma:.1%}  zeros={sum(1 for v in a.values() if v==0)}/{n}")
    print(f"  Cond E (same-family example): {me:.1%}  zeros={sum(1 for v in e.values() if v==0)}/{n}")
    print(f"  Cond F (wrong-family example):{mf:.1%}  zeros={sum(1 for v in f.values() if v==0)}/{n}")
    print(f"  E-A delta: {me-ma:+.1%}   F-A delta: {mf-ma:+.1%}   E-F delta: {me-mf:+.1%}")

    # Statistical tests
    p_ea, r_ea, n_ea = wilcoxon_test(a, e)
    p_fa, r_fa, n_fa = wilcoxon_test(a, f)
    p_ef, r_ef, n_ef = wilcoxon_test(f, e)
    print(f"\n  Wilcoxon signed-rank tests (paired, two-sided):")
    print(f"    E vs A: p={p_ea:.4f} (r={r_ea:.3f})" if p_ea is not None else "    E vs A: N/A")
    print(f"    F vs A: p={p_fa:.4f} (r={r_fa:.3f})" if p_fa is not None else "    F vs A: N/A")
    print(f"    E vs F: p={p_ef:.4f} (r={r_ef:.3f})" if p_ef is not None else "    E vs F: N/A")

    # Per-family
    print(f"\n  Per-family (best-iter, n=matched):")
    print(f"  {'Family':<12} {'A':^8} {'E':^8} {'F':^8} {'E-A':^7} {'E-F':^7}")
    print(f"  {'-'*55}")
    for fam, pred in FAMILIES.items():
        fns = [f2 for f2 in common if pred(f2)]
        if not fns:
            continue
        ma_f = sum(a[fn] for fn in fns) / len(fns)
        me_f = sum(e[fn] for fn in fns) / len(fns)
        mf_f = sum(f[fn] for fn in fns) / len(fns)
        print(f"  {fam:<12} {ma_f:^8.0%} {me_f:^8.0%} {mf_f:^8.0%} {me_f-ma_f:^+7.0%} {me_f-mf_f:^+7.0%}  (n={len(fns)})")

    # Interpretation
    print(f"\n  Interpretation:")
    if me - mf > 0.05 and (p_ef is None or p_ef < 0.05):
        print(f"  → E >> F: same-family predicates drive the improvement (not generic CBMC style)")
    elif abs(me - mf) <= 0.05:
        print(f"  → E ≈ F: any CBMC example helps similarly (generic learning effect)")
    else:
        print(f"  → F > E: unexpected — wrong-family example does better")

    return {"model": label, "n": n, "A": ma, "E": me, "F": mf,
            "E_A_delta": me-ma, "F_A_delta": mf-ma, "E_F_delta": me-mf,
            "p_EA": p_ea, "p_FA": p_fa, "p_EF": p_ef}


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--model", choices=["claude", "qwen", "both"], default="claude")
    parser.add_argument("--save", action="store_true")
    args = parser.parse_args()

    results = []
    if args.model in ("claude", "both"):
        r = compare_model("claude", "Claude")
        if r:
            results.append(r)
    if args.model in ("qwen", "both"):
        r = compare_model("", "Qwen")
        if r:
            results.append(r)

    if args.save and results:
        out = EVAL_DIR / "recall_comparison_AEF.json"
        out.write_text(json.dumps(results, indent=2))
        print(f"\nSaved → {out}")


if __name__ == "__main__":
    main()
