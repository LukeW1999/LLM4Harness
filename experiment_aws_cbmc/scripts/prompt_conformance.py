#!/usr/bin/env python3
"""
prompt_conformance.py — is what the prompt promises actually buildable?

The generation prompt documents an include set, a proof-helper API and a set of
status macros. Nothing checked that those instructions were consistent with the
build we run the harnesses under, and twice they were not: the s2n prompt
documented aws-c-common's `proof_helpers` API, and later the build linked stubs
the prompt still let the model redeclare. Both were found by generating 25
harnesses and reading the failures, an hour per hypothesis.

This is the fast version. `conformance/<corpus>_prompt_contract_harness.c` uses
every include, helper and macro the prompt names. The test is whether CBMC can
CONVERT it, not whether it verifies: a failing assertion means the contract is
buildable and one of our assertions is simply wrong about the code, while no
verdict at all means the prompt is telling the model to write something that
cannot compile.

Usage:  CBMC640=... python3 scripts/prompt_conformance.py
"""
import os, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import cbmc_runner as C  # noqa: E402

EXP = HERE.parent
CBMC = os.environ.get("CBMC640") or os.path.expanduser(
    "~/tools/cbmc-6.4.0/extracted/usr/bin/cbmc")

CASES = [("s2n", "s2n_stuffer_is_consumed",
          EXP / "conformance/s2n_prompt_contract_harness.c")]

def run(func, harness):
    cfg = C.FUNC_CONFIGS[func]
    s2n_root = str(C.S2N_SRCDIR).rsplit("/", 1)[0]
    remap = lambda x: str(x).replace("/root/s2n-tls", s2n_root)
    cmd = ([CBMC] + [remap(f) for f in cfg["flags"]] + list(cfg["unwind"])
           + list(cfg.get("unwindset") or [])
           + ["--no-standard-checks", "--no-unwinding-assertions",
              "--function", cfg["harness_entry"], str(harness)]
           + [remap(s) for s in cfg["sources"]])
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=400)
        return r.stdout + r.stderr
    except subprocess.TimeoutExpired:
        return "TIMEOUT"

def main():
    bad = 0
    for corpus, func, harness in CASES:
        if not harness.exists():
            print(f"{corpus}: no contract harness at {harness}"); bad += 1; continue
        out = run(func, harness)
        converted = "VERIFICATION SUCCESSFUL" in out or "VERIFICATION FAILED" in out
        print(f"{corpus}: prompt contract {'BUILDS' if converted else 'DOES NOT BUILD'}")
        if not converted:
            bad += 1
            for line in out.splitlines():
                if "error" in line.lower() or "not found" in line.lower():
                    print("   ", line[:150])
    sys.exit(1 if bad else 0)

if __name__ == "__main__":
    main()
