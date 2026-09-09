#!/usr/bin/env bash
# Every s2n expert harness must verify against its unmutated function under our
# build, or the corpus is not usable as a reference. This is the gate the s2n
# build fixes were chasing.
set -uo pipefail
cd "$(dirname "$(dirname "$0")")" || exit 1
CBMC640="${CBMC640:-$HOME/tools/cbmc-6.4.0/extracted/usr/bin/cbmc}" \
python3 - <<'PY'
import sys, os, subprocess
from concurrent.futures import ProcessPoolExecutor, as_completed
sys.path.insert(0, "scripts"); import cbmc_runner as C
S2N = str(C.S2N_SRCDIR).rsplit("/", 1)[0]
remap = lambda x: str(x).replace("/root/s2n-tls", S2N)
CBMC = os.environ["CBMC640"]
def run(func):
    cfg = C.FUNC_CONFIGS[func]
    gt = f"{C.S2N_PROOFDIR}/proofs/{func}/{func}_harness.c"
    if not os.path.exists(gt):
        return func, "NO GT"
    cmd = ([CBMC] + [remap(f) for f in cfg["flags"]] + list(cfg["unwind"])
           + list(cfg.get("unwindset") or [])
           + ["--no-standard-checks", "--no-unwinding-assertions",
              "--function", cfg["harness_entry"], gt]
           + [remap(s) for s in cfg["sources"]])
    try:
        t = subprocess.run(cmd, capture_output=True, text=True, timeout=400)
        t = t.stdout + t.stderr
    except Exception:
        return func, "TIMEOUT"
    return func, ("SUCCESS" if "VERIFICATION SUCCESSFUL" in t
                  else "FAILED" if "VERIFICATION FAILED" in t else "no verdict")
fs = sorted(k for k in C.FUNC_CONFIGS if k.startswith("s2n_"))
res = []
with ProcessPoolExecutor(max_workers=int(os.environ.get("CBMC_WORKERS", 6))) as ex:
    for f in as_completed([ex.submit(run, x) for x in fs]):
        res.append(f.result())
bad = sorted(r for r in res if r[1] != "SUCCESS")
print(f"expert harnesses verifying: {len(res) - len(bad)}/{len(res)}")
for b in bad:
    print("   ", b[0], b[1])
PY
