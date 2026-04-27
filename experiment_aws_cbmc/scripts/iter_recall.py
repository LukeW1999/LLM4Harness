#!/usr/bin/env python3
"""
iter_recall.py
==============
Compute postcondition recall at each feedback-loop iteration to answer:
  "Does the feedback loop improve specification recall?"

For each function and each available iter_N_harness.c, compare against
the GT harness using CBMC --show-properties + fuzzy property matching.

Result: per-iteration recall table + aggregate trend.

Usage:
    python iter_recall.py --variant feedback_loop_C_claude
    python iter_recall.py --variant feedback_loop_C_claude --save
    python iter_recall.py --all-variants --save
"""

import sys
import json
import argparse
from pathlib import Path
from dataclasses import dataclass

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))

from cbmc_runner import FUNC_CONFIGS, PROOFDIR
from cross_verify import get_properties, match_properties
from feedback_loop import RESULTS_DIR, PILOT_FUNCTIONS

EVAL_DIR = SCRIPT_DIR.parent / "evaluation"


# ── per-function, per-iteration recall ───────────────────────────────────────

@dataclass
class IterRecallRow:
    func:        str
    iteration:   int
    recall:      float   # strict
    recall_fuzzy: float  # fuzzy (normalised)
    gt_count:    int
    llm_count:   int
    matched:     int


def compute_iter_recall(func_name: str, variant_dir: Path,
                        verbose: bool = False) -> list[IterRecallRow]:
    """Return one IterRecallRow per available iteration for func_name."""
    gt_path = PROOFDIR / "proofs" / func_name / f"{func_name}_harness.c"
    if not gt_path.exists():
        return []

    func_dir = variant_dir / func_name
    if not func_dir.exists():
        return []

    harness_files = sorted(
        func_dir.glob("iter_*_harness.c"),
        key=lambda p: int(p.stem.split("_")[1])
    )
    if not harness_files:
        return []

    # GT properties (compute once)
    try:
        _, gt_harness_props = get_properties(func_name, gt_path)
    except Exception as e:
        if verbose:
            print(f"  [SKIP] {func_name}: GT property extraction failed: {e}")
        return []

    rows = []
    for h in harness_files:
        iteration = int(h.stem.split("_")[1])
        try:
            _, llm_harness_props = get_properties(func_name, h)
        except Exception as e:
            if verbose:
                print(f"  [SKIP] {func_name} iter {iteration}: {e}")
            continue

        m = match_properties(gt_harness_props, llm_harness_props)
        rows.append(IterRecallRow(
            func         = func_name,
            iteration    = iteration,
            recall       = m["recall"],
            recall_fuzzy = m["recall_fuzzy"],
            gt_count     = m["gt_count"],
            llm_count    = m["llm_count"],
            matched      = m["matched_count_fuzzy"],
        ))
        if verbose:
            print(f"  iter {iteration}: recall={m['recall_fuzzy']:.0%} "
                  f"({m['matched_count_fuzzy']}/{m['gt_count']})")
    return rows


# ── aggregate across all functions ───────────────────────────────────────────

def aggregate(rows: list[IterRecallRow]) -> dict[int, dict]:
    """Mean recall per iteration, over functions that have that iteration."""
    from collections import defaultdict
    buckets: dict[int, list[float]] = defaultdict(list)
    for r in rows:
        buckets[r.iteration].append(r.recall_fuzzy)
    return {
        i: {
            "n":          len(v),
            "mean_recall": sum(v) / len(v),
        }
        for i, v in sorted(buckets.items())
    }


# ── main ─────────────────────────────────────────────────────────────────────

def run_variant(variant: str, save: bool = False, verbose: bool = False):
    variant_dir = RESULTS_DIR / variant
    if not variant_dir.exists():
        print(f"[SKIP] {variant}: directory not found")
        return

    _, func_names = zip(*PILOT_FUNCTIONS)
    all_rows: list[IterRecallRow] = []

    print(f"\n{'='*60}")
    print(f"Variant: {variant}")
    print(f"{'='*60}")

    for func_name in func_names:
        if func_name not in FUNC_CONFIGS:
            continue
        if verbose:
            print(f"\n{func_name}:")
        rows = compute_iter_recall(func_name, variant_dir, verbose=verbose)
        all_rows.extend(rows)
        if not verbose and rows:
            iters = [r.iteration for r in rows]
            recalls = [f"iter{r.iteration}={r.recall_fuzzy:.0%}" for r in rows]
            if len(rows) > 1:
                print(f"  {func_name}: {' → '.join(recalls)}")

    agg = aggregate(all_rows)
    print(f"\n{'─'*60}")
    print(f"{'Iteration':<12} {'N funcs':^10} {'Mean recall':^12}")
    print(f"{'─'*60}")
    for i, d in agg.items():
        print(f"  iter {i:<6}  {d['n']:^10}  {d['mean_recall']:^12.1%}")
    print(f"{'─'*60}")

    # Key finding: iter 1 vs last iteration
    if 1 in agg:
        iter1_recall = agg[1]["mean_recall"]
        last_i = max(agg)
        last_recall = agg[last_i]["mean_recall"]
        delta = last_recall - iter1_recall
        print(f"\nIter 1 → Iter {last_i}: {iter1_recall:.1%} → {last_recall:.1%} "
              f"(Δ = {delta:+.1%})")

    if save:
        out = {
            "variant": variant,
            "rows": [
                {"func": r.func, "iter": r.iteration,
                 "recall_fuzzy": round(r.recall_fuzzy, 3),
                 "gt_count": r.gt_count, "matched": r.matched}
                for r in all_rows
            ],
            "aggregate": {str(k): v for k, v in agg.items()},
        }
        out_path = EVAL_DIR / f"iter_recall_{variant}.json"
        out_path.write_text(json.dumps(out, indent=2))
        print(f"\nSaved → {out_path}")

    return agg


def main():
    parser = argparse.ArgumentParser(description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--variant", default="feedback_loop_C_claude")
    parser.add_argument("--all-variants", action="store_true")
    parser.add_argument("--save",    action="store_true")
    parser.add_argument("--verbose", action="store_true")
    args = parser.parse_args()

    variants = (
        ["feedback_loop_A_claude", "feedback_loop_B_claude",
         "feedback_loop_C_claude", "feedback_loop_D_claude"]
        if args.all_variants else [args.variant]
    )

    for v in variants:
        run_variant(v, save=args.save, verbose=args.verbose)


if __name__ == "__main__":
    main()
