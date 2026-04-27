#!/usr/bin/env python3
"""
compare_conditions.py - Compare cross-verification results across conditions.

Loads condA and condB (and optionally original) cross_verify_results JSON files
and produces side-by-side comparison tables + delta analysis.

Usage:
    python compare_conditions.py                    # compare condA vs condB
    python compare_conditions.py --with-original    # include original as upper bound
"""

import json
import argparse
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
EVAL_DIR = SCRIPT_DIR.parent / "evaluation"

COND_LABELS = {
    "original": "Original (w/ macros)",
    "A": "Cond-A (NL-guided)",
    "B": "Cond-B (Code-only)",
}

# ── Function taxonomy ────────────────────────────────────────────────────────
#
# Type:
#   ARITHMETIC   – pure math functions; postcondition is a numeric identity
#   STRUCT_INIT  – initialise/destroy; postcondition is field assignments + validity
#   STATE_MUTATE – partial field update; postcondition is specific field changes + immutability
#   DATA_COPY    – data movement; postcondition is bytes copied + length update
#   POINTER_LINK – linked-structure pointer changes after insert/remove
#   QUERY        – read-only accessor; postcondition is return value + invariant
#
# NL quality in Condition A header:
#   FULL     – explicit Requires:/Ensures: annotations for the target function
#   PARTIAL  – informal description that hints at postcondition (no structured keywords)
#   NONE     – no useful annotation (bare declaration or math inline)

TAXONOMY = {
    # func : (type,           nl_quality)
    "aws_add_size_checked":         ("ARITHMETIC",   "NONE"),
    "aws_add_size_saturating":      ("ARITHMETIC",   "NONE"),
    "aws_mul_size_checked":         ("ARITHMETIC",   "NONE"),
    "aws_mul_size_saturating":      ("ARITHMETIC",   "NONE"),
    "aws_is_power_of_two":          ("ARITHMETIC",   "NONE"),
    "aws_round_up_to_power_of_two": ("ARITHMETIC",   "NONE"),

    "aws_byte_buf_init":            ("STRUCT_INIT",  "FULL"),
    "aws_byte_buf_clean_up":        ("STRUCT_INIT",  "NONE"),
    "aws_byte_buf_from_array":      ("STRUCT_INIT",  "NONE"),
    "aws_byte_buf_from_empty_array":("STRUCT_INIT",  "NONE"),
    "aws_linked_list_init":         ("STRUCT_INIT",  "NONE"),
    "aws_linked_list_node_reset":   ("STRUCT_INIT",  "NONE"),

    "aws_byte_buf_secure_zero":     ("STATE_MUTATE", "PARTIAL"),
    "aws_byte_buf_reset":           ("STATE_MUTATE", "PARTIAL"),
    "aws_byte_buf_append":          ("STATE_MUTATE", "FULL"),
    "aws_byte_buf_write_u8":        ("STATE_MUTATE", "PARTIAL"),
    "aws_byte_cursor_advance":      ("STATE_MUTATE", "PARTIAL"),

    "aws_array_list_back":          ("DATA_COPY",    "FULL"),
    "aws_array_list_front":         ("DATA_COPY",    "NONE"),
    "aws_array_list_get_at":        ("DATA_COPY",    "NONE"),

    "aws_linked_list_push_back":    ("POINTER_LINK", "FULL"),
    "aws_linked_list_push_front":   ("POINTER_LINK", "NONE"),
    "aws_linked_list_pop_back":     ("POINTER_LINK", "NONE"),
    "aws_linked_list_pop_front":    ("POINTER_LINK", "NONE"),
    "aws_array_list_push_back":     ("POINTER_LINK", "NONE"),
    "aws_array_list_pop_back":      ("POINTER_LINK", "NONE"),
    "aws_array_list_clear":         ("POINTER_LINK", "NONE"),

    "aws_array_list_length":        ("QUERY",        "PARTIAL"),
    "aws_array_list_capacity":      ("QUERY",        "PARTIAL"),
    "aws_byte_buf_eq":              ("QUERY",        "PARTIAL"),
}


