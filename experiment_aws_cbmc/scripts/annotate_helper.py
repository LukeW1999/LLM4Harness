#!/usr/bin/env python3
"""
annotate_helper.py — Pre-annotation tool for RQ1 taxonomy + sacrifice labelling.

For each function × condition, produces:
  1. Per-missed-GT-assertion row with auto-suggested labels
  2. HTML report for human rater review (confirm/override)
  3. CSV for κ computation

Taxonomy categories (auto-suggested by keyword heuristics):
  - validity_predicate: pointer non-null, return value, error code
  - length_invariant: buf.len, capacity, offset relationships
  - frame_condition: allocator unchanged, pointer identity, side-effect discipline

Sacrifice attribution (from iteration log):
  - never_generated: assertion never appeared in any iteration
  - deleted_sacrifice: appeared, then deleted after CBMC UNKNOWN (fix_unknown context)
  - deleted_correction: appeared, then deleted after CBMC violation (fix_verify context)
  - weakened: appeared but predicate loosened (weaken action)

Usage:
    python3 scripts/annotate_helper.py --condition A_gptoss120b --output annotation_pilot.csv
    python3 scripts/annotate_helper.py --condition A_gptoss120b --html annotation_report.html
    python3 scripts/annotate_helper.py --all-conditions --output annotation_all.csv
"""

import re
import json
import argparse
import csv
import html as html_module
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
RESULTS_DIR = experiment_dir / "results"
EVAL_DIR = experiment_dir / "evaluation"
DATASET_DIR = experiment_dir / "dataset_condA"

# ── Taxonomy heuristics ────────────────────────────────────────────────────────

VALIDITY_KEYWORDS = [
    r'\bnull\b', r'!= NULL', r'== NULL',
    r'\brc\b', r'\breturn\b', r'result\b', r'AWS_OP_SUCCESS', r'AWS_OP_ERR',
    r'aws_op_success', r'aws_op_err',
    r'\berr\b', r'error', r'== 0\b', r'!= 0\b',
    r'is_valid', r'aws_.*_is_valid',
]

LENGTH_KEYWORDS = [
    r'\.len\b', r'\.capacity\b', r'\.current_size\b',
    r'offset', r'length', r'size\b', r'count\b',
    r'\blen\b', r'\bcap\b', r'\bhead\b', r'\btail\b',
    r'buf\.len', r'buf\.capacity',
    r'>=\s*\d', r'<=\s*\d', r'==\s*\d',
    r'initial_len', r'bytes_written',
]

FRAME_KEYWORDS = [
    r'allocator', r'\.allocator\b',
    r'old_', r'__old', r'old\.',
    r'unchanged', r'same\b',
    r'assigns', r'frame',
    r'ptr\b.*==.*ptr\b',
    r'\.impl\b', r'aws_mem_',
]


def suggest_taxonomy(assert_text: str) -> str:
    text = assert_text.lower()
    scores = {"frame_condition": 0, "length_invariant": 0, "validity_predicate": 0}
    for pat in FRAME_KEYWORDS:
        if re.search(pat, text, re.IGNORECASE):
            scores["frame_condition"] += 2
    for pat in LENGTH_KEYWORDS:
        if re.search(pat, text, re.IGNORECASE):
            scores["length_invariant"] += 1
    for pat in VALIDITY_KEYWORDS:
        if re.search(pat, text, re.IGNORECASE):
            scores["validity_predicate"] += 1
    # Return highest scorer; default to validity_predicate
    best = max(scores, key=lambda k: scores[k])
    if scores[best] == 0:
        return "validity_predicate"
    return best


# ── Data loading ───────────────────────────────────────────────────────────────

def load_cross_verify(condition: str) -> dict:
    """Load cross_verify results for a condition. Returns {func_name: entry}."""
    path = EVAL_DIR / f"cross_verify_results_cond{condition}.json"
    if not path.exists():
        return {}
    data = json.loads(path.read_text())
    return {entry["func"]: entry for entry in data}


def load_iteration_log(condition: str, func_name: str) -> list:
    """Load iteration log for a function in a condition."""
    cond_dir = RESULTS_DIR / f"feedback_loop_{condition}"
    log_path = cond_dir / func_name / "iteration_log.json"
    if not log_path.exists():
        return []
    return json.loads(log_path.read_text())


