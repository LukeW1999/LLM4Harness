#!/usr/bin/env python3
"""
unwinding_sensitivity.py — what survives when unwinding assertions are on?

The study runs CBMC with `--no-unwinding-assertions`, which is also what CBMC
defaults to up to 6.4.0 and what aws-c-common's proof Makefile inherits (it
leaves CBMC_FLAG_UNWINDING_ASSERTIONS empty). CBMC 6.8.0 turns them on by
default, and under that setting a harness whose loop outruns its bound is
reported unverified instead of passing.

For every function that carries a silence, this runs both harnesses on the
UNMUTATED source under both settings and records whether each still passes its
own fidelity gate:

  gate_off / gate_on   SUCCESS means the harness verifies on the original

An LLM harness that passes with unwinding assertions off and fails with them on
was only ever accepted because the check was off; an expert harness that fails
with them on would invalidate the reference for that function, so both sides
are measured.

Usage:  CBMC640=... python3 scripts/unwinding_sensitivity.py [--out FILE]
"""
import argparse, collections, json, os, re, shutil, subprocess, sys, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXP = HERE.parent
sys.path.insert(0, str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS  # noqa: E402
import run_mutation_oracle_cbmc as rmo  # noqa: E402

CBMC = os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"

def run(func, harness, unwinding_on):
    cfg = FUNC_CONFIGS.get(func)
    if not cfg:
        return "NOCFG"
    flags = ["--no-standard-checks"] + (["--unwinding-assertions"] if unwinding_on
                                        else ["--no-unwinding-assertions"])
    cmd = ([CBMC] + list(COMMON_FLAGS) + list(cfg.get("defines", [])) + list(cfg["unwind"])
           + list(cfg.get("unwindset") or []) + flags
           + ["--function", f"{func}_harness"] + [str(p) for p in cfg["proof_sources"]]
           + [str(harness)] + [str(p) for p in cfg["project_sources"]])
    try:
        o = subprocess.run(cmd, capture_output=True, text=True, timeout=600).stdout
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    return ("SUCCESS" if "VERIFICATION SUCCESSFUL" in o else
            "FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")

def final_harness(cond, func):
    d = EXP / "results" / f"feedback_loop_{cond}" / func
    its = sorted(d.glob("iter_*_harness.c"),
                 key=lambda p: int(re.search(r"iter_(\d+)_", p.name).group(1)))
    return its[-1] if its else None

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--out", default=str(EXP / "evaluation/unwinding_sensitivity_640.json"))
    args = ap.parse_args()

    probe = json.load(open(EXP / "evaluation/reachability_probe_640.json"))["rows"]
    gt_dir = Path(str(rmo.GT_PROOFS_DIR))
    rows, seen_gt, t0 = [], {}, time.time()

    for r in probe:
        cond, func = r["cond"], r["func"]
        h = final_harness(cond, func)
        if h is None:
            continue
        off, on = run(func, h, False), run(func, h, True)
        rows.append({"side": "llm", "cond": cond, "func": func, "n": r["n"],
                     "dead": r["probe"] == "SUCCESS", "gate_off": off, "gate_on": on})
        print(f"llm  {off:>7} -> {on:<7} {cond}/{func} ({r['n']} silenced)", flush=True)

        if func not in seen_gt:
            g = gt_dir / func / f"{func}_harness.c"
            if g.exists():
                seen_gt[func] = (run(func, g, False), run(func, g, True))
                print(f"  GT {seen_gt[func][0]:>7} -> {seen_gt[func][1]:<7} {func}", flush=True)

    for func, (off, on) in seen_gt.items():
        rows.append({"side": "gt", "func": func, "gate_off": off, "gate_on": on})

    llm = [r for r in rows if r["side"] == "llm"]
    lost = [r for r in llm if r["gate_off"] == "SUCCESS" and r["gate_on"] != "SUCCESS"]
    gt = [r for r in rows if r["side"] == "gt"]
    summary = {
        "cbmc": "6.4.0",
        "llm_groups": len(llm),
        "llm_gate_lost_with_unwinding_on": len(lost),
        "silences_behind_them": sum(r["n"] for r in lost),
        "silences_behind_dead_groups_lost": sum(r["n"] for r in lost if r["dead"]),
        "gt_functions": len(gt),
        "gt_gate_lost_with_unwinding_on": sum(1 for r in gt if r["gate_off"] == "SUCCESS"
                                              and r["gate_on"] != "SUCCESS"),
        "elapsed_s": round(time.time() - t0),
    }
    json.dump({"summary": summary, "rows": rows}, open(args.out, "w"), indent=1)
    print("\n" + json.dumps(summary, indent=1))

if __name__ == "__main__":
    main()
