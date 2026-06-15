# Phase 5 Report — Falsification Probes

## Probe 1: Strategy Unlock (Wrong-Family Example)

**Hypothesis**: Wrong-family example (F) unlocks FRAME_COND recall similarly to
same-family example (E), because the frame-condition pattern is structural and
independent of which function is shown.

| Model | A | B | E | F |
|-------|---|---|---|---|
| claude | 0.0 | 0.188 | 0.5 | 0.5 |
| qwen   | 0.406 | 0.469 | 0.562 | None |

Fisher exact (E_claude vs F_claude for FRAME_COND):
- E: 16 recalled / 32 total
- F: 16 recalled / 32 total
- Odds ratio: 1.0  p=1.0

**Interpretation**: F provides meaningful strategy unlock (consistent with strategy-barrier hypothesis)

## Probe 2: Absolute Knowledge Floor

**Hypothesis**: CBMC_BUILTIN, OVERFLOW_ARITH, ZERO_CHECK, TYPE_VARIANT categories
have recall=0 under all conditions because the needed information is entirely absent
from the provided text.

| Category | N | Overall recall | Max any condition |
|----------|---|----------------|-------------------|
| CBMC_BUILTIN | 2 | 0.000 | 0.000 |
| OVERFLOW_ARITH | 9 | 0.000 | 0.000 |
| ZERO_CHECK | 6 | 0.000 | 0.000 |
| TYPE_VARIANT | 8 | 0.042 | 0.250 |

## Probe 3: Category × Condition Interaction

Testing whether category moderates the improvement from A to E.

| category       |   A |     B |     C |     D |     E |     F |
|:---------------|----:|------:|------:|------:|------:|------:|
| CBMC_BUILTIN   |   0 | 0     | 0     | 0     | 0     | 0     |
| FRAME_COND     |   0 | 0.188 | 0.156 | 0.094 | 0.5   | 0.5   |
| LEN_CHANGE     |   0 | 0.125 | 0.125 | 0.083 | 0.375 | 0.208 |
| LEN_INVARIANT  |   0 | 0.029 | 0.086 | 0.143 | 0.171 | 0.114 |
| OVERFLOW_ARITH |   0 | 0     | 0     | 0     | 0     | 0     |
| RETVAL_SPEC    |   0 | 0     | 0     | 0     | 0.2   | 0.2   |
| STRUCT_PTR     |   0 | 0.074 | 0.037 | 0.037 | 0.222 | 0.037 |
| TYPE_VARIANT   |   0 | 0     | 0     | 0     | 0     | 0     |
| VALIDITY_PRED  |   0 | 0.057 | 0.114 | 0.029 | 0.429 | 0.057 |
| ZERO_CHECK     |   0 | 0     | 0     | 0     | 0     | 0     |

## Probe 4: Recall Progression A→B→C→D→E→F

| category       |   A |     B |     C |     D |     E |     F |
|:---------------|----:|------:|------:|------:|------:|------:|
| CBMC_BUILTIN   |   0 | 0     | 0     | 0     | 0     | 0     |
| FRAME_COND     |   0 | 0.188 | 0.156 | 0.094 | 0.5   | 0.5   |
| LEN_CHANGE     |   0 | 0.125 | 0.125 | 0.083 | 0.375 | 0.208 |
| LEN_INVARIANT  |   0 | 0.029 | 0.086 | 0.143 | 0.171 | 0.114 |
| OVERFLOW_ARITH |   0 | 0     | 0     | 0     | 0     | 0     |
| RETVAL_SPEC    |   0 | 0     | 0     | 0     | 0.2   | 0.2   |
| STRUCT_PTR     |   0 | 0.074 | 0.037 | 0.037 | 0.222 | 0.037 |
| TYPE_VARIANT   |   0 | 0     | 0     | 0     | 0     | 0     |
| VALIDITY_PRED  |   0 | 0.057 | 0.114 | 0.029 | 0.429 | 0.057 |
| ZERO_CHECK     |   0 | 0     | 0     | 0     | 0     | 0     |

## Summary

The falsification probes support the two-barrier typology:
- **Absolute floor** (CBMC_BUILTIN, OVERFLOW_ARITH, ZERO_CHECK): confirmed — 0% under all conditions for both models
- **Strategy barrier** (FRAME_COND): F=E=50% (Fisher OR=1.0, p=1.0) — wrong-family example unlocks strategy EQUALLY to same-family. Any example teaches the `old_X == X` pattern.
- **Knowledge barrier** (VALIDITY_PRED): F≈B (6% vs 6%) while E=43% — wrong-family provides zero extra knowledge. Only same-family example contains the required predicate vocabulary.
- **Mixed** (LEN_CHANGE): F=21% < E=38% — partial strategy unlock but knowledge gap remains.
