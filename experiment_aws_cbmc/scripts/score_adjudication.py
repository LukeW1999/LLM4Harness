#!/usr/bin/env python3
"""
score_adjudication.py — score a filled worksheet against the rule's labels.

Reads `adjudication/worksheet.md` (each card's `rating:` line filled with NW,
Del, Nar or Unres) and `adjudication/key.json`, and reports per-group agreement,
overall agreement, and Cohen's kappa. Unfilled cards are skipped and counted.

Usage:  python3 score_adjudication.py [--worksheet PATH] [--rater NAME]
"""
import argparse, json, re
from collections import Counter
from pathlib import Path

BASE = Path(__file__).resolve().parent.parent
VALID = {"NW", "DEL", "NAR", "UNRES"}
CANON = {"NW": "NW", "DEL": "Del", "NAR": "Nar", "UNRES": "Unres"}

def read_ratings(path):
    out = {}
    idx = None
    for line in path.read_text().splitlines():
        m = re.match(r"^## (\d+)\. ", line)
        if m:
            idx = int(m.group(1))
            continue
        m = re.match(r"^rating:\s*(\S+)?\s*$", line)
        if m and idx is not None:
            v = (m.group(1) or "").strip().upper()
            out[idx] = CANON[v] if v in VALID else None
            idx = None
    return out

def kappa(a, b):
    """Cohen's kappa for two label sequences."""
    n = len(a)
    if not n:
        return float("nan")
    po = sum(x == y for x, y in zip(a, b)) / n
    ca, cb = Counter(a), Counter(b)
    pe = sum((ca[l] / n) * (cb[l] / n) for l in set(ca) | set(cb))
    return 1.0 if pe == 1 else (po - pe) / (1 - pe)

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--worksheet", default=str(BASE / "adjudication" / "worksheet.md"))
    ap.add_argument("--rater", default="rater")
    args = ap.parse_args()

    key = json.load(open(BASE / "adjudication" / "key.json"))
    ratings = read_ratings(Path(args.worksheet))
    rated = [(i, r) for i, r in sorted(ratings.items()) if r]
    blank = len(key) - len(rated)

    mine, theirs, disagreements = [], [], []
    for i, r in rated:
        g = key[i - 1]
        mine.append(r)
        theirs.append(g["label"])
        if r != g["label"]:
            disagreements.append((i, g["cond"], g["func"], g["n"], g["label"], r))

    agree = sum(x == y for x, y in zip(mine, theirs))
    print(f"rater: {args.rater}")
    print(f"rated {len(rated)} of {len(key)} groups ({blank} left blank)")
    if not rated:
        return
    print(f"agreement: {agree}/{len(rated)} = {100*agree/len(rated):.1f}%")
    print(f"Cohen's kappa: {kappa(mine, theirs):.3f}")
    mutants = sum(g["n"] for g in key)
    rated_mutants = sum(key[i-1]["n"] for i, _ in rated)
    print(f"coverage: {rated_mutants} of {mutants} silenced mutants")
    if disagreements:
        print("\ndisagreements (rule label vs rater):")
        for i, cond, func, n, k, r in disagreements:
            print(f"  #{i:<3} {cond}/{func} ({n} mutants): rule={k} rater={r}")

if __name__ == "__main__":
    main()
