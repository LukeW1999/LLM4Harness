#!/usr/bin/env python3
"""
Generate key figures for the Derivability paper.

Figure 1: Three-tier recall heatmap (category × condition, claude)
Figure 2: E vs F comparison — smoking gun for strategy vs knowledge barrier
Figure 3: FreeRTOS module-level recall under E (stub derivability)
Figure 4: Logistic regression model comparison (AIC/R²)
"""

import sys
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import pandas as pd

sys.path.insert(0, str(Path(__file__).parent))
from config import DATA_DIR, FIG_DIR, REPORT_DIR

FIG_DIR.mkdir(parents=True, exist_ok=True)

# ── Color scheme ──────────────────────────────────────────────────────────────
FLOOR_COLOR    = "#d62728"   # red — absolute floor
STRATEGY_COLOR = "#2ca02c"   # green — strategy barrier (unlocked by any example)
KNOW_COLOR     = "#1f77b4"   # blue — knowledge barrier (family-specific)
MIXED_COLOR    = "#ff7f0e"   # orange — mixed
NEUTRAL_COLOR  = "#7f7f7f"   # grey

CATEGORY_TIER = {
    "CBMC_BUILTIN":   "floor",
    "OVERFLOW_ARITH": "floor",
    "ZERO_CHECK":     "floor",
    "TYPE_VARIANT":   "floor",
    "FRAME_COND":     "strategy",
    "RETVAL_SPEC":    "strategy",
    "VALIDITY_PRED":  "knowledge",
    "STRUCT_PTR":     "knowledge",
    "LEN_CHANGE":     "mixed",
    "LEN_INVARIANT":  "mixed",
}

TIER_COLORS = {
    "floor": FLOOR_COLOR,
    "strategy": STRATEGY_COLOR,
    "knowledge": KNOW_COLOR,
    "mixed": MIXED_COLOR,
}


def load_long_df() -> pd.DataFrame:
    return pd.read_csv(DATA_DIR / "phase4_regression_data.csv")


# ── Figure 1: Category × Condition heatmap ────────────────────────────────────

def fig1_heatmap(long_df: pd.DataFrame):
    claude = long_df[long_df["model"] == "claude"]
    cond_order = ["A", "B", "C", "D", "E", "F"]
    cats = sorted(CATEGORY_TIER.keys())

    data = []
    for cat in cats:
        row = []
        for cond in cond_order:
            vals = claude[(claude["category"] == cat) & (claude["cond"] == cond)]["recalled"].values
            row.append(float(vals.mean()) if len(vals) > 0 else 0.0)
        data.append(row)

    data = np.array(data)

    fig, ax = plt.subplots(figsize=(8, 5))
    im = ax.imshow(data, cmap="YlGn", vmin=0, vmax=1, aspect="auto")

    ax.set_xticks(range(len(cond_order)))
    ax.set_xticklabels(cond_order, fontsize=11)
    ax.set_yticks(range(len(cats)))
    ax.set_yticklabels(cats, fontsize=10)

    # Add text annotations
    for i in range(len(cats)):
        for j in range(len(cond_order)):
            val = data[i, j]
            color = "black" if val < 0.6 else "white"
            ax.text(j, i, f"{val:.2f}", ha="center", va="center",
                    fontsize=9, color=color, fontweight="bold" if val > 0 else "normal")

    # Color-code y-axis tick labels by tier
    for tick, cat in zip(ax.get_yticklabels(), cats):
        tier = CATEGORY_TIER.get(cat, "mixed")
        tick.set_color(TIER_COLORS[tier])

    ax.set_xlabel("Condition (Claude Sonnet)", fontsize=11)
    ax.set_ylabel("Assertion category", fontsize=11)
    ax.set_title("Postcondition recall by category and condition (aws-c-common)", fontsize=12)

    plt.colorbar(im, ax=ax, label="Recall")

    # Add tier legend
    patches = [
        mpatches.Patch(color=FLOOR_COLOR,    label="Absolute floor"),
        mpatches.Patch(color=STRATEGY_COLOR, label="Strategy-resolvable"),
        mpatches.Patch(color=KNOW_COLOR,     label="Knowledge-dependent"),
        mpatches.Patch(color=MIXED_COLOR,    label="Mixed"),
    ]
    ax.legend(handles=patches, loc="upper right", fontsize=8,
              bbox_to_anchor=(1.35, 1), title="Barrier type")

    plt.tight_layout()
    path = FIG_DIR / "fig1_category_heatmap.pdf"
    plt.savefig(path, bbox_inches="tight", dpi=150)
    plt.close()
    print(f"Saved → {path}")


