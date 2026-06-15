# Phase 1 Report

## What I did

- Extracted GT assertions using CBMC --show-properties for aws-c-common (with
  regex fallback for any function missing from FUNC_CONFIGS).
- Extracted GT assertions using regex for s2n-tls (from JSON) and FreeRTOS (from .c files).
- Applied uniform normalization mirroring cross_verify.py's normalize_prop().
- Joined aws-c-common assertions with the 191-row taxonomy on (function, expr_norm).

## Key numbers

### Assertion counts per corpus

| Corpus             | Functions | Assertions | Assumes | Mean assert/func |
|--------------------|:---------:|:----------:|:-------:|:----------------:|
| aws-c-common       |        84 |        312 |      41 |              3.7 |
| s2n-tls            |        25 |        192 |      40 |              7.7 |
| FreeRTOS-Plus-TCP  |        72 |        228 |     387 |              3.2 |
| **Total**          |       181 |        732 | | |

### Taxonomy join (aws-c-common only)

- Taxonomy rows: 191
- Matched: 183 (95.8%)
- Unmatched: 8

## Interesting observations

1. **Join rate** [check]: 95.8% of the 191 taxonomy rows match. Unmatched rows
   are listed below — likely due to normalization differences or CBMC vs regex extraction.

2. **FreeRTOS assertion density** [likely real]: FreeRTOS harnesses are significantly
   larger and may have more complex assertion patterns including protocol-specific checks.

3. **Normalization consistency** [important]: Phase 2 features must use the same norm()
   function defined here to ensure correct join with the taxonomy recall labels from Phase 0.

## Unmatched taxonomy rows (first 8)

```
  [TYPE_VARIANT   ] aws_add_size_saturating: r == a + b
  [CBMC_BUILTIN   ] aws_mul_size_checked: __cprover_overflow_mult(a, b)
  [TYPE_VARIANT   ] aws_mul_size_saturating: r == a * b
  [LEN_INVARIANT  ] aws_array_list_swap: !list.length || list.data check failed
  [VALIDITY_PRED  ] aws_linked_list_insert_after: aws_linked_list_node_next_is_valid(&_arg_)
  [VALIDITY_PRED  ] aws_linked_list_insert_after: aws_linked_list_node_prev_is_valid(&_arg_)
  [VALIDITY_PRED  ] aws_linked_list_insert_before: aws_linked_list_node_prev_is_valid(&_arg_)
  [VALIDITY_PRED  ] aws_linked_list_insert_before: aws_linked_list_node_next_is_valid(&_arg_)
```

## Questions for the PI

1. Are the assertion counts reasonable? (Expected: ~400–600 total assertions)
2. Should assumes be included in the Phase 4 regression, or assertions only?
3. For FreeRTOS: confirm all 79 harnesses should be used (or apply the 40-function shortlist)?

## What I propose to do next

Begin Phase 2 feature engineering on `gt_assertions_joined.csv`.