def load(cond: str) -> dict:
    """Load cross_verify_results for a condition; return {func_name: row_dict}."""
    path = EVAL_DIR / f"cross_verify_results_cond{cond}.json"
    if not path.exists():
        return {}
    rows = json.loads(path.read_text())
    return {r["func"]: r for r in rows}


def fmt_pct(v) -> str:
    if v is None:
        return "  N/A "
    return f"{v*100:5.1f}%"


def fmt_adeq(v) -> str:
    if not v:
        return "N/A     "
    return f"{v:<8}"


def print_recall_table(data_a: dict, data_b: dict, data_orig: dict = None):
    has_orig = bool(data_orig)
    all_funcs = sorted(set(data_a) | set(data_b))

    w = 130
    print(f"\n{'='*w}")
    print(f"{'ASSERT RECALL / PRECISION COMPARISON':^{w}}")
    print(f"{'='*w}")

    if has_orig:
        hdr = (f"{'Function':<35} "
               f"{'Orig Rec':^9} {'A Rec':^9} {'B Rec':^9} "
               f"{'Δ(A-B)':^8} "
               f"{'Orig Prec':^10} {'A Prec':^9} {'B Prec':^9} "
               f"{'Class-A':<20} {'Class-B':<20}")
    else:
        hdr = (f"{'Function':<35} "
               f"{'A Recall':^9} {'B Recall':^9} {'Δ(A-B)':^8} "
               f"{'A Prec':^9} {'B Prec':^9} "
               f"{'Class-A':<22} {'Class-B':<22}")
    print(hdr)
    print("-" * w)

    for fn in all_funcs:
        ra = data_a.get(fn, {})
        rb = data_b.get(fn, {})
        ro = data_orig.get(fn, {}) if has_orig else {}

        rec_a = ra.get("harness_recall")
        rec_b = rb.get("harness_recall")
        prec_a = ra.get("harness_precision")
        prec_b = rb.get("harness_precision")
        cls_a = ra.get("classification", "N/A")
        cls_b = rb.get("classification", "N/A")

        delta_str = "  N/A "
        if rec_a is not None and rec_b is not None:
            delta = rec_a - rec_b
            sign = "+" if delta >= 0 else ""
            delta_str = f"{sign}{delta*100:5.1f}%"

        if has_orig:
            rec_o = ro.get("harness_recall")
            prec_o = ro.get("harness_precision")
            row = (f"{fn:<35} "
                   f"{fmt_pct(rec_o):^9} {fmt_pct(rec_a):^9} {fmt_pct(rec_b):^9} "
                   f"{delta_str:^8} "
                   f"{fmt_pct(prec_o):^10} {fmt_pct(prec_a):^9} {fmt_pct(prec_b):^9} "
                   f"{cls_a:<20} {cls_b:<20}")
        else:
            row = (f"{fn:<35} "
                   f"{fmt_pct(rec_a):^9} {fmt_pct(rec_b):^9} {delta_str:^8} "
                   f"{fmt_pct(prec_a):^9} {fmt_pct(prec_b):^9} "
                   f"{cls_a:<22} {cls_b:<22}")

        # Highlight large deltas
        marker = ""
        if rec_a is not None and rec_b is not None:
            if abs(rec_a - rec_b) >= 0.30:
                marker = " ◄◄"
            elif abs(rec_a - rec_b) >= 0.15:
                marker = " ◄"
        print(row + marker)

    print("=" * w)


