#!/usr/bin/env python3
"""
vacuity_cause.py — why does a silencing harness never reach its assertions?

`reachability_probe.py` says whether the postcondition region is reachable.
This says why it is not, by re-running each dead harness on the unmutated
source with unwinding assertions turned back on:

  unwind-truncation   CBMC reports an unwinding assertion failure, so a loop in
                      the harness needs more iterations than the proof's bound
                      allows and the paths past it were cut
  contradictory-assumes  bounds are adequate; the assumptions themselves admit
                      no execution
  (verdict shown otherwise)

Usage:  CBMC640=... python3 scripts/vacuity_cause.py
"""
import json, os, re, shutil, subprocess, sys, tempfile
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXP = HERE.parent
sys.path.insert(0, str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS  # noqa: E402

CBMC = os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"

def cbmc(func, path, unwinding_assertions):
    cfg = FUNC_CONFIGS.get(func)
    if not cfg:
        return "NOCFG", ""
    flags = ["--no-standard-checks"] + (["--unwinding-assertions"] if unwinding_assertions
                                        else ["--no-unwinding-assertions"])
    cmd = ([CBMC] + list(COMMON_FLAGS) + list(cfg.get("defines", [])) + list(cfg["unwind"])
           + list(cfg.get("unwindset") or []) + flags
           + ["--function", f"{func}_harness"] + [str(p) for p in cfg["proof_sources"]]
           + [str(path)] + [str(p) for p in cfg["project_sources"]])
    try:
        o = subprocess.run(cmd, capture_output=True, text=True, timeout=600).stdout
    except subprocess.TimeoutExpired:
        return "TIMEOUT", ""
    v = ("SUCCESS" if "VERIFICATION SUCCESSFUL" in o else
         "FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
    unwind_fail = [ln.strip() for ln in o.splitlines()
                   if "unwinding assertion" in ln and "FAILURE" in ln]
    return v, "; ".join(unwind_fail[:2])

def main():
    probe = json.load(open(EXP / "evaluation/reachability_probe_640.json"))["rows"]
    dead = [r for r in probe if r["probe"] == "SUCCESS"]
    rows = []
    for r in dead:
        h = EXP / "results" / f"feedback_loop_{r['cond']}" / r["func"] / r["harness"]
        verdict, unwind = cbmc(r["func"], h, unwinding_assertions=True)
        cause = ("unwind-truncation" if unwind or verdict == "FAIL"
                 else "contradictory-assumes" if verdict == "SUCCESS" else verdict)
        rows.append({**r, "with_unwinding_assertions": verdict,
                     "cause": cause, "evidence": unwind})
        print(f"{cause:<24} {r['cond']}/{r['func']} ({r['n']} silenced)  {unwind[:90]}", flush=True)

    out = EXP / "evaluation/vacuity_cause_640.json"
    summary = {}
    for r in rows:
        summary.setdefault(r["cause"], {"groups": 0, "silenced": 0})
        summary[r["cause"]]["groups"] += 1
        summary[r["cause"]]["silenced"] += r["n"]
    json.dump({"summary": summary, "rows": rows}, open(out, "w"), indent=1)
    print("\n" + json.dumps(summary, indent=1))

if __name__ == "__main__":
    main()
