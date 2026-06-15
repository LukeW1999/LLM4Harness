# Phase 4 Report — Regression Analysis

## Dataset
- Assertions: 183
- Total observations (assertion × condition): 1647

## Logistic Regression Model Comparison

| Model | Description | AIC | Pseudo-R² |
|-------|-------------|-----|-----------|
| M1 | LLM + condition | 1328.45 | 0.0763 |
| M2 | LLM + condition + typology | 1196.38 | 0.1818 |
| M3 | LLM + condition + features | 1198.95 | 0.1814 |
| M4 | Features only | 1306.03 | 0.0977 |

## PCA Results
- n=183 unique assertions
- Variance explained by 2 PCs: 0.4768
- PC1 avg knowledge loading: 0.2648
- PC1 avg strategy loading: 0.2826

## Interaction Analysis (Barrier × Condition)

| Category | Barrier | A_claude | E_claude | A_qwen | E_qwen | N |
|----------|---------|----------|----------|--------|--------|---|
| CBMC_BUILTIN | knowledge | 0.00 | 0.00 | 0.00 | 0.00 | 3 |
| FRAME_COND | strategy | 0.00 | 0.50 | 0.41 | 0.56 | 48 |
| LEN_CHANGE | mixed | 0.00 | 0.38 | 0.29 | 0.46 | 36 |
| VALIDITY_PRED | knowledge | 0.00 | 0.43 | 0.29 | 0.46 | 52 |