# ── Figure 2: E vs F smoking gun ─────────────────────────────────────────────

def fig2_ef_comparison(long_df: pd.DataFrame):
    """Bar chart comparing E and F recall for key categories (claude)."""
    cats_show = ["FRAME_COND", "VALIDITY_PRED", "STRUCT_PTR", "LEN_CHANGE",
                 "CBMC_BUILTIN", "OVERFLOW_ARITH"]
    claude = long_df[long_df["model"] == "claude"]

    e_recalls, f_recalls = [], []
    for cat in cats_show:
        sub = claude[claude["category"] == cat]
        e = sub[sub["cond"] == "E"]["recalled"].mean()
        f = sub[sub["cond"] == "F"]["recalled"].mean()
        e_recalls.append(e if not np.isnan(e) else 0.0)
        f_recalls.append(f if not np.isnan(f) else 0.0)

    x = np.arange(len(cats_show))
    width = 0.35

    fig, ax = plt.subplots(figsize=(9, 4.5))
    bars_e = ax.bar(x - width/2, e_recalls, width, label="E (same-family example)",
                    color="#2ca02c", alpha=0.85)
    bars_f = ax.bar(x + width/2, f_recalls, width, label="F (wrong-family example)",
                    color="#ff7f0e", alpha=0.85)

    ax.set_xticks(x)
    ax.set_xticklabels(cats_show, rotation=20, ha="right", fontsize=10)
    ax.set_ylabel("Postcondition recall", fontsize=11)
    ax.set_title("Same-family vs wrong-family example: strategy vs knowledge barrier\n(Claude Sonnet, aws-c-common taxonomy)", fontsize=11)
    ax.set_ylim(0, 0.7)
    ax.legend(fontsize=10)
    ax.yaxis.grid(True, linestyle="--", alpha=0.5)
    ax.set_axisbelow(True)

    # Add value labels
    for bar in bars_e:
        h = bar.get_height()
        if h > 0.02:
            ax.text(bar.get_x() + bar.get_width()/2, h + 0.01, f"{h:.2f}",
                    ha="center", va="bottom", fontsize=8)
    for bar in bars_f:
        h = bar.get_height()
        if h > 0.02:
            ax.text(bar.get_x() + bar.get_width()/2, h + 0.01, f"{h:.2f}",
                    ha="center", va="bottom", fontsize=8)

    # Annotate the key insight
    ax.annotate("E=F: any example\nunlocks strategy",
                xy=(0 - width/4, e_recalls[0] + 0.01),
                xytext=(0.8, 0.60),
                arrowprops=dict(arrowstyle="->", color="green"),
                color="green", fontsize=8.5)
    ax.annotate("E>>F: only same-\nfamily provides\nthe knowledge",
                xy=(1 + width/4, f_recalls[1] + 0.01),
                xytext=(1.5, 0.55),
                arrowprops=dict(arrowstyle="->", color="blue"),
                color="blue", fontsize=8.5)

    plt.tight_layout()
    path = FIG_DIR / "fig2_ef_smoking_gun.pdf"
    plt.savefig(path, bbox_inches="tight", dpi=150)
    plt.close()
    print(f"Saved → {path}")


# ── Figure 3: FreeRTOS module recall ──────────────────────────────────────────

