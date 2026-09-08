#!/usr/bin/env python3
"""
audit_coverage.py — which numbers in the paper does the registry actually check?

`paper_numbers_640.py` reports 0 mismatch over the claims someone thought to
register. That says nothing about a number in the paper that no entry covers,
which is where drift hides. This pulls every numeric literal out of paper.tex,
drops the ones that are not empirical claims (section numbers, years, page
counts, macro arguments), and reports which of the rest no registry entry
matches.

Usage:  python3 scripts/audit_coverage.py [--tex PATH] [--show-covered]
"""
import argparse, importlib.util, re, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXP = HERE.parent
DEFAULT_TEX = Path.home() / "research/papers/EOY2/paper.tex"

# Numeric contexts that are not empirical claims about our data.
SKIP_LINE = re.compile(r"\\(documentclass|usepackage|newcommand|renewcommand|setlength|"
                       r"scalebox|includegraphics|acmConference|label|ref|cite|ccsdesc|"
                       r"lstset|tcbset|definecolor|node|draw|tikz|hspace|vspace|"
                       r"newtcolorbox|begin\{tikzpicture\}|cmidrule|multicolumn)|"
                       r"black!|colback|colframe|boxrule|borderline|/\.style|"
                       r"basicstyle|aboveskip|belowskip|xleftmargin|tabcolsep")
SKIP_TOKEN = re.compile(r"^(0|1|2|3|4|5|6|7|8|9|10|11|12|100|2024|2025|2026|2027)$")

def paper_numbers(tex):
    """(number, context) for every numeric literal that looks like a claim."""
    out = []
    for raw in tex.splitlines():
        if not raw.strip() or raw.lstrip().startswith("%") or SKIP_LINE.search(raw):
            continue
        line = re.sub(r"\\(S|ref|label|cite[a-z]*)\{[^}]*\}", " ", raw)
        line = re.sub(r"\\S\\?ref", " ", line)
        for m in re.finditer(r"(?<![\w.])(\d{1,3}(?:[,{}\\ ]?\d{3})*(?:\.\d+)?)(?![\w])", line):
            tok = m.group(1)
            clean = tok.replace("{,}", "").replace(",", "").replace(" ", "").replace("\\", "")
            if SKIP_TOKEN.match(clean):
                continue
            try:
                val = float(clean)
            except ValueError:
                continue
            ctx = line[max(0, m.start() - 55):m.start() + 55].strip()
            out.append((val, ctx))
    return out

def registry_values():
    spec = importlib.util.spec_from_file_location("pn", HERE / "paper_numbers_640.py")
    mod = importlib.util.module_from_spec(spec)
    argv, sys.argv = sys.argv, ["paper_numbers_640"]
    try:
        spec.loader.exec_module(mod)
    except SystemExit:
        pass
    finally:
        sys.argv = argv
    return {float(claimed) for _loc, _desc, claimed, _fn, _tol in mod.R}, mod.R

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--tex", default=str(DEFAULT_TEX))
    ap.add_argument("--show-covered", action="store_true")
    args = ap.parse_args()

    tex = Path(args.tex).read_text()
    claims = paper_numbers(tex)
    reg, entries = registry_values()

    covered, uncovered = [], []
    for val, ctx in claims:
        if any(abs(val - r) <= max(0.05, abs(r) * 0.001) for r in reg):
            covered.append((val, ctx))
        else:
            uncovered.append((val, ctx))

    seen, uniq = set(), []
    for val, ctx in uncovered:
        if (val, ctx[:40]) in seen:
            continue
        seen.add((val, ctx[:40]))
        uniq.append((val, ctx))

    print(f"registry entries      : {len(entries)}")
    print(f"numeric claims in tex : {len(claims)}")
    print(f"covered by registry   : {len(covered)}")
    print(f"NOT covered           : {len(uniq)} distinct\n")
    for val, ctx in uniq:
        print(f"  {val:>10}  ...{ctx}...")
    if args.show_covered:
        print("\n--- covered ---")
        for val, ctx in covered:
            print(f"  {val:>10}  ...{ctx[:70]}...")

if __name__ == "__main__":
    main()
