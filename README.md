# LLM4Harness

LLM-assisted generation of CBMC proof harnesses for memory-safety C libraries.

## Overview

We study whether large language models can generate **CBMC proof harnesses**
that match the recall of hand-written ground-truth harnesses, and what kinds of
prompting actually help.

Two empirical studies, two models (Claude Sonnet, Qwen2.5-Coder-32B), six
prompting conditions:

| Cond | Prompt                  | Claude | Qwen  |
|------|-------------------------|--------|-------|
| A    | Natural-language docs   | 46.3%  | 39.2% |
| B    | No NL                   | 47.0%  | 40.3% |
| C    | NL + Chain-of-Thought   | 46.0%  | 35.1% |
| D    | No NL + CoT             | 43.0%  | 37.9% |
| E    | Same-family GT example  | **55.5%** | **47.0%** |
| F    | Wrong-family GT example | 52.1%  | -     |

## Key findings

1. **Recall ceiling at ~47%** — A ≈ B ≈ C ≈ D; standard prompt engineering has
   no effect.
2. **Two-mechanism finding** — any CBMC example contributes +5.8pp (generic
   idiom learning); a *same-family* example adds another +15pp on linked-list
   (family-specific predicate vocabulary).
3. **Structural-property gap** — 56% of missed assertions are
   `VALIDITY_PRED` / `FRAME_COND` / `LEN_INVARIANT` (taxonomy in
   `experiment_aws_cbmc/annotation/`).
4. **Cross-library replication on s2n-tls** — Delta(E-A) = +9.5pp
   (p = 0.003), matching the aws-c-common pattern.

## Repository layout

- `experiment_aws_cbmc/` — Study 1 on aws-c-common (170 GT harnesses, conditions A-F)
  - `scripts/`, `prompts/`, `dataset/`, `results/`, `evaluation/`,
    `annotation/`, `figures/`, `slides.tex`
- `experiment_s2n/` — Study 2 on s2n-tls stuffer family (25 functions, A and E)
- `paper/` — EMSE journal manuscript (`main.tex`, `references.bib`)

## Models and tools

- **CBMC** — primary bounded model checker; harness recall is measured against
  the assertions extracted by `cbmc --show-properties`.
- **ESBMC** — secondary bounded model checker used as a *cross-tool replication*
  to confirm that findings are not CBMC-specific. Wrappers and stubs live in
  `experiment_aws_cbmc/esbmc_overrides/`; runners are
  `scripts/esbmc_runner.py`, `scripts/run_esbmc_gt_batch.py`, and
  `scripts/run_esbmc_llm_batch.py`. Results are stored in
  `results/esbmc_gt_results.json` and `results/esbmc_llm_feedback_loop_*.json`.
- **Claude** — `claude-sonnet` via the Anthropic API.
- **Qwen** — `qwen2.5-coder-32b-instruct` via DashScope.

## Subjects

- `aws-c-common` — AWS foundational C library, 170 hand-written CBMC harnesses.
- `s2n-tls` — AWS TLS implementation, 68 hand-written stuffer harnesses.