def load_final_harness(condition: str, func_name: str) -> str:
    """Load the final harness text (last iter_N_harness.c)."""
    cond_dir = RESULTS_DIR / f"feedback_loop_{condition}" / func_name
    if not cond_dir.exists():
        return ""
    harnesses = sorted(cond_dir.glob("iter_*_harness.c"),
                       key=lambda p: int(p.stem.split("_")[1]))
    if not harnesses:
        return ""
    return harnesses[-1].read_text()


def load_gt_harness(func_name: str) -> str:
    """Load ground truth harness for a function."""
    for d in DATASET_DIR.iterdir():
        if d.is_dir() and func_name in d.name:
            gt = d / "ground_truth_harness.c"
            if gt.exists():
                return gt.read_text()
    return ""


# ── Attribution logic ──────────────────────────────────────────────────────────

def classify_attribution(assert_text: str, iteration_log: list) -> str:
    """
    Classify a missed GT assertion's attribution:
      - never_generated: never appeared in any iteration log
      - deleted_sacrifice: appeared, deleted in fix_unknown context
      - deleted_correction: appeared, deleted in fix_verify context
      - weakened: appeared but weakened (predicate loosened)
    """
    # Normalise: strip assert() wrapper and whitespace
    def norm(t: str) -> str:
        t = re.sub(r'^\s*assert\s*\(', '', t.strip())
        t = re.sub(r'\s*\)\s*;?\s*$', '', t)
        return t.strip().lower()

    target = norm(assert_text)

    appeared = False
    last_action = None
    last_context = None

    for entry in iteration_log:
        entry_text = norm(entry.get("assert_text", ""))
        # Fuzzy match: one is substring of the other, or edit distance < 5
        if target == entry_text or target in entry_text or entry_text in target:
            appeared = True
            last_action = entry.get("action")
            last_context = entry.get("context", "")

    if not appeared:
        return "never_generated"
    if last_action == "weaken":
        return "weakened"
    if last_action == "delete":
        ctx = last_context or ""
        if "unknown" in ctx.lower():
            return "deleted_sacrifice"
        else:
            return "deleted_correction"
    # Still present (add/weaken but not deleted) — shouldn't reach here for missed assertions
    return "never_generated"


# ── Main annotation ────────────────────────────────────────────────────────────

@dataclass
class AnnotationRow:
    func_name: str
    assert_text: str          # missed GT assertion text
    taxonomy_suggested: str   # auto-suggested category
    taxonomy_human: str       # human override (empty = use suggested)
    attribution_suggested: str  # never_generated / deleted_sacrifice / etc.
    attribution_human: str    # human override
    llm_verify: str           # SUCCESS / UNKNOWN / FAILURE
    condition: str
    notes: str = ""


def annotate_function(func_name: str, condition: str,
                      cv_entry: dict, iteration_log: list) -> list[AnnotationRow]:
    rows = []
    unmatched = cv_entry.get("unmatched_gt_harness", [])
    llm_verify = cv_entry.get("llm_verify", "")

    for gt_text_raw in unmatched:
        # Format: "predicate|assertion" or just text
        gt_text = gt_text_raw.split("|")[0].strip()

        taxonomy = suggest_taxonomy(gt_text)
        attribution = classify_attribution(gt_text, iteration_log)

        rows.append(AnnotationRow(
            func_name=func_name,
            assert_text=gt_text,
            taxonomy_suggested=taxonomy,
            taxonomy_human="",
            attribution_suggested=attribution,
            attribution_human="",
            llm_verify=llm_verify,
            condition=condition,
        ))
    return rows


def run_annotation(condition: str) -> list[AnnotationRow]:
    cv_data = load_cross_verify(condition)
    all_rows = []
    for func_name, cv_entry in cv_data.items():
        if not cv_entry.get("unmatched_gt_harness"):
            continue
        iteration_log = load_iteration_log(condition, func_name)
        rows = annotate_function(func_name, condition, cv_entry, iteration_log)
        all_rows.extend(rows)
    return all_rows


# ── Output: CSV ───────────────────────────────────────────────────────────────

