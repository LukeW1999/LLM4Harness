#!/usr/bin/env python3
"""
fidelity_gate_640.py — enforce the condition the certificate is defined on.

The oracle certifies a silenced bug when the expert harness fails on a mutant
and the LLM harness verifies on it *while itself verifying on the unmutated
function*. That last clause is what makes the LLM harness a harness rather than
a broken file, and nothing in the pipeline was checking it: `silenced_640.py`
records a verdict for every group whose final harness exists.

It matters asymmetrically. A harness that fails on correct code rarely returns
SUCCESS on a mutant, so almost no certified silence is affected; but it fails
on mutants freely, so it contributes cheap "catches" that inflate the
denominator every silence rate is measured against.

  python3 scripts/fidelity_gate_640.py
"""
import json, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "figures"))
import style as S  # noqa: E402

EXP = HERE.parent
PAPER8 = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
          "G_gptoss120b", "A_claude", "H_claude", "M_claude"]

def gate():
    """(cond, func) -> did the final harness verify on the unmutated function.
    Absent from the map means we have not measured it, which is not the same as
    failing it, and callers must not treat the two alike."""
    out = {}
    for f in ("passrate_640.json", "passrate_repeats_640.json"):
        p = EXP / "evaluation" / f
        if p.exists():
            for r in json.load(open(p))["verdicts"]:
                out[(r["cond"], r["func"])] = r["v"]
    return out

def main():
    g = gate()
    canon, llm = S.gt_fail_set(), S.llm_verdicts()
    print(f"{'condition':22s} {'catch':>7s} {'gated':>7s} {'sil':>5s} {'gated':>6s} "
          f"{'adj sil %':>10s} {'gated':>7s}")
    rows = {}
    for cond in PAPER8:
        v = llm.get(cond, {})
        c = gc = s_ = gs = ungated = 0
        for (f, m) in canon:
            x, ok = v.get((f, m)), g.get((cond, f))
            if ok is None:
                ungated += 1
            if x in ("FAIL", "SAT"):
                c += 1; gc += ok == "SUCCESS"
            elif x == "SUCCESS":
                s_ += 1; gs += ok == "SUCCESS"
        adj = 100 * s_ / (s_ + c) if s_ + c else 0
        gadj = 100 * gs / (gs + gc) if gs + gc else 0
        rows[cond] = {"catch": c, "catch_gated": gc, "silenced": s_,
                      "silenced_gated": gs, "adj_pct": round(adj, 1),
                      "adj_pct_gated": round(gadj, 1), "unmeasured": ungated}
        print(f"{S.COND_LABEL.get(cond, cond) + '/' + S.model_of(cond):22s} "
              f"{c:7d} {gc:7d} {s_:5d} {gs:6d} {adj:9.1f}% {gadj:6.1f}%")
    tot_s = sum(r["silenced"] for r in rows.values())
    tot_g = sum(r["silenced_gated"] for r in rows.values())
    print(f"\nsilences {tot_s} -> {tot_g} under the gate "
          f"({tot_s - tot_g} lost)")
    json.dump({"summary": {"silenced": tot_s, "silenced_gated": tot_g},
               "per_condition": rows}, open(EXP / "evaluation/fidelity_gate_640.json", "w"),
              indent=1)

if __name__ == "__main__":
    main()
