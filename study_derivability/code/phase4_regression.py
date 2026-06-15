#!/usr/bin/env python3
"""
Phase 4 — Statistical analysis of the Derivability Hypothesis.

Uses the 183 taxonomy assertions (missed by condA_claude) that have:
  - Per-condition binary recall labels  (taxonomy_recall_matrix.csv)
  - Phase 2 features                    (gt_assertions_features.csv)

Runs four logistic regression models:
  M1: recalled ~ LLM + condition                   (baseline)
  M2: recalled ~ LLM + condition + tax_category    (add typology)
  M3: recalled ~ LLM + condition + features        (add derivability features)
  M4: recalled ~ features only                     (features as sufficient predictor)

Also:
  - PCA on knowledge/strategy feature subsets
  - Interaction test: strategy_features × condE vs knowledge_features × condE
  - Two-barrier correlation test

Output:
  data/phase4_regression_data.csv   (long-format data)
  reports/phase_4.md
"""

import csv
import sys
import warnings
from pathlib import Path

import numpy as np
import pandas as pd
from scipy import stats
from sklearn.decomposition import PCA
from sklearn.preprocessing import StandardScaler

sys.path.insert(0, str(Path(__file__).parent))
from config import DATA_DIR, REPORT_DIR

warnings.filterwarnings("ignore")


# ── Load data ─────────────────────────────────────────────────────────────────

def load_features() -> pd.DataFrame:
    df = pd.read_csv(DATA_DIR / "gt_assertions_features.csv", low_memory=False)
    # Keep only aws_c_common assertions with taxonomy labels
    df = df[(df["corpus"] == "aws_c_common") & df["tax_id"].notna() & (df["tax_id"] != "")]
    return df


def load_recall_matrix() -> pd.DataFrame:
    df = pd.read_csv(DATA_DIR / "taxonomy_recall_matrix.csv")
    return df


def build_long_df(features: pd.DataFrame, recall: pd.DataFrame) -> pd.DataFrame:
    """
    Join features with recall labels, pivot to long format:
    one row per (assertion × condition × model).
    """
    # Merge features with recall matrix on tax_id/id
    merged = features.merge(recall, left_on="tax_id", right_on="id", suffixes=("_feat", "_tax"))

    # Condition columns in recall matrix
    cond_cols = {
        "A_claude": ("A", "claude"),
        "B_claude": ("B", "claude"),
        "C_claude": ("C", "claude"),
        "D_claude": ("D", "claude"),
        "E_claude": ("E", "claude"),
        "F_claude": ("F", "claude"),
        "A_qwen":   ("A", "qwen"),
        "B_qwen":   ("B", "qwen"),
        "E_qwen":   ("E", "qwen"),
    }

    rows = []
    feat_cols = [c for c in features.columns if c.startswith("f_")]

    for _, row in merged.iterrows():
        for col, (cond, model) in cond_cols.items():
            val = row.get(col, "")
            if str(val) not in ("0", "1"):
                continue
            rec = int(val)
            entry = {
                "tax_id":    row["tax_id"],
                "func":      row["function"],
                "expr_norm": row["expr_norm"],
                "category":  row.get("tax_category", ""),
                "cond":      cond,
                "model":     model,
                "recalled":  rec,
            }
            for fc in feat_cols:
                try:
                    entry[fc] = float(row[fc]) if str(row[fc]) not in ("", "nan") else np.nan
                except (ValueError, TypeError):
                    entry[fc] = np.nan
            rows.append(entry)

    return pd.DataFrame(rows)


# ── Feature groups ────────────────────────────────────────────────────────────

KNOWLEDGE_FEATURES = [
    "f_var_overlap", "f_field_overlap",
    "f_predicate_in_impl", "f_predicate_in_postcond", "f_predicate_in_nl",
    "f_uses_cbmc_builtin",
]
STRATEGY_FEATURES = [
    "f_is_frame_negative", "f_position_post_call",
    "f_negation_depth", "f_arity",
]
ALL_FEATURES = KNOWLEDGE_FEATURES + STRATEGY_FEATURES


# ── Logistic regression helpers ───────────────────────────────────────────────