def print_assume_table(data_a: dict, data_b: dict, data_orig: dict = None):
    has_orig = bool(data_orig)
    all_funcs = sorted(set(data_a) | set(data_b))

    w = 110
    print(f"\n{'='*w}")
    print(f"{'ASSUME ADEQUACY COMPARISON':^{w}}")
    print(f"{'='*w}")

    hdr = (f"{'Function':<35} "
           f"{'GT-Asm':^7} "
           f"{'A-Asm':^7} {'A-Adeq':^10} "
           f"{'B-Asm':^7} {'B-Adeq':^10} "
           f"{'A-OC':^7} {'B-OC':^7}")
    if has_orig:
        hdr = (f"{'Function':<35} "
               f"{'GT-Asm':^7} "
               f"{'Orig-Asm':^9} {'Orig-Adeq':^11} "
               f"{'A-Asm':^7} {'A-Adeq':^10} "
               f"{'B-Asm':^7} {'B-Adeq':^10}")
    print(hdr)
    print("-" * w)

    for fn in all_funcs:
        ra = data_a.get(fn, {})
        rb = data_b.get(fn, {})
        ro = data_orig.get(fn, {}) if has_orig else {}

        gt_n = ra.get("gt_assume_count", rb.get("gt_assume_count", "?"))

        if has_orig:
            row = (f"{fn:<35} "
                   f"{str(gt_n):^7} "
                   f"{str(ro.get('llm_assume_count','?')):^9} {fmt_adeq(ro.get('assume_adequacy')):^11} "
                   f"{str(ra.get('llm_assume_count','?')):^7} {fmt_adeq(ra.get('assume_adequacy')):^10} "
                   f"{str(rb.get('llm_assume_count','?')):^7} {fmt_adeq(rb.get('assume_adequacy')):^10}")
        else:
            oc_a = "YES" if ra.get("over_constrained") else "no"
            oc_b = "YES" if rb.get("over_constrained") else "no"
            row = (f"{fn:<35} "
                   f"{str(gt_n):^7} "
                   f"{str(ra.get('llm_assume_count','?')):^7} {fmt_adeq(ra.get('assume_adequacy')):^10} "
                   f"{str(rb.get('llm_assume_count','?')):^7} {fmt_adeq(rb.get('assume_adequacy')):^10} "
                   f"{oc_a:^7} {oc_b:^7}")
        print(row)

    print("=" * w)


def print_aggregates(data_a: dict, data_b: dict, data_orig: dict = None):
    def agg(data: dict) -> dict:
        rows = list(data.values())
        n = len(rows)
        if n == 0:
            return {}
        return {
            "n": n,
            "avg_recall":    sum(r.get("harness_recall", 0)    for r in rows) / n,
            "avg_precision": sum(r.get("harness_precision", 0) for r in rows) / n,
            "strong_equiv":  sum(1 for r in rows if r.get("classification") == "STRONG_EQUIV") / n,
            "weak_equiv":    sum(1 for r in rows if r.get("classification") == "WEAK_EQUIV") / n,
            "not_equiv":     sum(1 for r in rows if r.get("classification") == "NOT_EQUIV") / n,
            "verify_only":   sum(1 for r in rows if r.get("classification") == "VERIFY_EQUIV_ONLY") / n,
            "over_const":    sum(1 for r in rows if r.get("over_constrained")) / n,
            "tight":         sum(1 for r in rows if r.get("assume_adequacy") == "TIGHT") / n,
            "redundant":     sum(1 for r in rows if r.get("assume_adequacy") == "REDUNDANT") / n,
            "under":         sum(1 for r in rows if r.get("assume_adequacy") == "UNDER") / n,
        }

    a = agg(data_a)
    b = agg(data_b)
    o = agg(data_orig) if data_orig else {}

    print("\n" + "=" * 80)
    print(f"{'AGGREGATE COMPARISON':^80}")
    print("=" * 80)

    def row(label, key, fmt="{:.1%}"):
        va = fmt.format(a.get(key, 0)) if a else "N/A"
        vb = fmt.format(b.get(key, 0)) if b else "N/A"
        vo = fmt.format(o.get(key, 0)) if o else ""
        if o:
            print(f"  {label:<35} {vo:>10}  {va:>10}  {vb:>10}")
        else:
            print(f"  {label:<35} {va:>12}  {vb:>12}")

    if o:
        print(f"  {'Metric':<35} {'Original':>10}  {'Cond-A':>10}  {'Cond-B':>10}")
        print(f"  {'n':<35} {o.get('n',0):>10}  {a.get('n',0):>10}  {b.get('n',0):>10}")
    else:
        print(f"  {'Metric':<35} {'Cond-A':>12}  {'Cond-B':>12}")
        print(f"  {'n':<35} {a.get('n',0):>12}  {b.get('n',0):>12}")
    print()
    row("Avg assert recall (GT→LLM)",    "avg_recall")
    row("Avg assert precision",           "avg_precision")
    print()
    row("STRONG_EQUIV rate",              "strong_equiv")
    row("WEAK_EQUIV rate",                "weak_equiv")
    row("VERIFY_EQUIV_ONLY rate",         "verify_only")
    row("NOT_EQUIV rate",                 "not_equiv")
    print()
    row("Assume TIGHT rate",              "tight")
    row("Assume REDUNDANT rate",          "redundant")
    row("Assume UNDER rate",              "under")
    row("Over-constrained rate",          "over_const")
    print("=" * 80)


