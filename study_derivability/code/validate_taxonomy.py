#!/usr/bin/env python3
"""
Quick validation: do the existing 191-assertion taxonomy labels predict recall
across conditions?

Input:  experiment_aws_cbmc/annotation/annotated_missed_properties.csv
        cross_verify_results_cond{A/B/C/D}_{claude/qwen}.json
        results/feedback_loop_{E/F}_claude/{func}/iter_*_harness.c

Output: data/taxonomy_recall_matrix.csv
        figures/validate_*.png
        reports/validate_taxonomy.md
"""

import csv
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
from config import (
    TAXONOMY_CSV, AWS_EVAL, AWS_RESULTS,
    DATA_DIR, FIG_DIR, REPORT_DIR,
)

# ── Conditions with assertion-level cross_verify JSON ─────────────────────────
CV_FILES = {
    "A_claude": AWS_EVAL / "cross_verify_results_condA_claude.json",
    "A_qwen":   AWS_EVAL / "cross_verify_results_condA.json",
    "B_claude": AWS_EVAL / "cross_verify_results_condB_claude.json",
    "B_qwen":   AWS_EVAL / "cross_verify_results_condB.json",
    "C_claude": AWS_EVAL / "cross_verify_results_condC_claude.json",
    "D_claude": AWS_EVAL / "cross_verify_results_condD_claude.json",
}
# E/F need harness extraction
EF_RESULT_DIRS = {
    "E_claude": AWS_RESULTS / "feedback_loop_E_claude",
    "F_claude": AWS_RESULTS / "feedback_loop_F_claude",
    "E_qwen":   AWS_RESULTS / "feedback_loop_E",
}

ALL_CONDITIONS = list(CV_FILES) + list(EF_RESULT_DIRS)


# ── Normalisation (mirrors evaluate_harness.py) ───────────────────────────────

def normalise(s: str) -> str:
    """Mirror cross_verify.py normalize_prop() for taxonomy join compatibility."""
    s = s.lower().strip()
    s = s.replace('->', '.')
    s = re.sub(r'\s+', ' ', s)
    # X_old → old_X  (cross_verify.py convention, opposite of evaluate_harness.py)
    s = re.sub(r'\b([a-z][a-z0-9]*)_old\b', r'old_\1', s)
    # _is_valid(&identifier) → _is_valid(&_arg_)
    s = re.sub(r'(?<=_is_valid\()&[a-z]\w*', '&_arg_', s)
    # Canonical equality direction
    eq = re.match(r'^(.+?)\s*==\s*(.+)$', s)
    if eq:
        l, r = eq.group(1).strip(), eq.group(2).strip()
        if l > r:
            s = f"{r} == {l}"
    return s


# ── Load taxonomy ─────────────────────────────────────────────────────────────

def load_taxonomy() -> list[dict]:
    rows = list(csv.DictReader(TAXONOMY_CSV.open()))
    for r in rows:
        r["_norm"] = normalise(r["property_text"])
    return rows


# ── Build "missed set" per function per condition (from cross_verify JSONs) ────

def load_cv_missed(cv_path: Path) -> dict[str, set[str]]:
    """Return {func: set_of_normalised_missed_expressions}."""
    data = json.loads(cv_path.read_text())
    result: dict[str, set[str]] = {}
    for item in data:
        missed = set()
        for entry in item.get("unmatched_gt_harness", []):
            # format is "expr|kind" but expr may contain || (C or-operator)
            expr = entry.rsplit("|", 1)[0].strip()
            missed.add(normalise(expr))
        result[item["func"]] = missed
    return result


# ── Extract assertions from a harness C file (regex, good enough for matching) ─

_ASSERT_RE = re.compile(
    r'\b(?:assert|__CPROVER_assert)\s*\((.+?)\)\s*;', re.DOTALL
)

def extract_asserts(c_text: str) -> set[str]:
    code = re.sub(r'//[^\n]*', '', c_text)
    code = re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)
    return {normalise(m.group(1)) for m in _ASSERT_RE.finditer(code)}


def best_iter_harness(func_dir: Path) -> str | None:
    """Return text of the last available iter_*_harness.c in func_dir."""
    files = sorted(func_dir.glob("iter_*_harness.c"))
    if not files:
        return None
    # prefer iter with highest number (best-iter heuristic)
    return files[-1].read_text(errors="replace")


def load_ef_asserts(result_dir: Path) -> dict[str, set[str]]:
    """Return {func: set_of_normalised_asserts_in_llm_harness}."""
    out: dict[str, set[str]] = {}
    if not result_dir.exists():
        return out
    for func_dir in result_dir.iterdir():
        if not func_dir.is_dir():
            continue
        text = best_iter_harness(func_dir)
        if text:
            out[func_dir.name] = extract_asserts(text)
    return out


