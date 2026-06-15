# LLM4Harness Experiment Development Log

**Maintainer:** Weiqi Wang  
**Start date:** 2026/06/02  
**Purpose:** Track all code changes, test status, and data state for the LLM4Harness experiment pipeline.

---

## Architecture Overview

```
scripts/
  feedback_loop.py        ← LLM harness generation (Conditions A–F exist)
  cbmc_runner.py          ← CBMC execution wrapper
  esbmc_runner.py         ← ESBMC execution wrapper
  cross_verify.py         ← Recall / property matching
  mutation_test.py        ← Custom regex mutants (to be supplemented)
  run_esbmc_gt_batch.py   ← Batch ESBMC on GT harnesses
  run_esbmc_llm_batch.py  ← Batch ESBMC on LLM harnesses

  [TO BUILD]
  build_iteration_log.py  ← Post-hoc assertion diff logger (Phase 2)
  sacrifice_attribution.py ← Match missed properties to iteration log (Phase 2)
  generate_mutants.py     ← universalmutator integration (Phase 3)
  bidirectional_cex.py    ← Four-outcome CEX pipeline (Phase 4–5)
  rq3_pipeline.py         ← Three-arm RQ3 experiment (Phase 7)

tests/
  test_iteration_log.py
  test_sacrifice_attribution.py
  test_generate_mutants.py
  test_bidirectional_cex.py
```

---

## Data State

### Existing results (do not overwrite)

| Condition | LLM | Functions | Notes |
|-----------|-----|-----------|-------|
| A | qwen | 84 | Core baseline |
| A | claude | 84 | Primary LLM |
| B | qwen | 84 | +NL documentation |
| B | claude | 84 | |
| C | qwen | 83 | +CoT |
| C | claude | 84 | |
| D | qwen | 83 | +CoT, no NL |
| D | claude | 84 | |
| E | qwen | 83 | Same-family few-shot |
| E | claude | 83 | |
| F | claude | 83 | Wrong-family few-shot |

**Critical:** All `iter_N_harness.c` files are on disk and must never be deleted. They are the raw data for the iteration logger.

### Evaluation results (existing)

- `evaluation/cross_verify_results_cond{X}_{model}.json` — 83–84 functions each, recall + classification
- `evaluation/iter_recall_feedback_loop_*.json` — per-iteration recall trajectory
- `annotation/annotated_missed_properties.csv` — human annotation (partial)

### Missing conditions (to be run)

| Condition | Status | Description |
|-----------|--------|-------------|
| G | ☐ Not started | Single-pass, no CBMC feedback |
| H | ☐ Not started | Strategy-neutral repair prompt |
| I | ☐ Not started | Category label at each SAT failure |
| J | ☐ Not started | Running deletion log shown to LLM |
| K | ☐ Not started | Specification-first pipeline |
| Oracle Setup | ☐ Not started | H_GT preconditions provided |

---

## Implementation Roadmap

### Step 1: build_iteration_log.py  ← CURRENT
**Goal:** Compute per-assertion diff between consecutive iterations from saved harness files.  
**Input:** `results/feedback_loop_{cond}_{model}/{func}/iter_*.c`  
**Output:** `results/feedback_loop_{cond}_{model}/{func}/iteration_log.json`  
**Log schema:**
```json
[
  {
    "iteration": 2,
    "action": "delete",
    "assert_text": "buf->len == 0",
    "triggered_violation": true,
    "context": "fix_verify"
  }
]
```
**Test file:** `tests/test_iteration_log.py`  
**Status:** ☐ Not started

---

### Step 2: sacrifice_attribution.py
**Goal:** For each function, classify each missed GT assertion as never-generated / weakened / deleted (active sacrifice).  
**Input:** `iteration_log.json` + `cross_verify_results_cond{X}.json` + GT harness  
**Output:** `evaluation/sacrifice_attribution_cond{X}.json`  
**Schema per function:**
```json
{
  "func": "aws_byte_buf_init",
  "missed_gt_assertions": [
    {
      "gt_assert": "buf->allocator == allocator",
      "category": "validity_predicate",
      "classification": "deleted",
      "sacrifice_attributed": true,
      "triggered_before_deletion": true,
      "recovered": false
    }
  ],
  "sacrifice_fraction": 0.33,
  "never_generated_fraction": 0.5,
  "weakened_fraction": 0.17,
  "sacrifice_then_recover_count": 0
}
```
**Test file:** `tests/test_sacrifice_attribution.py`  
**Status:** ☐ Not started