def fig3_freertos_modules():
    """Bar chart of E_claude recall by FreeRTOS module (requires freertos_recall_by_func.csv)."""
    path = DATA_DIR / "freertos_recall_by_func.csv"
    if not path.exists():
        print(f"Skipping Fig 3: {path} not found")
        return

    df = pd.read_csv(path)
    if "recall_E_claude" not in df.columns:
        print("Skipping Fig 3: recall_E_claude column missing")
        return

    # Group by module
    mod_means = df.groupby("module")["recall_E_claude"].agg(["mean", "count"]).reset_index()
    mod_means.columns = ["module", "mean_recall", "n"]
    mod_means = mod_means.sort_values("mean_recall", ascending=True)

    fig, ax = plt.subplots(figsize=(8, 5))
    colors = ["#d62728" if m < 0.05 else "#ff7f0e" if m < 0.25 else "#2ca02c"
              for m in mod_means["mean_recall"]]
    bars = ax.barh(mod_means["module"], mod_means["mean_recall"], color=colors, alpha=0.85)

    # Add n labels
    for bar, (_, row) in zip(bars, mod_means.iterrows()):
        ax.text(bar.get_width() + 0.005, bar.get_y() + bar.get_height()/2,
                f"n={row['n']}", va="center", fontsize=8)

    ax.set_xlabel("Mean postcondition recall (E_claude)", fontsize=11)
    ax.set_title("FreeRTOS-Plus-TCP: recall under same-module example condition\n"
                 "(recall driven by stub-level textual overlap)", fontsize=11)
    ax.set_xlim(0, 1.15)
    ax.axvline(0.5, color="grey", linestyle="--", alpha=0.5)

    # Color legend
    patches = [
        mpatches.Patch(color="#2ca02c", label="High sharing (≥0.25)"),
        mpatches.Patch(color="#ff7f0e", label="Partial sharing"),
        mpatches.Patch(color="#d62728", label="No sharing (<0.05)"),
    ]
    ax.legend(handles=patches, loc="lower right", fontsize=9)

    plt.tight_layout()
    path = FIG_DIR / "fig3_freertos_modules.pdf"
    plt.savefig(path, bbox_inches="tight", dpi=150)
    plt.close()
    print(f"Saved → {path}")


# ── Figure 4: Model comparison bar ───────────────────────────────────────────

def fig4_model_comparison(long_df: pd.DataFrame):
    """AIC and pseudo-R² comparison for M1–M4."""
    models = ["M1\nLLM+cond", "M2\n+typology", "M3\n+features", "M4\nfeats only"]
    aics   = [1328.4, 1196.4, 1199.0, 1306.0]
    r2s    = [0.076, 0.182, 0.181, 0.098]

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(8, 4))

    colors = ["#7f7f7f", "#2ca02c", "#1f77b4", "#ff7f0e"]

    ax1.bar(models, aics, color=colors, alpha=0.85)
    ax1.set_ylabel("AIC (lower = better)", fontsize=10)
    ax1.set_title("Model comparison: AIC", fontsize=11)
    ax1.axhline(min(aics), color="red", linestyle="--", alpha=0.5, label=f"Best={min(aics):.0f}")
    ax1.legend(fontsize=8)
    for i, (m, a) in enumerate(zip(models, aics)):
        ax1.text(i, a + 5, f"{a:.0f}", ha="center", fontsize=9)

    ax2.bar(models, r2s, color=colors, alpha=0.85)
    ax2.set_ylabel("McFadden pseudo-R²", fontsize=10)
    ax2.set_title("Model comparison: Pseudo-R²", fontsize=11)
    ax2.set_ylim(0, 0.25)
    for i, (m, r) in enumerate(zip(models, r2s)):
        ax2.text(i, r + 0.003, f"{r:.3f}", ha="center", fontsize=9)

    plt.tight_layout()
    path = FIG_DIR / "fig4_model_comparison.pdf"
    plt.savefig(path, bbox_inches="tight", dpi=150)
    plt.close()
    print(f"Saved → {path}")


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    print("Loading Phase 4 regression data …")
    long_df = load_long_df()

    print("Generating Figure 1: Category heatmap …")
    fig1_heatmap(long_df)

    print("Generating Figure 2: E vs F smoking gun …")
    fig2_ef_comparison(long_df)

    print("Generating Figure 3: FreeRTOS module recall …")
    fig3_freertos_modules()

    print("Generating Figure 4: Model comparison …")
    fig4_model_comparison(long_df)

    print(f"\nAll figures saved to {FIG_DIR}/")


if __name__ == "__main__":
    main()
