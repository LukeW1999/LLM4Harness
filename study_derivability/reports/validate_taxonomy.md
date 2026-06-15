# Taxonomy Validation Report

## What this tests

The 191 taxonomy assertions are ALL missed by condA_claude (recall=0 by definition).
This analysis checks whether the manual taxonomy labels (info_source, reasoning, category)
predict which assertions get recalled under OTHER conditions.

If the Derivability Hypothesis is correct:
- DOMAIN assertions (require library-specific predicate knowledge) should have
  the LOWEST recall across all conditions — they can't be derived from code text.
- NL assertions (documented in header) should have higher recall.
- CODE assertions (inferable from implementation) should be intermediate.
- EXHAUSTIVE reasoning (frame conditions) should be hard regardless of info_source.

## Coverage

| Condition   | Covered | Recalled | Still missed | Recall rate |
|-------------|:-------:|:--------:|:------------:|:-----------:|
| A_claude     |     191 |        0 |          191 |         0.0 |
| A_qwen       |     191 |       40 |          151 |       0.209 |
| B_claude     |     191 |       14 |          177 |       0.073 |
| B_qwen       |     191 |       37 |          154 |       0.194 |
| C_claude     |     191 |       16 |          175 |       0.084 |
| D_claude     |     191 |       12 |          179 |       0.063 |
| E_claude     |     191 |       57 |          134 |       0.298 |
| F_claude     |     191 |       29 |          162 |       0.152 |
| E_qwen       |     191 |       64 |          127 |       0.335 |

*(Covered = assertion's function was attempted under this condition)*

## Recall by info_source

| Source | A_claude | B_claude | C_claude | D_claude | E_claude | F_claude | A_qwen | B_qwen | E_qwen |
|--------|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:------:|:------:|:------:|
| NL             | 0.0 | 0.105 | 0.053 | 0.053 | 0.316 | 0.158 | 0.316 | 0.421 | 0.421 |
| CODE           | 0.0 | 0.079 | 0.087 | 0.079 | 0.252 | 0.189 | 0.173 | 0.205 | 0.283 |
| DOMAIN         | 0.0 | 0.044 | 0.089 | 0.022 | 0.422 | 0.044 | 0.267 | 0.067 | 0.444 |

## Recall by reasoning complexity

| Reasoning    | A_claude | B_claude | C_claude | D_claude | E_claude | F_claude | A_qwen | B_qwen | E_qwen |
|--------------|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:------:|:------:|:------:|
| DIRECT         | 0.0 | 0.082 | 0.066 | 0.082 | 0.213 | 0.131 | 0.18 | 0.213 | 0.213 |
| CONDITIONAL    | 0.0 | 0.0 | 0.059 | 0.059 | 0.118 | 0.088 | 0.0 | 0.088 | 0.147 |
| EXHAUSTIVE     | 0.0 | 0.188 | 0.156 | 0.094 | 0.5 | 0.5 | 0.406 | 0.469 | 0.562 |
| STRUCTURAL     | 0.0 | 0.047 | 0.078 | 0.031 | 0.375 | 0.031 | 0.25 | 0.094 | 0.438 |

## Recall by semantic category

| Category      | A_claude | E_claude | F_claude | A_qwen | E_qwen |
|---------------|:--------:|:--------:|:--------:|:------:|:------:|
| CBMC_BUILTIN   | 0.0 | 0.0 | 0.0 | 0.0 | 0.0 |
| FRAME_COND     | 0.0 | 0.5 | 0.5 | 0.406 | 0.562 |
| LEN_CHANGE     | 0.0 | 0.375 | 0.208 | 0.292 | 0.458 |
| LEN_INVARIANT  | 0.0 | 0.167 | 0.111 | 0.028 | 0.056 |
| OVERFLOW_ARITH | 0.0 | 0.0 | 0.0 | 0.0 | 0.0 |
| RETVAL_SPEC    | 0.0 | 0.2 | 0.2 | 0.2 | 0.2 |
| STRUCT_PTR     | 0.0 | 0.222 | 0.037 | 0.222 | 0.296 |
| TYPE_VARIANT   | 0.0 | 0.0 | 0.0 | 0.0 | 0.4 |
| VALIDITY_PRED  | 0.0 | 0.487 | 0.051 | 0.308 | 0.513 |
| ZERO_CHECK     | 0.0 | 0.0 | 0.0 | 0.0 | 0.0 |

## Key observations

- **DOMAIN vs NL, condE_claude**: delta = 0.106 (negative = DOMAIN harder)
- **CODE vs NL, condE_claude**: delta = -0.064
- **EXHAUSTIVE reasoning, condE_claude**: recall = 0.5
- **STRUCTURAL reasoning, condE_claude**: recall = 0.375
- **DIRECT reasoning, condE_claude**: recall = 0.213

*(See figures/validate_heatmap_*.png and figures/validate_bars_*.png)*

## Interpretation guide

- If DOMAIN recall < CODE recall < NL recall across all conditions → supports Derivability Hypothesis
- If EXHAUSTIVE (frame conditions) recall stays near 0 even under E → supports "strategy gap" sub-claim
- If recall differences between conditions (A vs E) are SIMILAR across info_source groups →
  few-shot example helps uniformly (not selectively for derivable assertions) → weaker support
- If recall differences are LARGER for DOMAIN under E → few-shot example helps specifically
  because it demonstrates the predicate vocabulary → stronger support