def run_logistic(df: pd.DataFrame, formula_vars: list[str],
                 label: str, y_col: str = "recalled") -> dict:
    """Fit logistic regression with statsmodels using formula variables."""
    from statsmodels.formula.api import logit

    df2 = df[formula_vars + [y_col]].dropna()
    if len(df2) < 30:
        return {"label": label, "n": len(df2), "warning": "too few rows"}

    formula_rhs = " + ".join(formula_vars)
    try:
        model = logit(f"{y_col} ~ {formula_rhs}", data=df2).fit(disp=False)
        return {
            "label":    label,
            "n":        len(df2),
            "aic":      round(model.aic, 2),
            "bic":      round(model.bic, 2),
            "pseudo_r2": round(model.prsquared, 4),
            "converged": model.mle_retvals["converged"],
            "params":   {k: round(v, 4) for k, v in model.params.items()},
            "pvalues":  {k: round(v, 4) for k, v in model.pvalues.items()},
        }
    except Exception as e:
        return {"label": label, "error": str(e)}


def logistic_simple(x: np.ndarray, y: np.ndarray, label: str) -> dict:
    """Single-predictor Mann-Whitney test (feature value → recall probability)."""
    # Filter out N/A sentinel (-1) and NaN
    mask = ~(np.isnan(x) | np.isnan(y)) & (x >= 0)
    x, y = x[mask], y[mask]
    if len(x) < 20:
        return {"label": label, "n": len(x), "warning": "too few"}
    group1 = x[y == 1]
    group0 = x[y == 0]
    if len(group1) == 0:
        return {"label": label, "n": len(x), "warning": "no recalled"}
    stat, pval = stats.mannwhitneyu(group1, group0, alternative="greater")
    return {
        "label":  label,
        "n":      len(x),
        "n_recalled": int(y.sum()),
        "mean_recalled": round(float(group1.mean()), 4),
        "mean_not":      round(float(group0.mean()), 4) if len(group0) > 0 else None,
        "mw_stat": round(float(stat), 2),
        "p_value": round(float(pval), 4),
    }


# ── PCA ───────────────────────────────────────────────────────────────────────

def run_pca(df: pd.DataFrame) -> dict:
    """PCA on feature matrix to test two-barrier structure."""
    feat_df = df[ALL_FEATURES].dropna()
    # Replace -1 sentinels (N/A for predicate features) with 0
    # (-1 means "not applicable" ~ "feature absent", treated as 0 for PCA)
    feat_df = feat_df.replace(-1, 0)
    if len(feat_df) < 10:
        return {"warning": "too few complete rows for PCA"}

    scaler = StandardScaler()
    X = scaler.fit_transform(feat_df)
    pca = PCA(n_components=min(len(ALL_FEATURES), 5))
    pca.fit(X)

    explained = pca.explained_variance_ratio_
    loadings = pd.DataFrame(
        pca.components_.T,
        index=ALL_FEATURES,
        columns=[f"PC{i+1}" for i in range(pca.n_components_)],
    )

    # Check if PC1/PC2 separate knowledge from strategy features
    k_idx = [ALL_FEATURES.index(f) for f in KNOWLEDGE_FEATURES if f in ALL_FEATURES]
    s_idx = [ALL_FEATURES.index(f) for f in STRATEGY_FEATURES if f in ALL_FEATURES]

    pc1_knowledge = float(np.abs(pca.components_[0][k_idx]).mean())
    pc1_strategy  = float(np.abs(pca.components_[0][s_idx]).mean())
    pc2_knowledge = float(np.abs(pca.components_[1][k_idx]).mean())
    pc2_strategy  = float(np.abs(pca.components_[1][s_idx]).mean())

    return {
        "n": len(feat_df),
        "explained_variance": [round(float(v), 4) for v in explained],
        "cumulative_2pc": round(float(explained[:2].sum()), 4),
        "pc1_mean_loading_knowledge": round(pc1_knowledge, 4),
        "pc1_mean_loading_strategy":  round(pc1_strategy, 4),
        "pc2_mean_loading_knowledge": round(pc2_knowledge, 4),
        "pc2_mean_loading_strategy":  round(pc2_strategy, 4),
        "loadings_PC1": loadings["PC1"].round(4).to_dict(),
        "loadings_PC2": loadings["PC2"].round(4).to_dict(),
    }


# ── Interaction analysis ───────────────────────────────────────────────────────

