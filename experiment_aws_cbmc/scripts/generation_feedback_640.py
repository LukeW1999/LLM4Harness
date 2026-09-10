#!/usr/bin/env python3
"""
generation_feedback_640.py — was the model ever told its harness does not run?

A natural objection to the never-ran layer is that we scored those harnesses
under a permissive setting, and that a model given the strict feedback would
have fixed the loop. The generation loop, though, calls CBMC through
`cbmc_runner.run_cbmc`, which passes no `--no-` flags at all and therefore runs
at 6.4.0's defaults, unwinding assertions included. So the model did receive
that feedback, on every iteration, for up to fifteen iterations.

This re-runs each never-ran harness exactly as the generation loop did and
records whether CBMC reported an unwinding assertion failure. A harness that
fails here is one the model was told about and did not repair.

  CBMC640=... python3 scripts/generation_feedback_640.py
"""
import json, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE / "figures"))
import cbmc_runner as C  # noqa: E402
import style as S  # noqa: E402
import reachability_probe as P  # noqa: E402

EXP = HERE.parent

def main():
    fullp = {(r["cond"], r["func"]): r
             for r in json.load(open(EXP / "evaluation/reachability_full_640.json"))["rows"]}
    dead = sorted({k for k, r in fullp.items() if r.get("head") != "FAIL"}
                  | {k for k in S.dead_groups() if k not in fullp})
    rows, told = [], 0
    for cond, func in dead:
        h = P.final_harness(cond, func)
        if h is None:
            continue
        try:
            r = C.run_cbmc(func, h, timeout=200)
        except Exception as e:
            rows.append({"cond": cond, "func": func, "error": str(e)[:80]}); continue
        out = getattr(r, "stdout", "") or ""
        unwind = "unwinding assertion" in out
        told += unwind
        rows.append({"cond": cond, "func": func,
                     "generation_verdict": getattr(r, "verification_result", None)
                                            or getattr(r, "result", None),
                     "unwinding_assertion_reported": unwind})
        print(f"  {'told' if unwind else '    '}  {cond}/{func}")
    print(f"\n{told} of {len(rows)} never-ran harnesses were reported to the model "
          f"as failing an unwinding assertion during generation")
    json.dump({"summary": {"groups": len(rows), "told_during_generation": told},
               "rows": rows}, open(EXP / "evaluation/generation_feedback_640.json", "w"), indent=1)

if __name__ == "__main__":
    main()