# ── Build per-assertion recall matrix ─────────────────────────────────────────

def build_recall_matrix(taxonomy: list[dict]) -> list[dict]:
    # Load CV missed sets
    cv_missed: dict[str, dict[str, set[str]]] = {}
    for cond, path in CV_FILES.items():
        if path.exists():
            cv_missed[cond] = load_cv_missed(path)
        else:
            print(f"  WARNING: {path.name} not found")

    # Load E/F LLM assertion sets
    ef_asserts: dict[str, dict[str, set[str]]] = {}
    for cond, rdir in EF_RESULT_DIRS.items():
        ef_asserts[cond] = load_ef_asserts(rdir)

    rows = []
    for t in taxonomy:
        func = t["func"]
        norm = t["_norm"]
        row = {
            "id":          t["id"],
            "func":        func,
            "property_text": t["property_text"],
            "info_source": t["info_source"],
            "reasoning":   t["reasoning"],
            "category":    t["category"],
        }

        # CV conditions: 0 if in missed set, 1 if function covered but not missed, NA otherwise
        for cond, missed_by_func in cv_missed.items():
            if func not in missed_by_func:
                row[cond] = "NA"  # function not run under this condition
            elif norm in missed_by_func[func]:
                row[cond] = 0    # still missed
            else:
                row[cond] = 1    # recalled

        # E/F conditions: 1 if assertion found in LLM harness, 0 if not, NA if func not run
        for cond, asserts_by_func in ef_asserts.items():
            if func not in asserts_by_func:
                row[cond] = "NA"
            elif norm in asserts_by_func[func]:
                row[cond] = 1
            else:
                row[cond] = 0

        rows.append(row)
    return rows


# ── Analysis: recall rate by taxonomy dimension ────────────────────────────────

def recall_by_group(matrix: list[dict], group_col: str) -> dict[str, dict[str, float]]:
    """Return {group_val: {condition: mean_recall}} (NA excluded)."""
    groups = sorted({r[group_col] for r in matrix})
    result = {}
    for g in groups:
        g_rows = [r for r in matrix if r[group_col] == g]
        cond_recalls = {}
        for cond in ALL_CONDITIONS:
            vals = [r[cond] for r in g_rows if r.get(cond, "NA") != "NA"]
            if vals:
                cond_recalls[cond] = round(sum(vals) / len(vals), 3)
        result[g] = cond_recalls
    return result


# ── Plots ──────────────────────────────────────────────────────────────────────

COND_ORDER = ["A_claude", "B_claude", "C_claude", "D_claude",
              "E_claude", "F_claude", "A_qwen", "B_qwen", "E_qwen"]
COND_LABELS = {
    "A_claude": "A\n(claude)", "B_claude": "B\n(claude)",
    "C_claude": "C\n(claude)", "D_claude": "D\n(claude)",
    "E_claude": "E★\n(claude)", "F_claude": "F\n(claude)",
    "A_qwen":   "A\n(qwen)",  "B_qwen":   "B\n(qwen)",
    "E_qwen":   "E★\n(qwen)",
}
COND_COLORS = {
    "A_claude": "#90CAF9", "B_claude": "#90CAF9",
    "C_claude": "#90CAF9", "D_claude": "#90CAF9",
    "E_claude": "#1565C0", "F_claude": "#42A5F5",
    "A_qwen":   "#FFCC80", "B_qwen":   "#FFCC80",
    "E_qwen":   "#E65100",
}


def plot_recall_heatmap(by_group: dict, group_name: str, title: str, filename: str):
    groups = list(by_group.keys())
    conds  = [c for c in COND_ORDER if any(c in v for v in by_group.values())]

    data = np.full((len(groups), len(conds)), np.nan)
    for i, g in enumerate(groups):
        for j, c in enumerate(conds):
            if c in by_group[g]:
                data[i, j] = by_group[g][c]

    fig, ax = plt.subplots(figsize=(max(7, len(conds) * 0.9), max(3, len(groups) * 0.7 + 1.5)))
    im = ax.imshow(data, aspect="auto", cmap="RdYlGn", vmin=0, vmax=1)
    plt.colorbar(im, ax=ax, label="Recall rate")

    ax.set_xticks(range(len(conds)))
    ax.set_xticklabels([COND_LABELS.get(c, c) for c in conds], fontsize=9)
    ax.set_yticks(range(len(groups)))
    ax.set_yticklabels(groups, fontsize=10)
    ax.set_xlabel("Condition × Model")
    ax.set_ylabel(group_name)
    ax.set_title(title, pad=12)

    # annotate cells
    for i in range(len(groups)):
        for j in range(len(conds)):
            v = data[i, j]
            if not np.isnan(v):
                ax.text(j, i, f"{v:.2f}", ha="center", va="center",
                        fontsize=8, color="black" if 0.3 < v < 0.8 else "white")

    fig.tight_layout()
    out = FIG_DIR / filename
    fig.savefig(out, dpi=150)
    plt.close(fig)
    print(f"  saved: {out}")