def print_delta_ranking(data_a: dict, data_b: dict):
    """Show functions ranked by |recall_A - recall_B| — the biggest gaps."""
    common = set(data_a) & set(data_b)
    deltas = []
    for fn in common:
        ra = data_a[fn].get("harness_recall")
        rb = data_b[fn].get("harness_recall")
        if ra is not None and rb is not None:
            deltas.append((fn, ra, rb, ra - rb))

    deltas.sort(key=lambda x: abs(x[3]), reverse=True)

    print("\n" + "=" * 70)
    print(f"{'FUNCTIONS WITH LARGEST RECALL DELTA (A - B)':^70}")
    print("=" * 70)
    print(f"  {'Function':<35} {'A-Rec':^8} {'B-Rec':^8} {'Δ':^8}")
    print("  " + "-" * 65)
    for fn, ra, rb, delta in deltas[:15]:
        sign = "+" if delta >= 0 else ""
        marker = "  ◄◄" if abs(delta) >= 0.3 else ("  ◄" if abs(delta) >= 0.15 else "")
        print(f"  {fn:<35} {ra*100:6.1f}%  {rb*100:6.1f}%  {sign}{delta*100:5.1f}%{marker}")
    print("=" * 70)


def print_taxonomy_analysis(data_a: dict, data_b: dict, version: str = ""):
    """Group recall deltas by function type and NL quality."""
    label = f"TAXONOMY ANALYSIS{' - ' + version if version else ''}"
    w = 110
    print(f"\n{'='*w}")
    print(f"{label:^{w}}")
    print(f"{'='*w}")

    # Build per-type and per-NL aggregates
    by_type = {}
    by_nl = {}
    rows_detail = []

    for fn, (ftype, nl_q) in sorted(TAXONOMY.items()):
        ra_row = data_a.get(fn, {})
        rb_row = data_b.get(fn, {})
        rec_a = ra_row.get("harness_recall")
        rec_b = rb_row.get("harness_recall")
        if rec_a is None and rec_b is None:
            continue

        delta = (rec_a or 0) - (rec_b or 0)
        rows_detail.append((fn, ftype, nl_q, rec_a or 0, rec_b or 0, delta))

        by_type.setdefault(ftype, []).append(delta)
        by_nl.setdefault(nl_q, []).append(delta)

    # Per-function table
    print(f"\n{'Function':<42} {'Type':<14} {'NL':^8} {'A-Rec':^8} {'B-Rec':^8} {'Δ(A-B)':^8}")
    print("  " + "-" * (w - 2))
    for fn, ftype, nl_q, rec_a, rec_b, delta in rows_detail:
        sign = "+" if delta >= 0 else ""
        marker = "  ◄◄" if abs(delta) >= 0.30 else ("  ◄" if abs(delta) >= 0.15 else "")
        print(f"  {fn:<40} {ftype:<14} {nl_q:^8} {rec_a*100:6.1f}%  {rec_b*100:6.1f}%  "
              f"{sign}{delta*100:5.1f}%{marker}")

    # Per-type aggregate
    print(f"\n{'Type Aggregate':^{w}}")
    print(f"  {'Type':<16} {'n':^4} {'Avg A-Rec':^10} {'Avg B-Rec':^10} {'Avg Δ(A-B)':^12} {'A>B':^5} {'B>A':^5} {'Tie':^5}")
    print("  " + "-" * 75)
    type_order = ["ARITHMETIC", "STRUCT_INIT", "STATE_MUTATE", "DATA_COPY", "POINTER_LINK", "QUERY"]
    for ftype in type_order:
        deltas = by_type.get(ftype, [])
        if not deltas:
            continue
        n = len(deltas)
        # Get matching rows
        type_rows = [(rec_a, rec_b, dd) for _, t, _, rec_a, rec_b, dd in rows_detail if t == ftype]
        avg_a = sum(r[0] for r in type_rows) / n
        avg_b = sum(r[1] for r in type_rows) / n
        avg_d = sum(r[2] for r in type_rows) / n
        a_wins = sum(1 for d in deltas if d > 0.01)
        b_wins = sum(1 for d in deltas if d < -0.01)
        ties = n - a_wins - b_wins
        sign = "+" if avg_d >= 0 else ""
        print(f"  {ftype:<16} {n:^4} {avg_a*100:8.1f}%  {avg_b*100:8.1f}%  "
              f"{sign}{avg_d*100:8.1f}%    {a_wins:^5} {b_wins:^5} {ties:^5}")

    # Per-NL-quality aggregate
    print(f"\n{'NL Quality Aggregate':^{w}}")
    print(f"  {'NL Quality':<10} {'n':^4} {'Avg A-Rec':^10} {'Avg B-Rec':^10} {'Avg Δ(A-B)':^12} {'Interpretation'}")
    print("  " + "-" * 80)
    nl_interpretations = {
        "FULL":    "Structured Requires/Ensures → LLM can translate directly",
        "PARTIAL": "Informal desc → LLM must infer formal spec from prose",
        "NONE":    "No annotation → both conditions equally disadvantaged",
    }
    for nl_q in ["FULL", "PARTIAL", "NONE"]:
        deltas = by_nl.get(nl_q, [])
        if not deltas:
            continue
        n = len(deltas)
        nl_rows = [(rec_a, rec_b, dd) for _, _, q, rec_a, rec_b, dd in rows_detail if q == nl_q]
        avg_a = sum(r[0] for r in nl_rows) / n
        avg_b = sum(r[1] for r in nl_rows) / n
        avg_d = sum(r[2] for r in nl_rows) / n
        sign = "+" if avg_d >= 0 else ""
        interp = nl_interpretations.get(nl_q, "")
        print(f"  {nl_q:<10} {n:^4} {avg_a*100:8.1f}%  {avg_b*100:8.1f}%  "
              f"{sign}{avg_d*100:8.1f}%    {interp}")

    print("=" * w)


