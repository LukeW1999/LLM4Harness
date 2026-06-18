# REBUILD.md — rebuild the experiment environment from scratch

Purpose: if the ECS server is released, this is how to recreate a working
environment from this GitHub repo and reproduce / re-run the core pipeline.
Every step below was verified on 2026-06-18 on the production host.

All experiment **data** (results/, evaluation/, mutants/, mutants_s2n/,
dataset_condA/B/) already lives in this repo, so reproducing the *analysis*
needs only CBMC + this repo. The external source trees and API keys are needed
only to **re-generate** harnesses or mutants.

---

## 0. Path assumption (important)

Scripts hard-code two absolute paths:
- `/root/experiment_aws_cbmc`  → this repo's `experiment_aws_cbmc/` directory
- `/root/aws-c-common`         → the aws-c-common source tree

Recreate the env at exactly these paths (clone the repo, then `mv` or symlink
`experiment_aws_cbmc` to `/root/experiment_aws_cbmc`), or edit the paths in
`scripts/cbmc_runner.py` (FUNC_CONFIGS) + `scripts/run_mutation_oracle_cbmc.py`.

## 1. System + tools (Ubuntu 24.04 LTS, 8 vCPU / 14 GiB is plenty)

```bash
sudo apt update
sudo apt install -y cbmc            # gives 5.95.1-4ubuntu1 == the version we used
pip install --break-system-packages universalmutator   # 1.14.1
pip install --break-system-packages numpy scipy anthropic openai requests
# ESBMC 8.3.0 (OPTIONAL, only for the RQ2 ESBMC robustness cross-check):
#   download the 8.3.0 linux release binary -> /usr/local/bin/esbmc
```
Verify: `cbmc --version` → 5.95.1; `mutate -h` works; `python3 --version` → 3.12.x.

## 2. This repo (the data + scripts)

```bash
git clone git@github.com:LukeW1999/LLM4Harness.git
mv LLM4Harness/experiment_aws_cbmc /root/experiment_aws_cbmc   # exact path
```
Note: the server had no GitHub SSH key configured — clone with a key/token, or
just `rsync` the local repo up. Compiled CBMC artifacts (`*.goto/*.gb/*.o`) are
gitignored and regenerate on first run.

## 3. External source trees (only for RE-GENERATION, not for analysis)

- **aws-c-common** → `/root/aws-c-common`. It was a source tarball (NOT a git
  checkout), `cmake_minimum_required 3.9`. The compiled **mutants are already in
  this repo** (`mutants/`), so analysis does not need it; mutant regeneration and
  the two source-reading audit checks (see §6) do.
- **s2n-tls** → `git clone https://github.com/aws/s2n-tls /root/s2n-tls`
  (we used commit `d4805fd`). It needs the adaptations baked into
  `scripts/cbmc_runner.py` (s2n branch): truncated `make_common_datastructures.c`
  to drop OpenSSL-3.0 BIGNUM/EC populators, and `-DS2N_MIN/-DS2N_MAX` added to
  `S2N_COMMON_FLAGS`. The s2n mutants are also already in `mutants_s2n/`.

## 4. API keys (only for re-generating harnesses; gitignored)

Create `/root/experiment_aws_cbmc/.env` (never commit it):
```
OPENROUTER_API_KEY=...
OPENROUTER_MODEL=openai/gpt-oss-120b
ANTHROPIC_API_KEY=...
```
Reproducing CBMC analysis / paper numbers does NOT need any key.

## 5. Smoke test (verify the rebuild)

```bash
cd /root/experiment_aws_cbmc

# (a) core numbers reproduce — expect "0 mismatch(es)" on a full server-grade env:
python3 scripts/paper_numbers.py | tail -2

# (b) differential oracle end-to-end on ONE function.
#  !! WARNING: --func OVERWRITES the whole dataset's oracle JSON. Back it up first:
cp evaluation/mutation_oracle_cbmc_feedback_loop_A_claude.json /tmp/_bak.json
python3 scripts/run_mutation_oracle_cbmc.py --dataset feedback_loop_A_claude --func aws_byte_buf_cat
#  expected: 44 mutants, GT caught 33, LLM caught 30, Silenced 3 (6.8%)
mv /tmp/_bak.json evaluation/mutation_oracle_cbmc_feedback_loop_A_claude.json   # restore!
```

## 6. Known reproducibility gaps (status 2026-06-18)

`paper_numbers.py` is fully green (0 mismatch) on a complete environment. On a
bare local checkout WITHOUT `/root/aws-c-common` and CBMC it reports ~7 misses:
- **2 checks** (`S4/rev` reverse-cell, `Threats/pin` pinned-KG) actually re-run
  CBMC over the source tree, so they need the full env (`/root/aws-c-common` +
  CBMC). They are computation, not data lookups.
- **5 `Thr/pin` matched-set numbers** (pinned∩unpinned function set: paper says
  37, current repo data computes 38, propagating to the matched pass%/UNKNOWN).
  This is the open portability/staleness item (task #46/#49): the paper value
  dates from an earlier data snapshot. To finalize, recompute the matched set on
  the current pinned data and update §threats, OR pin the 37 by recording which
  function the later regen added. Not data loss — all inputs are in the repo.

`_BASE` in `paper_numbers.py` resolves to `/root/experiment_aws_cbmc` if present,
else the repo's `experiment_aws_cbmc/`, so most checks run anywhere.