def plot_recall_bars(by_group: dict, group_name: str, title: str, filename: str):
    """Bar chart: one group of bars per taxonomy value, one bar per condition."""
    groups = list(by_group.keys())
    conds  = [c for c in COND_ORDER if any(c in v for v in by_group.values())]
    x = np.arange(len(groups))
    width = 0.8 / len(conds)

    fig, ax = plt.subplots(figsize=(max(8, len(groups) * 1.5), 5))
    for i, cond in enumerate(conds):
        vals = [by_group[g].get(cond, np.nan) for g in groups]
        offset = (i - len(conds) / 2 + 0.5) * width
        bars = ax.bar(x + offset, vals, width, label=COND_LABELS.get(cond, cond),
                      color=COND_COLORS.get(cond, "#999"), alpha=0.9)

    ax.set_xticks(x)
    ax.set_xticklabels(groups, fontsize=10)
    ax.set_xlabel(group_name)
    ax.set_ylabel("Recall rate (among 191 missed by A_claude)")
    ax.set_title(title)
    ax.set_ylim(0, 1)
    ax.axhline(0, color="black", linewidth=0.5)
    ax.legend(fontsize=7, ncol=3)
    fig.tight_layout()
    out = FIG_DIR / filename
    fig.savefig(out, dpi=150)
    plt.close(fig)
    print(f"  saved: {out}")


# ── Coverage stats per condition ───────────────────────────────────────────────

def coverage_stats(matrix: list[dict]) -> dict[str, dict]:
    stats = {}
    for cond in ALL_CONDITIONS:
        vals = [r.get(cond, "NA") for r in matrix]
        covered = [v for v in vals if v != "NA"]
        recalled = [v for v in covered if v == 1]
        stats[cond] = {
            "covered": len(covered),
            "recalled": len(recalled),
            "still_missed": len(covered) - len(recalled),
            "recall_rate": round(len(recalled) / len(covered), 3) if covered else "NA",
        }
    return stats


# ── Main ───────────────────────────────────────────────────────────────────────

def main():
    DATA_DIR.mkdir(exist_ok=True)
    FIG_DIR.mkdir(exist_ok=True)
    REPORT_DIR.mkdir(exist_ok=True)

    print("Loading taxonomy…")
    taxonomy = load_taxonomy()
    print(f"  {len(taxonomy)} assertions from {len({r['func'] for r in taxonomy})} functions")

    print("Building per-assertion recall matrix…")
    matrix = build_recall_matrix(taxonomy)

    # Save matrix CSV
    matrix_path = DATA_DIR / "taxonomy_recall_matrix.csv"
    fieldnames = list(matrix[0].keys())
    with matrix_path.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames)
        w.writeheader()
        w.writerows(matrix)
    print(f"  saved: {matrix_path}")

    # Coverage per condition
    print("\nCoverage per condition (of 191 missed-by-A_claude assertions):")
    cov = coverage_stats(matrix)
    for cond, s in cov.items():
        print(f"  {cond:12s}  covered={s['covered']:3}  recalled={s['recalled']:3}  "
              f"still_missed={s['still_missed']:3}  rate={s['recall_rate']}")

    # Recall by dimension
    dims = {
        "info_source": ("Information source (NL/CODE/DOMAIN)",
                        "validate_heatmap_info_source.png",
                        "validate_bars_info_source.png"),
        "reasoning":   ("Reasoning complexity",
                        "validate_heatmap_reasoning.png",
                        "validate_bars_reasoning.png"),
        "category":    ("Semantic category",
                        "validate_heatmap_category.png",
                        "validate_bars_category.png"),
    }

    by_dim: dict[str, dict] = {}
    for col, (title, hmap_file, bar_file) in dims.items():
        print(f"\nRecall by {col}:")
        by_group = recall_by_group(matrix, col)
        by_dim[col] = by_group
        for g, cond_map in sorted(by_group.items()):
            print(f"  {g:25s}  " + "  ".join(f"{c}={v:.2f}" for c, v in cond_map.items()))
        plot_recall_heatmap(by_group, col, title, hmap_file)
        plot_recall_bars(by_group, col, title, bar_file)

    # Key table: does info_source differentiate across conditions?
    print("\n── Key test: DOMAIN vs CODE vs NL recall by condition ──")
    for cond in COND_ORDER:
        row_parts = []
        for src in ["NL", "CODE", "DOMAIN"]:
            v = by_dim["info_source"].get(src, {}).get(cond, "NA")
            row_parts.append(f"{src}={v}")
        print(f"  {cond:12s}: {' | '.join(row_parts)}")

    # Write report
    report = build_report(taxonomy, matrix, cov, by_dim)
    rpath = REPORT_DIR / "validate_taxonomy.md"
    rpath.write_text(report)
    print(f"\n  report: {rpath}")


