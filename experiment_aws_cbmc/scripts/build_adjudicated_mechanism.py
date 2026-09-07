#!/usr/bin/env python3
"""
build_adjudicated_mechanism.py — materialise the mechanism labels behind Table 2.

The automated attribution_v2 output (`attribution_feedback_loop_*.json`) is the
starting point; the blind manual adjudication (ADJUDICATION_attribution_v2.md,
28/28 groups concordant after the AOC empty-set fix) corrects it in three
documented ways. This script applies those corrections and writes
`evaluation/adjudicated_mechanism.json`, the artifact paper_numbers_640.py
audits Table 2 against, so the correction is reproducible rather than manual.

Labels: NW never-written, Del deleted-to-pass, Nar narrowed-away, Unres.
"""
import json
from pathlib import Path

BASE = Path(__file__).resolve().parent.parent
EVAL = BASE / "evaluation"

RAW = {"KNOWLEDGE-GAP": "NW", "SACRIFICE-CANDIDATE": "Del",
       "ASSUMPTION-OVER-CONSTRAINT": "Nar", "UNKNOWN (structural)": "Unres"}

GPTOSS = ["A_gptoss120b", "H_gptoss120b", "M_gptoss120b", "G_gptoss120b", "Oracle_gptoss120b"]

# Adjudicated Claude labels (ADJUDICATION_attribution_v2.md, groups 17-28). The
# automated run mislabels these: its Jaccard-era SAC calls were overturned by
# exact-match plus a CBMC semantic re-run, leaving one genuine deletion group.
CLAUDE = {
    "A_claude": {"aws_byte_buf_cat": "NW", "aws_byte_buf_from_array": "NW",
                 "aws_byte_buf_from_empty_array": "NW", "aws_byte_buf_reset": "NW",
                 "aws_byte_buf_secure_zero": "NW",
                 "aws_ring_buffer_buf_belongs_to_pool": "Del"},
    "H_claude": {"aws_byte_buf_cat": "NW", "aws_byte_buf_eq_c_str": "NW",
                 "aws_byte_buf_reset": "NW", "aws_byte_buf_from_empty_array": "Nar",
                 "aws_string_destroy": "Unres"},
    "M_claude": {"aws_byte_buf_cat": "NW", "aws_byte_buf_reset": "NW",
                 "aws_byte_buf_from_empty_array": "NW",
                 "aws_string_eq_byte_cursor": "NW", "aws_string_eq_byte_buf": "NW"},
}

REASONS = []

def labels_for(cond):
    d = json.load(open(EVAL / f"attribution_feedback_loop_{cond}.json"))
    out = {}
    for func, v in d["per_function"].items():
        lab = RAW[v["attribution"]]
        # (1) AOC empty-set fix: the detector's "all GT assertions still present"
        # test is vacuously true when no GT assertion was parsed, so those groups
        # are unresolved, not over-constraint.
        if lab == "Nar" and v.get("gt_asserts", 0) == 0:
            lab = "Unres"
            REASONS.append(f"{cond}/{func}: Nar -> Unres (no GT assertion parsed)")
        # (2) The Oracle control runs under the expert's own assume envelope, so
        # the envelope-comparison detector cannot validly fire for it.
        elif lab == "Nar" and cond == "Oracle_gptoss120b":
            lab = "NW"
            REASONS.append(f"{cond}/{func}: Nar -> NW (runs under GT's own assume envelope)")
        # (3) Adjudication #7: the catching assertion was never written here, and
        # the documented KG > AOC priority routes the group to never-written.
        elif lab == "Nar" and (cond, func) == ("M_gptoss120b", "aws_byte_buf_eq_c_str"):
            lab = "NW"
            REASONS.append(f"{cond}/{func}: Nar -> NW (catching assertion never written; KG>AOC)")
        out[func] = lab
    return out

def main():
    labels = {c: labels_for(c) for c in GPTOSS}
    labels.update(CLAUDE)
    doc = {
        "source": "attribution_feedback_loop_*.json corrected by ADJUDICATION_attribution_v2.md",
        "labels_key": {"NW": "never-written", "Del": "deleted-to-pass",
                       "Nar": "narrowed-away", "Unres": "unresolved (scaffold-level)"},
        "corrections": REASONS,
        "claude_note": "Claude groups carry the blind manual adjudication verbatim; "
                       "silenced counts are identical under CBMC 5.95.1 and 6.4.0 "
                       "(16/16/11), so the group labels carry over unchanged.",
        "labels": labels,
    }
    p = EVAL / "adjudicated_mechanism.json"
    json.dump(doc, open(p, "w"), indent=1)
    print(f"wrote {p} ({len(labels)} conditions, {len(REASONS)} corrections)")
    for r in REASONS:
        print("  -", r)

if __name__ == "__main__":
    main()