---

### Step 3: New conditions G, H (feedback_loop.py extension)
**Goal:** Add Condition G (single-pass) and Condition H (strategy-neutral) to feedback_loop.py.  
**Changes to feedback_loop.py:**
- Add `"G"` to CONDITION_DATASET/CONDITION_PROMPT (single-pass = max_iterations=1)
- Add `"H"` with a new `build_fix_verification_prompt_H()` that omits all repair strategy hints
- The existing `build_fix_verification_prompt()` prompt with `__CPROVER_assume` stays as Condition A
**Test file:** `tests/test_new_conditions.py`  
**Status:** ☐ Not started

---

### Step 4: generate_mutants.py (universalmutator integration)
**Goal:** Generate ~1,900 compilation-passing mutants using universalmutator.  
**Input:** 108 function source files  
**Output:** `mutants/{func}/mutant_{N}.c` + `mutants/mutant_index.json`  
**Pipeline:**
1. universalmutator generate → M_raw
2. gcc compilation filter → M_all
3. TCE (object file identity check) → M_all_filtered
**Test file:** `tests/test_generate_mutants.py`  
**Status:** ☐ Not started

---

### Step 5: bidirectional_cex.py (Phase 4–5)
**Goal:** ESBMC(H_GT, m) × ESBMC(H_LLM, m) for all mutants → four-outcome classification.  
**Input:** `mutants/`, `results/feedback_loop_{cond}/`, GT harnesses  
**Output:** `evaluation/bidirectional_cex_cond{X}.json`  
**Four outcomes:** GT_SAT/LLM_UNSAT (primary), GT_SAT/LLM_SAT, GT_UNSAT/LLM_SAT, GT_UNSAT/LLM_UNSAT  
**Equivalent mutant partition:** TCE + random differential testing (500 inputs)  
**Test file:** `tests/test_bidirectional_cex.py`  
**Status:** ☐ Not started

---

### Step 6: ablation conditions I, J, K, Oracle Setup
**Goal:** Implement remaining ablation conditions.  
**Status:** ☐ Not started — depends on Step 1 (J needs deletion log from iteration logger)

---

### Step 7: rq3_pipeline.py (three-arm, Phase 7)
**Goal:** Pipeline A / B-strict / B-relaxed on stratified RQ2 confirmed bugs.  
**Status:** ☐ Not started — depends on Steps 4–5

---

## Test Conventions

Every script must have a corresponding test file in `tests/`.

**Test structure:**
```
tests/
  fixtures/                    ← Small synthetic data for unit tests
    mock_iter_1_harness.c
    mock_iter_2_harness.c
    mock_iter_3_harness.c
    mock_summary.json
    mock_gt_harness.c
    mock_cross_verify.json
  test_iteration_log.py
  test_sacrifice_attribution.py
  ...
```

**Test rules:**
1. Unit tests use only `tests/fixtures/` — never touch real `results/` data
2. Integration tests are marked `@pytest.mark.integration` and skipped by default
3. Every function exported by a script must have at least one test
4. Tests must run in < 5 seconds (no CBMC invocations in unit tests)
5. Run with: `cd experiment_aws_cbmc && python -m pytest tests/ -v`

---

## Regression Checklist

Before merging any change, verify:

- [ ] `python -m pytest tests/ -v` passes (all unit tests green)
- [ ] `cross_verify_results_condA_claude.json` unchanged (no data corruption)
- [ ] `annotated_missed_properties.csv` unchanged
- [ ] No `iter_N_harness.c` files deleted or modified

---

## Change Log

| Date | Step | Change | Tests | Status |
|------|------|--------|-------|--------|
| 2026/06/02 | — | Initial DEVLOG created | — | ✅ |

