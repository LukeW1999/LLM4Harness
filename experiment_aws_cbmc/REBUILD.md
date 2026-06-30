# Rebuilding the experiment environment (server teardown recovery)

Everything below is needed to **re-run** the CBMC mutation-oracle pipeline. The
**data** (oracle JSONs, attribution, cross_verify, mutants) is already in this
repo under `experiment_aws_cbmc/`; you only need this to reproduce or extend runs.

## ⚠️ CBMC version is load-bearing — pin 5.95.1, NOT 6.x
Verifier verdicts are version-sensitive. The study was generated with **CBMC 5.95.1**.
Under CBMC 6.x the LLM harnesses fail their own fidelity gate on the *unmutated*
function (e.g. `aws_byte_buf_cat` SUCCESS under 5.95.1 -> FAIL under 6.8.0), which
collapses the differential (byte_buf_cat: 33 silenced under 5.95.1 vs 0 under 6.8.0).
Do **not** `apt install cbmc` (pulls latest). Install the 5.95.1 release:
  - Download the 5.95.1 .deb from https://github.com/diffblue/cbmc/releases/tag/cbmc-5.95.1
  - `sudo dpkg -i ubuntu-*-cbmc-5.95.1-*.deb` ; verify `cbmc --version` == 5.95.1
ESBMC (cross-engine check) is the version used in §7.2; any recent ESBMC works for that.

## Sources expected by scripts/cbmc_runner.py
- aws-c-common at `/root/aws-c-common` (server) or `/home/weiqi/Verification/aws-c-common` (local).
  Commit used: 2f65e15.
  Proof-helpers live under `<aws-c-common>/verification/cbmc/{include,sources,stubs,proofs}`.
- s2n-tls at `/root/s2n-tls` (for the §6.1 cross-corpus run only).
- The path probe is portable (PermissionError-guarded); falls back to the local
  checkout if /root/* is absent.

## Toolchain
- Python 3.10+, `scipy` (stats), `universalmutator` (mutant generation; already-generated
  mutants are committed under `experiment_aws_cbmc/mutants/`, so regeneration is optional).
- ~8 vCPU recommended (oracle is embarrassingly parallel; --workers N).

## Re-run recipes
- One-function oracle smoke test:
  `python3 scripts/run_mutation_oracle_cbmc.py --dataset feedback_loop_A_gptoss120b --func aws_byte_buf_cat --workers 4`
  (NOTE: writes evaluation/mutation_oracle_cbmc_<dataset>.json — it OVERWRITES; the
   canonical results are git-tracked, so commit/restore deliberately.)
- p-value stability (review response, no CBMC): `python3 scripts/pval_stability.py`
- M1 unwinding-assertions soundness check: `python3 scripts/m1_unwind_check.py <dataset> ...`
  (requires 5.95.1; result snapshot in evaluation/m1_unwinding_check_result.txt)