def interaction_analysis(long_df: pd.DataFrame) -> dict:
    """
    Test whether condition E preferentially boosts recall for
    strategy-barrier vs knowledge-barrier assertions.
    """
    results = {}

    for cat, barrier in [("FRAME_COND", "strategy"), ("VALIDITY_PRED", "knowledge"),
                          ("CBMC_BUILTIN", "knowledge"), ("LEN_CHANGE", "mixed")]:
        sub = long_df[long_df["category"] == cat]
        if len(sub) < 5:
            continue

        cond_A = sub[sub["cond"] == "A"]["recalled"].values
        cond_E = sub[sub["cond"] == "E"]["recalled"].values
        if len(cond_A) == 0 or len(cond_E) == 0:
            continue

        results[cat] = {
            "barrier": barrier,
            "recall_A_claude": round(float(sub[(sub["cond"]=="A") & (sub["model"]=="claude")]["recalled"].mean()), 3),
            "recall_E_claude": round(float(sub[(sub["cond"]=="E") & (sub["model"]=="claude")]["recalled"].mean()), 3),
            "recall_A_qwen":   round(float(sub[(sub["cond"]=="A") & (sub["model"]=="qwen")]["recalled"].mean()), 3),
            "recall_E_qwen":   round(float(sub[(sub["cond"]=="E") & (sub["model"]=="qwen")]["recalled"].mean()), 3),
            "n": len(sub) // len(sub["cond"].unique()),
        }

    return results


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    print("Loading data …")
    features = load_features()
    recall   = load_recall_matrix()
    print(f"  Features: {len(features)} aws_c_common assertions with taxonomy labels")
    print(f"  Recall matrix: {len(recall)} rows")

    long_df = build_long_df(features, recall)
    print(f"  Long-format: {len(long_df)} rows ({long_df['tax_id'].nunique()} assertions × conditions)")

    long_df.to_csv(DATA_DIR / "phase4_regression_data.csv", index=False)
    print(f"  Saved → {DATA_DIR / 'phase4_regression_data.csv'}")

    # ── Condition/model baseline ───────────────────────────────────────────────
    print("\n── Recall by condition × model ──")
    pivot = long_df.groupby(["cond", "model"])["recalled"].agg(["mean", "count"]).reset_index()
    pivot.columns = ["cond", "model", "recall", "n"]
    print(pivot.to_string(index=False))

    # ── Category baseline ──────────────────────────────────────────────────────
    print("\n── Recall by taxonomy category (E_claude) ──")
    e_claude = long_df[(long_df["cond"] == "E") & (long_df["model"] == "claude")]
    cat_table = e_claude.groupby("category")["recalled"].agg(["mean", "count"]).reset_index()
    cat_table.columns = ["category", "recall_E_claude", "n"]
    cat_table = cat_table.sort_values("recall_E_claude", ascending=False)
    print(cat_table.to_string(index=False))

    # ── Logistic regression models ────────────────────────────────────────────
    print("\n── Logistic regression models ──")

    # Encode categoricals
    df_reg = long_df.copy()
    df_reg["is_claude"]    = (df_reg["model"] == "claude").astype(int)
    df_reg["is_condE"]     = (df_reg["cond"] == "E").astype(int)
    df_reg["is_condB"]     = (df_reg["cond"] == "B").astype(int)
    df_reg["is_condC"]     = (df_reg["cond"] == "C").astype(int)
    df_reg["is_condD"]     = (df_reg["cond"] == "D").astype(int)
    df_reg["is_condF"]     = (df_reg["cond"] == "F").astype(int)

    cond_dummies = ["is_condB", "is_condC", "is_condD", "is_condE", "is_condF"]
    cat_dummies  = [f"cat_{c}" for c in long_df["category"].unique() if c]
    for c in long_df["category"].unique():
        if c:
            df_reg[f"cat_{c}"] = (df_reg["category"] == c).astype(int)

    # M1: LLM + condition only
    m1 = run_logistic(df_reg, ["is_claude"] + cond_dummies, "M1: LLM+cond")
    print(f"\nM1  AIC={m1.get('aic','?')}  pseudo_R2={m1.get('pseudo_r2','?')}")

    # M2: LLM + condition + category
    m2 = run_logistic(df_reg, ["is_claude"] + cond_dummies + cat_dummies, "M2: LLM+cond+cat")
    print(f"M2  AIC={m2.get('aic','?')}  pseudo_R2={m2.get('pseudo_r2','?')}")

    # M3: LLM + condition + features (drop cols with high NA)
    available_feats = [f for f in ALL_FEATURES
                       if df_reg[f].notna().mean() > 0.5]
    m3 = run_logistic(df_reg, ["is_claude"] + cond_dummies + available_feats, "M3: LLM+cond+feats")
    print(f"M3  AIC={m3.get('aic','?')}  pseudo_R2={m3.get('pseudo_r2','?')}")

    # M4: features only
    m4 = run_logistic(df_reg, available_feats, "M4: features only")
    print(f"M4  AIC={m4.get('aic','?')}  pseudo_R2={m4.get('pseudo_r2','?')}")

    # ── Feature-recall Mann-Whitney tests ─────────────────────────────────────
    print("\n── Feature-recall association (Mann-Whitney, E_claude) ──")
    e_claude_df = df_reg[(df_reg["cond"] == "E") & (df_reg["model"] == "claude")]
    feat_tests = []
    for feat in ALL_FEATURES:
        if e_claude_df[feat].notna().sum() < 20:
            continue
        x = e_claude_df[feat].values
        y = e_claude_df["recalled"].values
        res = logistic_simple(x, y, feat)
        feat_tests.append(res)

    for t in sorted(feat_tests, key=lambda x: x.get("p_value", 1)):
        print(f"  {t['label']:30s}  n={t['n']:4d}  p={t.get('p_value','?'):.4f}"
              f"  mean_rec={t.get('mean_recalled','?')}  mean_not={t.get('mean_not','?')}")

    # ── PCA ───────────────────────────────────────────────────────────────────
    print("\n── PCA on feature matrix ──")
    # Use unique assertions (not long-format) for PCA
    feat_unique = long_df.drop_duplicates("tax_id")[ALL_FEATURES]
    pca_result = run_pca(feat_unique)
    print(f"  n={pca_result.get('n')}  2-PC variance={pca_result.get('cumulative_2pc')}")
    print(f"  PC1 loading: knowledge={pca_result.get('pc1_mean_loading_knowledge')}  "
          f"strategy={pca_result.get('pc1_mean_loading_strategy')}")
    print(f"  PC2 loading: knowledge={pca_result.get('pc2_mean_loading_knowledge')}  "
          f"strategy={pca_result.get('pc2_mean_loading_strategy')}")
    print("  PC1 top loadings:", sorted(pca_result.get("loadings_PC1", {}).items(),
                                        key=lambda x: abs(x[1]), reverse=True)[:5])

    # ── Interaction: strategy vs knowledge under condE ────────────────────────
    print("\n── Category recall by condition (barrier analysis) ──")
    inter = interaction_analysis(long_df)
    for cat, res in sorted(inter.items()):
        print(f"  {cat:20s} [{res['barrier']:9s}]  "
              f"A_cl={res['recall_A_claude']:.2f}  E_cl={res['recall_E_claude']:.2f}  "
              f"A_qw={res['recall_A_qwen']:.2f}  E_qw={res['recall_E_qwen']:.2f}  "
              f"n={res['n']}")

    # ── Write report ──────────────────────────────────────────────────────────
    report_lines = [
        "# Phase 4 Report — Regression Analysis",
        "",
        "## Dataset",
        f"- Assertions: {long_df['tax_id'].nunique()}",
        f"- Total observations (assertion × condition): {len(long_df)}",
        "",
        "## Logistic Regression Model Comparison",
        "",
        "| Model | Description | AIC | Pseudo-R² |",
        "|-------|-------------|-----|-----------|",
        f"| M1 | LLM + condition | {m1.get('aic','?')} | {m1.get('pseudo_r2','?')} |",
        f"| M2 | LLM + condition + typology | {m2.get('aic','?')} | {m2.get('pseudo_r2','?')} |",
        f"| M3 | LLM + condition + features | {m3.get('aic','?')} | {m3.get('pseudo_r2','?')} |",
        f"| M4 | Features only | {m4.get('aic','?')} | {m4.get('pseudo_r2','?')} |",
        "",
        "## PCA Results",
        f"- n={pca_result.get('n')} unique assertions",
        f"- Variance explained by 2 PCs: {pca_result.get('cumulative_2pc', 'n/a')}",
        f"- PC1 avg knowledge loading: {pca_result.get('pc1_mean_loading_knowledge')}",
        f"- PC1 avg strategy loading: {pca_result.get('pc1_mean_loading_strategy')}",
        "",
        "## Interaction Analysis (Barrier × Condition)",
        "",
        "| Category | Barrier | A_claude | E_claude | A_qwen | E_qwen | N |",
        "|----------|---------|----------|----------|--------|--------|---|",
    ]
    for cat, res in sorted(inter.items()):
        report_lines.append(
            f"| {cat} | {res['barrier']} | {res['recall_A_claude']:.2f} | "
            f"{res['recall_E_claude']:.2f} | {res['recall_A_qwen']:.2f} | "
            f"{res['recall_E_qwen']:.2f} | {res['n']} |"
        )

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    (REPORT_DIR / "phase_4.md").write_text("\n".join(report_lines))
    print(f"\nSaved → {REPORT_DIR / 'phase_4.md'}")


if __name__ == "__main__":
    main()
