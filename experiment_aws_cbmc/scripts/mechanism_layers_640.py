#!/usr/bin/env python3
"""
mechanism_layers_640.py — how far into its job did the harness actually get?

Every layer here is decided by CBMC, not inferred from the harness text. That is
the point: the earlier decomposition labelled the live silences by reading
assertions and diffing iterations, which left never-written as a residual and
made it the weakest claim in the paper.

Four layers, in the order a harness fails them:

  1 never ran        the reachability probe never fires, so no assertion in the
                     harness can, whatever it says
  2 illegal setup    handed the expert's postconditions, the harness FAILS on the
                     UNMUTATED function: its setup admits states the specification
                     forbids, so a strong assertion would fire on correct code and
                     the model has no honest way to write one
  3 unreachable bug  the harness is sound on the original and still misses the
                     mutant with the expert's assertions in place, so its setup
                     never puts the fault on trial
  4 missing assert   the expert's assertions catch the mutant once inserted, so
                     the input space was fine and the assertion was the whole gap

Layers 2-4 come from the GT-guided strengthening runs (`b2_repair_gt_*.json`),
which hand the model the expert's assertions and forbid touching the assume
envelope.

Usage:  python3 scripts/mechanism_layers_640.py
"""
import collections, glob, json, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "figures"))
import style as S  # noqa: E402

EXP = HERE.parent
PAPER8 = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
          "G_gptoss120b", "A_claude", "H_claude", "M_claude"]
LAYERS = ["never ran", "illegal setup", "unreachable bug", "missing assert",
          "envelope changed", "untested"]
LOOSENED = set()

def loosened():
    """Groups whose strengthening rewrite dropped assumes on net. A catch there
    may be the looser envelope reaching the fault rather than the assertion
    finding it, so the silence is not evidence for the missing-assertion layer."""
    p = EXP / "evaluation/envelope_check_640.json"
    if not p.exists():
        return set()
    return {(r["cond"], r["func"]) for r in json.load(open(p))["rows"] if r["net_loosened"]}

def gt_runs():
    out = {}
    for p in glob.glob(str(EXP / "evaluation/b2_repair_gt_*.json")):
        cond = Path(p).stem.replace("b2_repair_gt_", "")
        for r in json.load(open(p)):
            out[(cond, r["func"])] = r
    return out

def classify(cond, func, mutant, dead, runs):
    """Layers 3 and 4 are decided per mutant, not per function. A strengthened
    harness that catches one of a function's twenty silences tells us nothing
    about the other nineteen, and reading `n_caught` as a boolean would label
    all twenty a missing assertion."""
    if (cond, func) in dead:
        return "never ran"
    r = runs.get((cond, func))
    if r is None or "per_mutant" not in r:
        return "untested"
    if r.get("orig_after") not in ("SUCCESS", "UNKNOWN"):
        return "illegal setup"
    hit = r["per_mutant"].get(mutant) == "FAIL"
    if hit and (cond, func) in LOOSENED:
        return "envelope changed"
    return "missing assert" if hit else "unreachable bug"

def main():
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    full = {(r["cond"], r["func"]): r for r in
            json.load(open(EXP / "evaluation/reachability_full_640.json"))["rows"]}
    head_dead = S.dead_groups()
    # The full probe supersedes the older head-only one where it has a verdict.
    dead = {k for k, r in full.items() if r.get("head") != "FAIL"}
    dead |= {k for k in head_dead if k not in full}
    runs = gt_runs()
    global LOOSENED
    LOOSENED = loosened()

    rows = []
    for cond in PAPER8:
        v = llm.get(cond, {})
        for (f, m) in canon:
            if v.get((f, m)) == "SUCCESS":
                rows.append((cond, f, m, classify(cond, f, m, dead, runs)))

    def table(title, sel):
        if not sel:
            return
        mut = collections.Counter(l for _, _, _, l in sel)
        grp = collections.Counter()
        for (c, f), l in {(c, f): l for c, f, _, l in sel}.items():
            grp[l] += 1
        n, g = len(sel), len({(c, f) for c, f, _, _ in sel})
        print(f"\n{title}: {n} silences in {g} groups")
        for k in LAYERS:
            if mut[k] or grp[k]:
                print(f"   {k:16s} mutants {mut[k]:4d} ({100*mut[k]/n:5.1f}%)"
                      f"   groups {grp[k]:3d} ({100*grp[k]/g:5.1f}%)")

    table("ALL eight conditions", rows)
    table("gpt-oss", [r for r in rows if "gptoss" in r[0]])
    table("Claude", [r for r in rows if "claude" in r[0]])
    table("organic gpt-oss (Baseline, Neutral, Single)",
          [r for r in rows if r[0] in ("A_gptoss120b", "H_gptoss120b", "G_gptoss120b")])

    live = [r for r in rows if r[3] != "never ran"]
    tested = [r for r in live if r[3] != "untested"]
    print(f"\nlive silences: {len(live)}, of which {len(tested)} tested "
          f"({100*len(tested)/len(live):.0f}%)")

if __name__ == "__main__":
    main()