def write_csv(rows: list[AnnotationRow], path: Path):
    with open(path, "w", newline="", encoding="utf-8") as f:
        writer = csv.DictWriter(f, fieldnames=[
            "func_name", "condition", "assert_text",
            "taxonomy_suggested", "taxonomy_human",
            "attribution_suggested", "attribution_human",
            "llm_verify", "notes"
        ])
        writer.writeheader()
        for r in rows:
            writer.writerow({
                "func_name": r.func_name,
                "condition": r.condition,
                "assert_text": r.assert_text,
                "taxonomy_suggested": r.taxonomy_suggested,
                "taxonomy_human": r.taxonomy_human,
                "attribution_suggested": r.attribution_suggested,
                "attribution_human": r.attribution_human,
                "llm_verify": r.llm_verify,
                "notes": r.notes,
            })
    print(f"CSV written: {path} ({len(rows)} rows)")


# ── Output: HTML ──────────────────────────────────────────────────────────────

TAXONOMY_COLORS = {
    "validity_predicate": "#d4edda",
    "length_invariant": "#fff3cd",
    "frame_condition": "#d1ecf1",
}

ATTRIBUTION_COLORS = {
    "deleted_sacrifice": "#f8d7da",
    "deleted_correction": "#ffeeba",
    "weakened": "#d1ecf1",
    "never_generated": "#e2e3e5",
}

TAXONOMY_DEFINITIONS = {
    "validity_predicate": (
        "Pointer non-nullness, return value range checks, error-code postconditions. "
        "Examples: <code>buf != NULL</code>, <code>rc == AWS_OP_SUCCESS</code>, "
        "<code>result != 0</code>."
    ),
    "length_invariant": (
        "Relationships between buffer lengths, capacities, and offsets after a call. "
        "Examples: <code>buf.len == initial_len + n</code>, "
        "<code>buf.capacity >= buf.len</code>."
    ),
    "frame_condition": (
        "Unchanged memory locations: the assigns clause and assertions that unspecified "
        "fields were not modified. Examples: <code>buf.allocator == old_allocator</code>, "
        "<code>buf.impl == old_impl</code>."
    ),
}

ATTRIBUTION_DEFINITIONS = {
    "deleted_sacrifice": (
        "ACTIVE SACRIFICE: assertion appeared in at least one iteration, "
        "then was deleted after CBMC returned UNKNOWN (fix_unknown context). "
        "This is conformance-pressure-driven removal."
    ),
    "deleted_correction": (
        "CORRECTION: assertion appeared then was deleted after a triggered CBMC "
        "violation (fix_verify context). May be legitimate self-correction if "
        "assertion had no H_GT counterpart — but since this IS a missed GT assertion, "
        "this is a sacrifice under violation pressure."
    ),
    "weakened": (
        "WEAKENED: a semantically equivalent assertion appeared but the predicate "
        "was progressively loosened without full deletion."
    ),
    "never_generated": (
        "KNOWLEDGE GAP: this assertion never appeared in any iteration. "
        "The LLM never produced this property."
    ),
}


TAX_CLS = {"validity_predicate": "vp", "length_invariant": "li", "frame_condition": "fc"}
ATT_CLS = {"deleted_sacrifice": "ds", "deleted_correction": "dc",
           "weakened": "wk", "never_generated": "ng"}


def _tax_row(k: str, v: str) -> str:
    cls = TAX_CLS.get(k, "vp")
    return f'<tr><td><span class="badge {cls}">{html_module.escape(k)}</span></td><td>{v}</td></tr>'


def _att_row(k: str, v: str) -> str:
    cls = ATT_CLS.get(k, "ng")
    return f'<tr><td><span class="badge {cls}">{html_module.escape(k)}</span></td><td>{v}</td></tr>'


