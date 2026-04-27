#!/usr/bin/env python3
"""
compute_kappa.py
================
Compute Cohen's kappa for inter-annotator agreement on the taxonomy classification.

Usage:
    python compute_kappa.py --second annotator2.csv

The second annotator's CSV must have columns: id, your_category
The annotation_key.csv (gold labels) is loaded from the same directory.
"""

import csv
import argparse
from pathlib import Path
from collections import Counter

SCRIPT_DIR = Path(__file__).parent

CATEGORIES = [
    "VALIDITY_PRED", "LEN_INVARIANT", "FRAME_COND", "STRUCT_PTR",
    "LEN_CHANGE", "TYPE_VARIANT", "OVERFLOW_ARITH", "ZERO_CHECK",
    "RETVAL_SPEC", "CBMC_BUILTIN",
]


def cohen_kappa(labels1, labels2):
    """Compute Cohen's kappa for two lists of labels."""
    assert len(labels1) == len(labels2)
    n = len(labels1)

    # Observed agreement
    agree = sum(1 for a, b in zip(labels1, labels2) if a == b)
    p_o = agree / n

    # Expected agreement
    counts1 = Counter(labels1)
    counts2 = Counter(labels2)
    all_cats = set(counts1) | set(counts2)
    p_e = sum((counts1.get(c, 0) / n) * (counts2.get(c, 0) / n) for c in all_cats)

    kappa = (p_o - p_e) / (1 - p_e) if p_e < 1 else 1.0
    return kappa, p_o, p_e


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--second", required=True, help="CSV from second annotator")
    parser.add_argument("--key", default=str(SCRIPT_DIR / "annotation_key.csv"),
                        help="Key CSV with gold labels")
    args = parser.parse_args()

    # Load gold labels
    gold = {}
    with open(args.key) as f:
        for row in csv.DictReader(f):
            gold[row["id"]] = row["category"]

    # Load second annotator
    second = {}
    with open(args.second) as f:
        for row in csv.DictReader(f):
            cat = row.get("your_category", "").strip()
            if cat:
                second[row["id"]] = cat

    # Match on common IDs
    common_ids = sorted(set(gold) & set(second))
    missing_in_second = set(gold) - set(second)

    if missing_in_second:
        print(f"WARNING: {len(missing_in_second)} items not annotated by second annotator: {missing_in_second}")

    if not common_ids:
        print("ERROR: No common IDs found between key and second annotator CSV.")
        return

    labels1 = [gold[i] for i in common_ids]
    labels2 = [second[i] for i in common_ids]

    kappa, p_o, p_e = cohen_kappa(labels1, labels2)

    print(f"\n{'='*50}")
    print(f"  Inter-annotator Agreement (n={len(common_ids)})")
    print(f"{'='*50}")
    print(f"  Observed agreement (p_o): {p_o:.3f} ({p_o*100:.1f}%)")
    print(f"  Expected agreement (p_e): {p_e:.3f}")
    print(f"  Cohen's kappa:            {kappa:.3f}")

    if kappa >= 0.80:
        interp = "Almost perfect"
    elif kappa >= 0.61:
        interp = "Substantial"
    elif kappa >= 0.41:
        interp = "Moderate"
    elif kappa >= 0.21:
        interp = "Fair"
    else:
        interp = "Slight or poor"
    print(f"  Interpretation:           {interp}")

    # Per-category breakdown
    print(f"\n  Per-category agreement:")
    cat_agree = Counter()
    cat_total = Counter()
    for a, b in zip(labels1, labels2):
        cat_total[a] += 1
        if a == b:
            cat_agree[a] += 1

    print(f"  {'Category':<22} {'Agree':>6} {'Total':>6} {'%':>6}")
    print(f"  {'-'*44}")
    for cat in sorted(cat_total, key=lambda c: -cat_total[c]):
        t = cat_total[cat]
        a = cat_agree[cat]
        print(f"  {cat:<22} {a:>6} {t:>6} {a/t*100:>5.0f}%")

    # Confusion pairs (where they disagree)
    disagree = [(gold[i], second[i]) for i in common_ids if gold[i] != second[i]]
    if disagree:
        print(f"\n  Disagreement pairs ({len(disagree)}):")
        for a, b in Counter(disagree).most_common(10):
            n = Counter(disagree)[(a, b)]
            print(f"    {a} vs {b}: {n}")


if __name__ == "__main__":
    main()
