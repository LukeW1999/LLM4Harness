"""Do the expert harnesses survive the unwinding check the LLM harnesses fail?

Runs each aws-c-common expert harness on its unmutated function twice, once
with --unwinding-assertions and once without, so the asymmetry Threats claims
rests on a measurement rather than an estimate.
"""
import json, os, shutil, subprocess, sys, time
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
HERE = Path(__file__).resolve().parent; EXP = HERE.parent
sys.path.insert(0, str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
from run_mutation_oracle_cbmc import GT_PROOFS_DIR
CBMC = os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"

def verify(task):
    func, on = task
    cfg = FUNC_CONFIGS.get(func)
    harness = GT_PROOFS_DIR / func / f"{func}_harness.c"
    if not cfg or "project_sources" not in cfg or not harness.exists():
        return func, on, None
    match = ["--no-standard-checks"] + (["--unwinding-assertions"] if on
                                        else ["--no-unwinding-assertions"])
    cmd = ([CBMC] + list(COMMON_FLAGS) + list(cfg.get("defines", []))
           + list(cfg["unwind"]) + list(cfg.get("unwindset") or []) + match
           + ["--function", f"{func}_harness"]
           + [str(p) for p in cfg["proof_sources"]] + [str(harness)]
           + [str(p) for p in cfg["project_sources"]])
    try:
        o = subprocess.run(cmd, capture_output=True, text=True, timeout=240).stdout
    except subprocess.TimeoutExpired:
        return func, on, "TIMEOUT"
    return func, on, ("SUCCESS" if "VERIFICATION SUCCESSFUL" in o
                      else "FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")

if __name__ == "__main__":
    t0 = time.time()
    tasks = [(f, on) for f in FUNC_CONFIGS for on in (True, False)]
    workers = max(1, (os.cpu_count() or 8) - 1)
    out = {}
    with ProcessPoolExecutor(max_workers=workers) as ex:
        for fut in as_completed([ex.submit(verify, t) for t in tasks]):
            f, on, v = fut.result()
            if v is not None:
                out.setdefault(f, {})["on" if on else "off"] = v
    ok_off = [f for f, v in out.items() if v.get("off") == "SUCCESS"]
    lost = [f for f in ok_off if out[f].get("on") != "SUCCESS"]
    json.dump({"cbmc": "6.4.0", "functions": len(out), "verify_off": len(ok_off),
               "stop_when_on": len(lost), "lost": sorted(lost),
               "elapsed_s": round(time.time() - t0), "per_function": out},
              open(EXP / "evaluation/expert_passrate_640.json", "w"), indent=1)
    print(f"expert harnesses: {len(out)} run, {len(ok_off)} verify with the check off, "
          f"{len(lost)} stop verifying with it on -> {sorted(lost)}")
