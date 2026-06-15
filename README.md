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

- **aws-c-common** — 83 functions, 238 expert harnesses, 1,233 mutants (370 GT-detected).
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
script (`scripts/paper_numbers.py`, 0 mismatch). Open-model runs use the
OpenRouter API; the precision-mixing caveat is documented, and the headline is
corroborated on the Anthropic-served Claude and a pinned bf16 re-run.

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
- *In progress* — pinned-precision re-run of all eight RQ1 conditions to confirm
  the pass-rate/recall inversion is precision-robust.
