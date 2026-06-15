# Phase 2 Report

## What I did

- Built source text indices for all three corpora
- Computed 10 numeric features per assertion (6 knowledge-barrier, 4 strategy-barrier)
- Generated correlation heatmap (aws-c-common assertions with no NA features)
- Generated feature distribution plots

## Key numbers

### Feature summary (all corpora, NA excluded)

| Feature                        |    n |  NA |   mean | frac>0  |
|--------------------------------|:----:|:---:|:------:|:-------:|
| f_var_overlap                  | 1200 |   0 |  0.386 |    0.639 |
| f_field_overlap                |  524 | 676 |  0.476 |    0.494 |
| f_predicate_in_impl            |  159 | 1041 |  0.742 |    0.742 |
| f_predicate_in_postcond        |  159 | 1041 |  0.604 |    0.604 |
| f_predicate_in_nl              |  159 | 1041 |  0.038 |    0.038 |
| f_uses_cbmc_builtin            | 1200 |   0 |  0.032 |    0.033 |
| f_is_frame_negative            | 1200 |   0 |  0.148 |    0.148 |
| f_position_post_call           |  490 | 710 |  0.131 |    0.131 |
| f_negation_depth               | 1200 |   0 |  0.348 |    0.344 |
| f_arity                        | 1200 |   0 |  0.152 |     0.14 |

### Sanity checks on taxonomy-matched assertions (183 rows)

- Frame condition rate (f_is_frame_negative=1): 0.0%
  (expected ~17% — matches taxonomy FRAME_COND proportion)
- DOMAIN assertions with predicate in impl (f_predicate_in_impl=1): 0.0%
  (expected ~30–50% based on A_qwen DOMAIN recovery rate)

## Interesting observations

1. **f_position_post_call** [check]: many ASSUME-kind properties are before the call
   (preconditions); assertions should be after. If the split is clean, this feature
   adds information about precondition vs postcondition nature.

2. **f_field_overlap NA rate** [expected]: many assertions are function calls
   (e.g. `aws_X_is_valid(&_arg_)`) with no struct field access → f_field_overlap = -1.
   This is expected and handled as NA in regression.

3. **FreeRTOS source coverage** [check]: FreeRTOS functions mapped via name-variant
   search. For functions where no source was found, impl = full combined source text
   (conservative: gives max possible variable overlap, biasing f_var_overlap high).

## Questions for the PI

1. Is f_field_overlap NA (~40–60% of rows) acceptable for regression? Plan: use multiple
   imputation or drop the feature if NA rate is too high.
2. Should f_position_post_call be computed on the GT harness or LLM harness?
   (Currently: GT harness — tells us structural position, not LLM behavior)
3. Confirm: for Phase 4, use only ASSERT-kind rows (not ASSUME)?

## What I propose to do next

Phase 4 analysis: build the `(assertion, LLM, condition)` regression dataframe,
fit M1–M4 mixed-effects logistic models, run PCA on features to test typology.
(Phase 3 FreeRTOS LLM runs can proceed in parallel with Phase 4 on existing data.)