def build_report(taxonomy, matrix, cov, by_dim) -> str:
    n_funcs = len({r["func"] for r in taxonomy})

    # Coverage table
    cov_rows = "\n".join(
        f"| {cond:12s} | {s['covered']:>7} | {s['recalled']:>8} | "
        f"{s['still_missed']:>12} | {str(s['recall_rate']):>11} |"
        for cond, s in cov.items()
    )

    # info_source table
    src_rows = ""
    for src in ["NL", "CODE", "DOMAIN"]:
        vals = by_dim["info_source"].get(src, {})
        cells = " | ".join(f"{vals.get(c, 'NA')}" for c in COND_ORDER if c in vals or True)
        src_rows += f"| {src:6s} | {cells} |\n"

    # reasoning table
    reas_rows = ""
    for reas in ["DIRECT", "CONDITIONAL", "EXHAUSTIVE", "STRUCTURAL"]:
        vals = by_dim["reasoning"].get(reas, {})
        cells = " | ".join(f"{vals.get(c, 'NA')}" for c in COND_ORDER if True)
        reas_rows += f"| {reas:12s} | {cells} |\n"

    # Key observations
    info_src = by_dim["info_source"]
    reas_src = by_dim["reasoning"]

    def delta(group_dict, key_a, key_b, cond):
        a = group_dict.get(key_a, {}).get(cond, None)
        b = group_dict.get(key_b, {}).get(cond, None)
        if isinstance(a, float) and isinstance(b, float):
            return round(b - a, 3)
        return "NA"

    domain_vs_nl_E  = delta(info_src, "NL", "DOMAIN", "E_claude")
    code_vs_nl_E    = delta(info_src, "NL", "CODE",   "E_claude")
    exhaustive_E    = reas_src.get("EXHAUSTIVE", {}).get("E_claude", "NA")
    structural_E    = reas_src.get("STRUCTURAL", {}).get("E_claude", "NA")
    direct_E        = reas_src.get("DIRECT",     {}).get("E_claude", "NA")

    return f"""# Taxonomy Validation Report

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
{cov_rows}

*(Covered = assertion's function was attempted under this condition)*

## Recall by info_source

| Source | A_claude | B_claude | C_claude | D_claude | E_claude | F_claude | A_qwen | B_qwen | E_qwen |
|--------|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:------:|:------:|:------:|
{_table_rows(by_dim['info_source'], ["NL","CODE","DOMAIN"])}

## Recall by reasoning complexity

| Reasoning    | A_claude | B_claude | C_claude | D_claude | E_claude | F_claude | A_qwen | B_qwen | E_qwen |
|--------------|:--------:|:--------:|:--------:|:--------:|:--------:|:--------:|:------:|:------:|:------:|
{_table_rows(by_dim['reasoning'], ["DIRECT","CONDITIONAL","EXHAUSTIVE","STRUCTURAL"])}

## Recall by semantic category

| Category      | A_claude | E_claude | F_claude | A_qwen | E_qwen |
|---------------|:--------:|:--------:|:--------:|:------:|:------:|
{_table_rows(by_dim['category'], sorted(by_dim['category']), short=True)}

## Key observations

- **DOMAIN vs NL, condE_claude**: delta = {domain_vs_nl_E} (negative = DOMAIN harder)
- **CODE vs NL, condE_claude**: delta = {code_vs_nl_E}
- **EXHAUSTIVE reasoning, condE_claude**: recall = {exhaustive_E}
- **STRUCTURAL reasoning, condE_claude**: recall = {structural_E}
- **DIRECT reasoning, condE_claude**: recall = {direct_E}

*(See figures/validate_heatmap_*.png and figures/validate_bars_*.png)*

## Interpretation guide

- If DOMAIN recall < CODE recall < NL recall across all conditions → supports Derivability Hypothesis
- If EXHAUSTIVE (frame conditions) recall stays near 0 even under E → supports "strategy gap" sub-claim
- If recall differences between conditions (A vs E) are SIMILAR across info_source groups →
  few-shot example helps uniformly (not selectively for derivable assertions) → weaker support
- If recall differences are LARGER for DOMAIN under E → few-shot example helps specifically
  because it demonstrates the predicate vocabulary → stronger support
"""


def _table_rows(by_group, keys, short=False) -> str:
    conds = COND_ORDER if not short else ["A_claude", "E_claude", "F_claude", "A_qwen", "E_qwen"]
    lines = []
    for k in keys:
        vals = by_group.get(k, {})
        cells = " | ".join(str(vals.get(c, "—")) for c in conds)
        lines.append(f"| {k:14s} | {cells} |")
    return "\n".join(lines)


if __name__ == "__main__":
    main()
