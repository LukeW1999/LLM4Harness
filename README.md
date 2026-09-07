# LLM4Harness — artifact for *Convergence Is Not Completeness*

**This branch is the research artifact.** For what to run first, see
[`INSTALL.md`](INSTALL.md); for badges and reusability, [`STATUS.md`](STATUS.md);
for hardware, software and API requirements, [`REQUIREMENTS.md`](REQUIREMENTS.md).

The functional claim in one command (about two minutes, no model checker, no
network, no API key):

```bash
cd experiment_aws_cbmc && python3 scripts/paper_numbers_640.py
#  ...
#  132 numbers checked, 0 mismatch(es). CBMC 6.4.0, denom=397
```

Every number, table and figure in the paper is recomputed there from the
released per-mutant verdicts. `artifact/smoke_test.sh` wraps it with a
dependency and file-presence check.

> **Anonymity.** This branch names its authors, in its git history and in its
> development logs, and is meant for post-acceptance artifact evaluation. The
> anonymised package for double-anonymous review is produced by
> `artifact/make_anonymous_package.sh`, which exports without git metadata,
> drops the development logs, scrubs personal paths and names, and refuses to
> write the tarball if `artifact/check_anonymity.sh` still finds anything.

---

# LLM4Harness — Do LLMs Verify or Conform?

A formal study of the **completeness** of LLM-generated CBMC proof harnesses for
production C, using a **circularity-free differential mutation oracle**.

## The question

LLMs increasingly write CBMC proof harnesses and, under iterative verifier
feedback, reach `UNSAT` — but convergence is not safety. We ask whether a
passing harness actually *specifies* the function or merely *conforms* to the
verifier, and we measure the gap formally.

## Method — differential mutation oracle

For each mutant `m` of a target function, run CBMC with both the expert harness
`H_GT` and the LLM harness `H_LLM`. A **silenced bug** is certified when

```
CBMC(H_GT, m) = FAIL   ∧   CBMC(H_LLM, m) = SUCCESS
```

This requires **no assumption that the expert harness is complete** — only that
it catches something the LLM harness misses. Run in reverse it audits the
experts too (it found return-value postconditions missing from AWS's own
proofs). See the workflow figure in `paper/`.

## Corpora

- **aws-c-common** — 83 functions, 238 expert harnesses, 1,233 mutants (397 GT-detected under the pinned CBMC 6.4.0; 370 under the earlier 5.95.1 sweep).
- **s2n-tls** — 25 `stuffer` functions, 253 GT-detected mutants (cross-corpus replication).

## Models

gpt-oss-120b (primary, via OpenRouter) and Claude Sonnet 4.6 (Anthropic);
DeepSeek-V4-Flash plus a six-model screen (GPT-5.5, Llama 3.3 70B,
DeepSeek-V4-Pro) for the sacrifice analysis.

## Conditions (feedback protocols)

`G` single-pass (no loop) · `A` deletion-permitting · `H` strategy-neutral ·
`M` bounded-allocation hint · `I`/`J`/`K` ablations · `Oracle` free GT
preconditions (negative control).

## Findings

1. **Pass rate is an inverted proxy** for specification quality — the
   highest-pass conditions catch the fewest bugs (Spearman ρ = −0.26); the
   Oracle control, handed GT preconditions for free, silences the most.
2. **Knowledge gaps dominate; gaming is rarest** — 81.2–100% of silenced bugs
   are catching assertions the LLM never wrote (96.5% under a rename-immune
   behavioural re-check). Active assertion sacrifice (SAC) is rare and
   seed-fragile; an earlier token-overlap estimate over-reported it 3×,
   corrected here by exact-match + CBMC-semantic attribution (28/28 blind
   adjudication).
3. **The gap is generative, not purely epistemic** — a CBMC-scored cloze test:
   models fill 33/34 blanked expert assertions with bug-catching ones.
4. **Cross-corpus** — on s2n-tls the picture is identical (KG 100%, distributed
   across functions).
5. **Robust** — silenced counts swing run-to-run, but the mechanism stays
   KG-dominated in every run and under provider/precision-pinned (bf16) re-runs.

## Reproducibility

Every number in the paper is recomputed from released artifacts by an audit
script (`scripts/paper_numbers_640.py`, 132 numbers, 0 mismatch, CBMC 6.4.0;
`scripts/paper_numbers.py` still audits the superseded 5.95.1 sweep). Open-model runs use the
OpenRouter API; the precision-mixing caveat is documented, and the headline is
corroborated on the Anthropic-served Claude and a pinned bf16 re-run.

**The pinned checker is CBMC 6.4.0**, the version aws-c-common's own CI proofs
run, and no distribution packages it. The audit itself reads the released JSONs
and needs no checker; regenerating them does:

```bash
cd experiment_aws_cbmc
./scripts/get_cbmc640.sh                                    # download, verify sha256, unpack
export CBMC640=$HOME/tools/cbmc-6.4.0/extracted/usr/bin/cbmc
python3 scripts/gtfail_640.py                               # or any other scripts/*_640.py
```

The script unpacks rather than installs, so a system CBMC is left alone; every
`*_640.py` reads `$CBMC640` and falls back to `cbmc` on PATH, which will give
the wrong verdicts if that is a different version. The earlier CBMC 5.95.1
verdict is recorded alongside the 6.4.0 one in each `evaluation/*_640.json`, so
the version migration is auditable rather than a replacement (the shared GT-fail
denominator moves 370 → 397). Verified on a second machine with a freshly
fetched binary: `k_passrate_640.py` reproduces its released JSON exactly.

## Repository layout

- `experiment_aws_cbmc/` — Study 1: `dataset/`, `scripts/`, `prompts/`,
  `results/`, `evaluation/`, `annotation/`, `figures/`.
- `experiment_s2n/` — Study 2: s2n-tls cross-corpus replication.
- `paper/` — manuscript (*Do LLMs Verify or Conform?*).
- `research_design.md` — full study design and running log.
- `PREREGISTRATION_sac_threshold.md`, `ADJUDICATION_attribution_v2.md` —
  pre-registration and blind adjudication of the attribution rule.

## Development log

- **Setup** — 83-function aws-c-common corpus, 238 expert harnesses, CBMC
  mutation oracle.
- **Conditions & taxonomy** — feedback conditions G/A/H/M/I/J/K/Oracle; the
  KG / SAC / AOC mechanism taxonomy.
- **Attribution correction** — token-overlap over-counted SAC 3×; replaced with
  exact-match + CBMC-semantic attribution, validated by blind adjudication.
- **Generalization** — s2n-tls second corpus (KG 100%, distributed).
- **Robustness** — multi-run variability; provider/precision-pinned
  (DeepInfra/bf16) confirmation of the headline.
- **Review hardening** — behavioural rename-immune KG re-attribution (96.5%);
  Clopper–Pearson bounds on the SAC zeros; Sil/GT-among-compilable; pass-rate vs
  recall and mechanism-attribution figures; positioning against RLVR
  reward-hacking (Helff et al. 2026).
- **CBMC 6.4.0 migration** — every version-dependent number recomputed under the
  version aws-c-common's CI proofs run (`scripts/*_640.py`); GT-fail denominator
  370 -> 397, audited by `scripts/paper_numbers_640.py` (132 numbers, 0 mismatch).
- *In progress* — pinned-precision re-run of all eight RQ1 conditions to confirm
  the pass-rate/recall inversion is precision-robust.
