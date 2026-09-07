#!/usr/bin/env python3
"""
make_adjudication_worksheet.py — blind rating sheet for the mechanism labels.

Writes `adjudication/worksheet.md`, one card per silenced (condition, function)
group, carrying only the raw evidence a rater needs and none of the automated
label: the GT assertions with, for each, whether the LLM harness ever wrote it
exactly and what CBMC returned at that iteration; the LLM's final assertions;
and the two assume envelopes. Groups appear in a shuffled, seeded order.

The answer key (the rule's own labels) goes to `adjudication/key.json`, which a
rater must not open. `score_adjudication.py` scores a filled worksheet against
it and reports agreement and Cohen's kappa.

Usage:  python3 make_adjudication_worksheet.py [--sample N]
"""
import argparse, importlib.util as _u, json, random
from pathlib import Path

BASE = Path(__file__).resolve().parent.parent
OUT = BASE / "adjudication"

_spec = _u.spec_from_file_location("av2", str(BASE / "scripts" / "attribution_v2.py"))
av2 = _u.module_from_spec(_spec)
_spec.loader.exec_module(av2)

LABEL = {"KNOWLEDGE-GAP": "NW", "SACRIFICE": "Del", "AOC": "Nar", "UNRESOLVED": "Unres"}
CONDITIONS = ["A_gptoss120b", "H_gptoss120b", "M_gptoss120b", "G_gptoss120b",
              "Oracle_gptoss120b", "A_claude", "H_claude", "M_claude"]

def evidence(cond, func):
    ds = f"feedback_loop_{cond}"
    gts = av2.A.get_gt_asserts(func)
    its = av2.A.get_llm_iter_asserts(ds, func)
    vmap = av2.verdicts(ds, func)
    pres = {it: {av2.norm(a) for a in asl} for it, asl in its.items()}
    maxit = max(pres) if pres else None
    final = pres.get(maxit, set())
    rows = []
    for g in gts:
        gn = av2.norm(g)
        seen = [it for it in sorted(pres) if gn in pres[it]]
        if gn in final:
            rows.append((g.strip(), "present in the final harness"))
        elif not seen:
            rows.append((g.strip(), "never written exactly, in any iteration"))
        else:
            last = seen[-1]
            rows.append((g.strip(), f"written exactly in iteration {last}, absent from the "
                                    f"final; CBMC at iteration {last} returned "
                                    f"{vmap.get(last, 'unrecorded')}"))
    try:
        is_aoc, aoc_ev = av2.A.detect_aoc(ds, func, its)
    except Exception as e:
        is_aoc, aoc_ev = None, [f"(assume comparison unavailable: {e})"]
    return {
        "gt_rows": rows,
        "final_asserts": sorted(a.strip() for a in its.get(maxit, [])) if maxit is not None else [],
        "final_iter": maxit,
        "assume_evidence": [str(x) for x in (aoc_ev or [])][:4],
    }

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--sample", type=int, default=0,
                    help="rate only N groups (stratified: every non-NW group first)")
    ap.add_argument("--seed", type=int, default=20260907)
    args = ap.parse_args()

    groups = []
    for cond in CONDITIONS:
        run = av2.run(cond)
        if run is None:
            continue
        _s, per_func = run
        for func, (attribution, n) in per_func.items():
            groups.append({"cond": cond, "func": func, "n": n, "label": LABEL[attribution]})

    if args.sample:
        rare = [g for g in groups if g["label"] != "NW"]
        rest = [g for g in groups if g["label"] == "NW"]
        random.Random(args.seed).shuffle(rest)
        groups = (rare + rest)[:max(args.sample, len(rare))]

    random.Random(args.seed).shuffle(groups)
    OUT.mkdir(exist_ok=True)

    lines = [
        "# Mechanism adjudication worksheet (blind)",
        "",
        "One card per silenced (condition, function) group. For each card, read the evidence",
        "and write one label on the `rating:` line, then run `score_adjudication.py`.",
        "",
        "- **NW** never-written: no GT assertion that would catch the fault is in the final",
        "  harness, and the missing one was never written exactly in any iteration (or was",
        "  removed after a CBMC FAIL, i.e. self-correction).",
        "- **Del** deleted-to-pass: a GT assertion was written exactly, CBMC returned UNKNOWN",
        "  at that iteration, and it is gone from the final harness.",
        "- **Nar** narrowed-away: every GT assertion is present in the final harness, but the",
        "  harness's assumptions are tighter than the expert's, so the faulty states are unreachable.",
        "- **Unres** unresolved: the evidence does not support any of the three (for example no",
        "  GT assertion could be parsed, so nothing can be said about presence).",
        "",
        "Do not open `key.json` before finishing.",
        "",
    ]
    for i, g in enumerate(groups, 1):
        ev = evidence(g["cond"], g["func"])
        lines += [f"## {i}. `{g['cond']}` / `{g['func']}` ({g['n']} silenced mutants)", ""]
        lines.append("**Expert (GT) assertions and what the LLM harness did with each:**")
        lines.append("")
        if ev["gt_rows"]:
            for a, status in ev["gt_rows"]:
                lines.append(f"- `{a}`  \n  → {status}")
        else:
            lines.append("- (no GT assertion could be parsed from the expert harness)")
        lines += ["", f"**LLM harness, final iteration ({ev['final_iter']}), assertions written:**", ""]
        if ev["final_asserts"]:
            lines += [f"- `{a}`" for a in ev["final_asserts"]]
        else:
            lines.append("- (none)")
        if ev["assume_evidence"]:
            lines += ["", "**Assume-envelope comparison (LLM vs expert):**", ""]
            lines += [f"- {x}" for x in ev["assume_evidence"]]
        lines += ["", "rating: ", "", "---", ""]

    (OUT / "worksheet.md").write_text("\n".join(lines))
    json.dump([{k: g[k] for k in ("cond", "func", "n", "label")} for g in groups],
              open(OUT / "key.json", "w"), indent=1)
    print(f"wrote {OUT/'worksheet.md'} ({len(groups)} groups) and {OUT/'key.json'}")

if __name__ == "__main__":
    main()