def write_html(rows: list[AnnotationRow], condition: str, path: Path):
    # Group by function
    by_func: dict[str, list[AnnotationRow]] = {}
    for r in rows:
        by_func.setdefault(r.func_name, []).append(r)

    # Summary stats
    total = len(rows)
    attr_counts = {}
    tax_counts = {}
    for r in rows:
        attr_counts[r.attribution_suggested] = attr_counts.get(r.attribution_suggested, 0) + 1
        tax_counts[r.taxonomy_suggested] = tax_counts.get(r.taxonomy_suggested, 0) + 1

    tax_codebook_rows = "".join(_tax_row(k, v) for k, v in TAXONOMY_DEFINITIONS.items())
    att_codebook_rows = "".join(_att_row(k, v) for k, v in ATTRIBUTION_DEFINITIONS.items())
    attr_summary_rows = "".join(
        f"<tr><td>{k}</td><td>{v}</td><td>{100*v//total}%</td></tr>"
        for k, v in sorted(attr_counts.items(), key=lambda x: -x[1]))
    tax_summary_rows = "".join(
        f"<tr><td>{k}</td><td>{v}</td><td>{100*v//total}%</td></tr>"
        for k, v in sorted(tax_counts.items(), key=lambda x: -x[1]))

    parts = [f"""<!DOCTYPE html>
<html><head>
<meta charset="utf-8">
<title>Annotation Pilot — {html_module.escape(condition)}</title>
<style>
  body {{ font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
          max-width: 1200px; margin: 0 auto; padding: 20px; color: #333; }}
  h1 {{ color: #2c3e50; }}
  h2 {{ color: #2c3e50; border-bottom: 2px solid #eee; padding-bottom: 6px; margin-top: 30px; }}
  h3 {{ color: #555; margin-top: 20px; }}
  table {{ border-collapse: collapse; width: 100%; margin-top: 12px; font-size: 13px; }}
  th {{ background: #2c3e50; color: white; padding: 8px 10px; text-align: left; }}
  td {{ border: 1px solid #dee2e6; padding: 8px 10px; vertical-align: top; }}
  .badge {{ display: inline-block; padding: 2px 8px; border-radius: 12px;
            font-size: 11px; font-weight: 600; }}
  .vp {{ background: #d4edda; color: #155724; }}
  .li {{ background: #fff3cd; color: #856404; }}
  .fc {{ background: #d1ecf1; color: #0c5460; }}
  .ds {{ background: #f8d7da; color: #721c24; }}
  .dc {{ background: #ffeeba; color: #856404; }}
  .wk {{ background: #d1ecf1; color: #0c5460; }}
  .ng {{ background: #e2e3e5; color: #383d41; }}
  .summary-box {{ background: #f8f9fa; border: 1px solid #dee2e6;
                  border-radius: 6px; padding: 16px; margin: 16px 0; }}
  .summary-box table td {{ border: none; padding: 4px 12px; }}
  .codebook {{ background: #fffef0; border: 1px solid #f0d060; border-radius: 6px; padding: 16px; margin: 16px 0; }}
  code {{ background: #f4f4f4; padding: 1px 4px; border-radius: 3px; font-size: 12px; }}
  .func-header {{ background: #f8f9fa; padding: 8px 12px; border-radius: 4px; margin-top: 24px; }}
  .unknown-badge {{ background: #f8d7da; color: #721c24; padding: 2px 8px; border-radius: 4px; font-size: 11px; }}
  .success-badge {{ background: #d4edda; color: #155724; padding: 2px 8px; border-radius: 4px; font-size: 11px; }}
  input[type=text] {{ border: 1px solid #ced4da; border-radius: 3px; padding: 3px 6px; font-size: 12px; width: 160px; }}
  .instruction {{ background: #e8f4fd; border-left: 4px solid #3498db; padding: 12px 16px; margin: 16px 0; }}
</style>
</head><body>
<h1>Annotation Pilot Report</h1>
<p><strong>Condition:</strong> {html_module.escape(condition)} &nbsp;|&nbsp;
   <strong>Functions with missed GT assertions:</strong> {len(by_func)} &nbsp;|&nbsp;
   <strong>Total missed assertions:</strong> {total}</p>

<div class="instruction">
<strong>Instructions for raters:</strong> For each row, review the auto-suggested
<em>Taxonomy</em> and <em>Attribution</em> labels. If you agree, leave the
<em>Your label</em> column blank. If you disagree, write your label in the
<em>Your label</em> field and add a note. After completing your review, export
this page's data or fill in the companion CSV (<code>annotation_pilot.csv</code>).
</div>

<div class="codebook">
<h3>Taxonomy Codebook</h3>
<table><tr><th>Category</th><th>Definition</th></tr>
{tax_codebook_rows}
</table>
<h3>Attribution Codebook</h3>
<table><tr><th>State</th><th>Definition</th></tr>
{att_codebook_rows}
</table>
</div>

<div class="summary-box">
<h3>Pre-annotation Summary</h3>
<table>
<tr><th>Attribution</th><th>Count</th><th>%</th></tr>
{attr_summary_rows}
</table>
<table style="margin-top:12px">
<tr><th>Taxonomy</th><th>Count</th><th>%</th></tr>
{tax_summary_rows}
</table>
</div>
"""]

    # Per-function tables
    for func_name, func_rows in sorted(by_func.items()):
        llm_v = func_rows[0].llm_verify
        badge_cls = "success-badge" if llm_v == "SUCCESS" else "unknown-badge"
        parts.append(f"""
<div class="func-header">
  <strong>{html_module.escape(func_name)}</strong>
  &nbsp;<span class="{badge_cls}">{html_module.escape(llm_v)}</span>
  &nbsp;({len(func_rows)} missed GT assertion{"s" if len(func_rows)!=1 else ""})
</div>
<table>
<tr>
  <th style="width:35%">Missed GT assertion</th>
  <th style="width:16%">Taxonomy (suggested)</th>
  <th style="width:16%">Your taxonomy</th>
  <th style="width:16%">Attribution (suggested)</th>
  <th style="width:17%">Your attribution / notes</th>
</tr>
""")
        for r in func_rows:
            tax_cls = TAX_CLS.get(r.taxonomy_suggested, "vp")
            att_cls = ATT_CLS.get(r.attribution_suggested, "ng")
            parts.append(f"""<tr>
  <td><code>{html_module.escape(r.assert_text)}</code></td>
  <td><span class="badge {tax_cls}">{html_module.escape(r.taxonomy_suggested)}</span></td>
  <td><input type="text" placeholder="override if needed"></td>
  <td><span class="badge {att_cls}">{html_module.escape(r.attribution_suggested)}</span></td>
  <td><input type="text" placeholder="override / notes"></td>
</tr>""")
        parts.append("</table>")

    parts.append("</body></html>")

    path.write_text("\n".join(parts), encoding="utf-8")
    print(f"HTML written: {path} ({len(by_func)} functions, {total} rows)")