def main():
    parser = argparse.ArgumentParser(description="Compare A/B cross-verify results")
    parser.add_argument("--with-original", action="store_true",
                        help="Include original condition as upper bound")
    parser.add_argument("--v2", action="store_true",
                        help="Load v2 results (with mandatory assert guidance)")
    parser.add_argument("--both-versions", action="store_true",
                        help="Show side-by-side v1 vs v2 taxonomy analysis")
    args = parser.parse_args()

    if args.both_versions:
        # Load both v1 and v2
        data_a_v1 = load("A")
        data_b_v1 = load("B")
        data_a_v2 = load("A_v2")
        data_b_v2 = load("B_v2")
        print_taxonomy_analysis(data_a_v1, data_b_v1, version="v1 (no assert guidance)")
        print_taxonomy_analysis(data_a_v2, data_b_v2, version="v2 (mandatory assert guidance)")
        return

    suffix = "_v2" if args.v2 else ""
    data_a = load(f"A{suffix}")
    data_b = load(f"B{suffix}")
    data_orig = load("original") if args.with_original else {}

    if not data_a and not data_b:
        print("No results found. Run feedback_loop.py + cross_verify.py first.")
        return

    ver = "v2" if args.v2 else "v1"
    print(f"Version: {ver}")
    if data_a:
        print(f"Loaded Cond-A: {len(data_a)} functions")
    else:
        print("Cond-A: no data yet")

    if data_b:
        print(f"Loaded Cond-B: {len(data_b)} functions")
    else:
        print("Cond-B: no data yet")

    print_recall_table(data_a, data_b, data_orig or None)
    print_assume_table(data_a, data_b, data_orig or None)
    print_aggregates(data_a, data_b, data_orig or None)
    if data_a and data_b:
        print_delta_ranking(data_a, data_b)
        print_taxonomy_analysis(data_a, data_b, version=ver)


if __name__ == "__main__":
    main()
