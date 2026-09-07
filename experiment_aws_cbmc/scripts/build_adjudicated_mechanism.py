#!/usr/bin/env python3
"""
build_adjudicated_mechanism.py — materialise the mechanism labels behind Table 2.

The labels come from `attribution_v2.py`'s deterministic rule (exact assertion
match, UNKNOWN-triggered removal for active deletion, assume-envelope comparison
for over-constraint, with AOC suppressed when no GT assertion was parsed). This
script runs that classifier over every condition in the table and writes
`evaluation/adjudicated_mechanism.json`, the artifact paper_numbers_640.py
audits Table 2 against.

The blind re-adjudication recorded in ADJUDICATION_attribution_v2.md validates
this rule; it does not supply the labels. The stale
`attribution_feedback_loop_*.json` dumps predate the AOC empty-set fix and are
kept only as history.

Labels: NW never-written, Del deleted-to-pass, Nar narrowed-away, Unres.
"""
import importlib.util as _u
import json
from pathlib import Path

BASE = Path(__file__).resolve().parent.parent
EVAL = BASE / "evaluation"

_spec = _u.spec_from_file_location("av2", str(BASE / "scripts" / "attribution_v2.py"))
av2 = _u.module_from_spec(_spec)
_spec.loader.exec_module(av2)

LABEL = {"KNOWLEDGE-GAP": "NW", "SACRIFICE": "Del", "AOC": "Nar", "UNRESOLVED": "Unres"}

CONDITIONS = ["A_gptoss120b", "H_gptoss120b", "M_gptoss120b", "G_gptoss120b",
              "Oracle_gptoss120b", "A_claude", "H_claude", "M_claude"]

def labels_for(cond):
    out = {}
    run = av2.run(cond)
    if run is None:
        return out
    _summary, per_func = run
    for func, (attribution, _n) in per_func.items():
        out[func] = LABEL[attribution]
    return out

def main():
    labels = {c: labels_for(c) for c in CONDITIONS}
    doc = {
        "source": "attribution_v2.classify_func, run per (condition, function) silenced group",
        "labels_key": {"NW": "never-written", "Del": "deleted-to-pass",
                       "Nar": "narrowed-away", "Unres": "unresolved (scaffold-level)"},
        "validation": "ADJUDICATION_attribution_v2.md records a blind re-adjudication of "
                      "all 28 groups against this rule (28/28 after the AOC empty-set fix). "
                      "The re-adjudication validates the rule; the labels here are the rule's own output.",
        "note": "Silenced counts for the Claude conditions are identical under CBMC 5.95.1 "
                "and 6.4.0 (16/16/11), so the group labels carry over unchanged.",
        "labels": labels,
    }
    p = EVAL / "adjudicated_mechanism.json"
    json.dump(doc, open(p, "w"), indent=1)
    print(f"wrote {p}")
    for c in CONDITIONS:
        print(f"  {c:<20} {len(labels[c])} groups")

if __name__ == "__main__":
    main()
