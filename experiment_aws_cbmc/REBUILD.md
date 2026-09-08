# Rebuilding the experiment environment (server teardown recovery)

Everything below is needed to **re-run** the CBMC mutation-oracle pipeline. The
**data** (oracle JSONs, attribution, cross_verify, mutants) is already in this
repo under `experiment_aws_cbmc/`; you only need this to reproduce or extend runs.

## ⚠️ CBMC version is load-bearing — pin 6.4.0
The paper's verdicts are CBMC **6.4.0** (`scripts/get_cbmc640.sh` fetches it; the
earlier 5.95.1 sweep is kept as the version comparison). Do **not**
`apt install cbmc`: it pulls whatever is current, and one default changed
between 6.4.0 and 6.8.0 that decides these results.

| invocation | unwinding assertions |
|---|---|
| `cbmc 6.4.0` with no unwinding flag | OFF |
| `cbmc 6.8.0` with no unwinding flag | **ON** |

aws-c-common's `Makefile.common` leaves `CBMC_FLAG_UNWINDING_ASSERTIONS` empty,
so the production proofs inherit whichever default their CBMC has.

**An earlier version of this file read that the wrong way round.** It recorded
that under 6.8.0 the LLM harnesses fail their own fidelity gate on the unmutated
function and the differential collapses (`aws_byte_buf_cat`: 33 silenced -> 0),
and concluded 6.x was unsuitable. The observation is right and the conclusion was
not: with unwinding assertions ON, CBMC is correctly reporting that the harness's
loop outruns its bound, so the harness never executes its postconditions and was
never entitled to pass. Those 33 silences are the dead-scaffold mechanism
(`scripts/reachability_probe.py`, `scripts/vacuity_cause.py`), not an artefact of
the checker. Expert harnesses are unaffected: they still verify with the check on.

Every script passes the unwinding setting explicitly, so the CBMC version only
decides what you get when you run CBMC by hand.
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