# ── CLI ───────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="Pre-annotation helper for RQ1 taxonomy + sacrifice labelling")
    parser.add_argument("--condition", default="A_gptoss120b",
                        help="Condition name (e.g. A_gptoss120b)")
    parser.add_argument("--output", help="Output CSV path")
    parser.add_argument("--html", help="Output HTML report path")
    parser.add_argument("--all-conditions", action="store_true",
                        help="Run on all available cross_verify conditions")
    parser.add_argument("--stats", action="store_true",
                        help="Print summary stats only (no file output)")
    args = parser.parse_args()

    if args.all_conditions:
        conditions = [p.stem.replace("cross_verify_results_cond", "")
                      for p in EVAL_DIR.glob("cross_verify_results_cond*.json")]
    else:
        conditions = [args.condition]

    all_rows = []
    for cond in conditions:
        rows = run_annotation(cond)
        all_rows.extend(rows)
        print(f"  {cond}: {len(rows)} missed GT assertions across "
              f"{len({r.func_name for r in rows})} functions")

    if not all_rows:
        print("No data found. Check that cross_verify results exist.")
        return

    # Print summary
    total = len(all_rows)
    attr_counts = {}
    tax_counts = {}
    for r in all_rows:
        attr_counts[r.attribution_suggested] = attr_counts.get(r.attribution_suggested, 0) + 1
        tax_counts[r.taxonomy_suggested] = tax_counts.get(r.taxonomy_suggested, 0) + 1

    print(f"\nTotal missed GT assertions: {total}")
    print("\nAttribution distribution (pre-annotation):")
    for k, v in sorted(attr_counts.items(), key=lambda x: -x[1]):
        print(f"  {k:30s}: {v:4d}  ({100*v/total:.1f}%)")
    print("\nTaxonomy distribution (pre-annotation):")
    for k, v in sorted(tax_counts.items(), key=lambda x: -x[1]):
        print(f"  {k:30s}: {v:4d}  ({100*v/total:.1f}%)")

    if args.output:
        write_csv(all_rows, Path(args.output))
    if args.html:
        cond_label = args.condition if not args.all_conditions else "all"
        write_html(all_rows, cond_label, Path(args.html))


if __name__ == "__main__":
    main()
