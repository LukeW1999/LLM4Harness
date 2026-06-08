#!/usr/bin/env python3
"""
compute_kappa_pilot.py
======================
Compute Cohen's kappa for inter-annotator agreement on the pilot annotation.

Usage:
    python compute_kappa_pilot.py --a1 annotator1.csv --a2 annotator2.csv

Both CSVs must have columns: id, your_taxonomy, your_attribution
(The columns match pilot_worksheet.csv and pilot_blank.csv)

Outputs:
  - kappa_taxonomy: validity_predicate / length_invariant / frame_condition
  - kappa_attribution: never_generated / deleted_sacrifice / weakened
  - Per-row disagreements for inspection
"""

import csv
import argparse
from pathlib import Path
from collections import Counter

TAXONOMY_LABELS = {"validity_predicate", "length_invariant", "frame_condition"}
ATTRIBUTION_LABELS = {"never_generated", "deleted_sacrifice", "weakened"}


def cohen_kappa(labels1, labels2):
    assert len(labels1) == len(labels2), "Label lists must be same length"
    n = len(labels1)
    agree = sum(1 for a, b in zip(labels1, labels2) if a == b)
    p_o = agree / n
    c1 = Counter(labels1)
    c2 = Counter(labels2)
    all_cats = set(c1) | set(c2)
    p_e = sum((c1.get(c, 0) / n) * (c2.get(c, 0) / n) for c in all_cats)
    kappa = (p_o - p_e) / (1 - p_e) if p_e < 1 else 1.0
    return round(kappa, 4), round(p_o, 4), round(p_e, 4)


def load_csv(path):
    rows = {}
    with open(path, encoding='utf-8') as f:
        for row in csv.DictReader(f):
            pid = row.get('id', '').strip()
            if pid:
                rows[pid] = {
                    'taxonomy': row.get('your_taxonomy', '').strip().lower(),
                    'attribution': row.get('your_attribution', '').strip().lower(),
                    'func': row.get('func_name', '').strip(),
                    'assert': row.get('assert_text', '').strip(),
                }
    return rows


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--a1", required=True, help="Annotator 1 CSV (pilot_worksheet.csv after filling)")
    parser.add_argument("--a2", required=True, help="Annotator 2 CSV (pilot_blank.csv after filling)")
    parser.add_argument("--show-disagreements", action="store_true", help="Print disagreeing rows")
    args = parser.parse_args()

    a1 = load_csv(args.a1)
    a2 = load_csv(args.a2)

    common_ids = sorted(set(a1.keys()) & set(a2.keys()))
    if not common_ids:
        print("ERROR: No matching IDs between the two files.")
        return

    print(f"\nMatched {len(common_ids)} annotations\n")

    # Check for missing/invalid labels
    issues = 0
    for pid in common_ids:
        for ann, name in [(a1[pid], "A1"), (a2[pid], "A2")]:
            if ann['taxonomy'] not in TAXONOMY_LABELS:
                print(f"  WARNING {pid} {name}: invalid taxonomy '{ann['taxonomy']}'")
                issues += 1
            if ann['attribution'] not in ATTRIBUTION_LABELS:
                print(f"  WARNING {pid} {name}: invalid attribution '{ann['attribution']}'")
                issues += 1
    if issues:
        print(f"\n{issues} invalid label(s) found. Fix before computing kappa.\n")
        return

    tax1 = [a1[i]['taxonomy'] for i in common_ids]
    tax2 = [a2[i]['taxonomy'] for i in common_ids]
    att1 = [a1[i]['attribution'] for i in common_ids]
    att2 = [a2[i]['attribution'] for i in common_ids]

    k_tax, po_tax, pe_tax = cohen_kappa(tax1, tax2)
    k_att, po_att, pe_att = cohen_kappa(att1, att2)

    print("=" * 50)
    print("TAXONOMY (validity_predicate / length_invariant / frame_condition)")
    print(f"  Cohen's κ = {k_tax}  (p_o={po_tax}, p_e={pe_tax})")
    print(f"  → {'PASS ✅' if k_tax >= 0.8 else 'FAIL ❌ — need κ ≥ 0.8'}")

    print()
    print("ATTRIBUTION (never_generated / deleted_sacrifice / weakened)")
    print(f"  Cohen's κ = {k_att}  (p_o={po_att}, p_e={pe_att})")
    print(f"  → {'PASS ✅' if k_att >= 0.8 else 'FAIL ❌ — need κ ≥ 0.8'}")
    print("=" * 50)

    if k_tax >= 0.8 and k_att >= 0.8:
        print("\n✅ Both kappa gates passed. Full annotation can proceed.")
    else:
        print("\n❌ At least one gate failed. Discuss disagreements and refine codebook.")

    # Show disagreements
    if args.show_disagreements or k_tax < 0.8 or k_att < 0.8:
        print("\n--- Taxonomy disagreements ---")
        for pid in common_ids:
            if a1[pid]['taxonomy'] != a2[pid]['taxonomy']:
                print(f"  {pid} [{a1[pid]['func']}]")
                print(f"    assert: {a1[pid]['assert'][:80]}")
                print(f"    A1: {a1[pid]['taxonomy']}  |  A2: {a2[pid]['taxonomy']}")

        print("\n--- Attribution disagreements ---")
        for pid in common_ids:
            if a1[pid]['attribution'] != a2[pid]['attribution']:
                print(f"  {pid} [{a1[pid]['func']}]")
                print(f"    assert: {a1[pid]['assert'][:80]}")
                print(f"    A1: {a1[pid]['attribution']}  |  A2: {a2[pid]['attribution']}")


if __name__ == "__main__":
    main()
