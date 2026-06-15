#!/usr/bin/env python3
"""
Phase 0 — Sanity check and data census.

Produces:
  data/phase0_census.json      — structured census (parsed by phase0 report)
  figures/phase0_loc_hist.png  — LoC histogram across all three corpora
  reports/phase_0.md           — Phase 0 report (auto-generated)
"""

import json
import re
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# ── import config ──────────────────────────────────────────────────────────────
sys.path.insert(0, str(Path(__file__).parent))
from config import (
    AWS_DATASET, AWS_CORPUS_DIR, AWS_RESULT_DIRS, AWS_EVAL, TAXONOMY_CSV,
    S2N_DATASET, S2N_RESULT_DIRS, S2N_RECALL_FILES,
    FREERTOS_CORPUS_DIR, FREERTOS_SHORTLIST,
    DATA_DIR, FIG_DIR, REPORT_DIR,
    CROSS_VERIFY_FILES,
)

# ── helpers ────────────────────────────────────────────────────────────────────

def count_loc(path: Path) -> int:
    try:
        lines = path.read_text(errors="replace").splitlines()
        # Non-blank, non-comment-only lines
        return sum(1 for l in lines if l.strip() and not l.strip().startswith("//"))
    except Exception:
        return 0


def extract_helper_calls(code: str) -> set[str]:
    """Return set of distinct function-call names in code (simple regex, just for census)."""
    return set(re.findall(r'\b([a-z_][a-zA-Z0-9_]*)\s*\(', code))


def load_freertos_harnesses() -> list[dict]:
    """Return list of {func, module, path, loc} for all FreeRTOS harness .c files.
    Deduplicates same-function multi-variant harnesses by keeping the first seen.
    """
    seen_funcs: set[str] = set()
    harnesses = []
    for harness_file in sorted(FREERTOS_CORPUS_DIR.rglob("*_harness.c")):
        module = harness_file.parts[
            list(harness_file.parts).index("proofs") + 1
        ]
        func_name = harness_file.stem.removesuffix("_harness")
        if func_name in seen_funcs:
            continue
        seen_funcs.add(func_name)
        harnesses.append({
            "func": func_name,
            "module": module,
            "path": str(harness_file),
            "loc": count_loc(harness_file),
        })
    return harnesses


# ── aws-c-common census ────────────────────────────────────────────────────────

def census_aws() -> dict:
    # PI used 84 functions (result dirs). Find their GT harnesses in fresh clone.
    pi_funcs = sorted(d.name for d in AWS_RESULT_DIRS[("A", "claude")].iterdir()
                      if d.is_dir()) if AWS_RESULT_DIRS[("A", "claude")].exists() else []

    # Build func_name -> harness_path from fresh clone (canonical source)
    clone_index: dict[str, Path] = {}
    for h in AWS_CORPUS_DIR.rglob("*_harness.c"):
        func = h.stem.removesuffix("_harness")
        clone_index[func] = h

    harnesses = []
    missing = []
    for func in pi_funcs:
        gt = clone_index.get(func)
        if gt is None:
            missing.append(func)
            continue
        code = gt.read_text(errors="replace")
        harnesses.append({
            "func": func,
            "path": str(gt),
            "loc": count_loc(gt),
            "helpers": len(extract_helper_calls(code)),
        })
    if missing:
        print(f"  WARNING: {len(missing)} PI functions not found in clone: {missing}")

    locs = [h["loc"] for h in harnesses]
    helper_counts = [h["helpers"] for h in harnesses]
    return {
        "n_harnesses": len(harnesses),
        "total_loc": sum(locs),
        "mean_loc": round(sum(locs) / len(locs), 1) if locs else 0,
        "median_loc": sorted(locs)[len(locs) // 2] if locs else 0,
        "mean_helpers": round(sum(helper_counts) / len(helper_counts), 1),
        "harnesses": harnesses,
    }


def census_aws_llm() -> dict:
    """Count attempts, compiles, and recall-basis entries per (cond, model) cell."""
    cells = {}
    for (cond, model), result_dir in AWS_RESULT_DIRS.items():
        if not result_dir.exists():
            cells[f"{cond}_{model}"] = {"attempted": 0, "compiled": 0, "note": "dir missing"}
            continue
        func_dirs = [d for d in result_dir.iterdir() if d.is_dir()]
        compiled = 0
        for fd in func_dirs:
            summary = fd / "summary.json"
            if summary.exists():
                try:
                    s = json.loads(summary.read_text())
                    if s.get("first_pass_compile", False):
                        compiled += 1
                except Exception:
                    pass
        cells[f"{cond}_{model}"] = {
            "attempted": len(func_dirs),
            "compiled": compiled,
        }

    # Add recall numbers from cross_verify JSONs (condA/B only have per-assertion data)
    recall_cells = {}
    for (cond, model), cv_path in CROSS_VERIFY_FILES.items():
        if not cv_path.exists():
            continue
        try:
            data = json.loads(cv_path.read_text())
            # iter_recall files have different shape
            if isinstance(data, dict) and "rows" in data:
                # iter_recall: take iter=1 rows only
                rows = [r for r in data["rows"] if r.get("iter", 1) == 1]
                recalls = [r["recall_fuzzy"] for r in rows if "recall_fuzzy" in r]
            elif isinstance(data, list) and data and "harness_recall_fuzzy" in data[0]:
                recalls = [r["harness_recall_fuzzy"] for r in data]
            elif isinstance(data, list) and data and "recall_fuzzy" in data[0]:
                recalls = [r["recall_fuzzy"] for r in data]
            else:
                recalls = []
            if recalls:
                recall_cells[f"{cond}_{model}"] = round(sum(recalls) / len(recalls), 4)
        except Exception as e:
            recall_cells[f"{cond}_{model}"] = f"parse_error: {e}"

    return {"generations": cells, "mean_recall_fuzzy": recall_cells}


# ── s2n-tls census ─────────────────────────────────────────────────────────────

def census_s2n() -> dict:
    data = json.loads(S2N_DATASET.read_text())
    harnesses = []
    for func_name, entry in data.items():
        gt_code = entry.get("gt_harness", "")
        lines = [l for l in gt_code.splitlines() if l.strip() and not l.strip().startswith("//")]
        harnesses.append({
            "func": func_name,
            "loc": len(lines),
            "helpers": len(extract_helper_calls(gt_code)),
        })

    locs = [h["loc"] for h in harnesses]
    helper_counts = [h["helpers"] for h in harnesses]
    return {
        "n_harnesses": len(harnesses),
        "total_loc": sum(locs),
        "mean_loc": round(sum(locs) / len(locs), 1) if locs else 0,
        "median_loc": sorted(locs)[len(locs) // 2] if locs else 0,
        "mean_helpers": round(sum(helper_counts) / len(helper_counts), 1),
        "harnesses": harnesses,
    }


def census_s2n_llm() -> dict:
    cells = {}
    for (cond, model), result_dir in S2N_RESULT_DIRS.items():
        if not result_dir.exists():
            cells[f"{cond}_{model}"] = {"attempted": 0, "note": "dir missing"}
            continue
        func_dirs = [d for d in result_dir.iterdir() if d.is_dir()]
        compiled = 0
        for fd in func_dirs:
            summary = fd / "summary.json"
            if summary.exists():
                try:
                    s = json.loads(summary.read_text())
                    if s.get("first_pass_compile", s.get("converged", False)):
                        compiled += 1
                except Exception:
                    pass
        cells[f"{cond}_{model}"] = {"attempted": len(func_dirs), "compiled": compiled}

    recall_cells = {}
    for (cond, model), recall_path in S2N_RECALL_FILES.items():
        if not recall_path.exists():
            continue
        try:
            rows = json.loads(recall_path.read_text())
            recalls = [r["recall_fuzzy"] for r in rows if "recall_fuzzy" in r]
            if recalls:
                recall_cells[f"{cond}_{model}"] = round(sum(recalls) / len(recalls), 4)
        except Exception as e:
            recall_cells[f"{cond}_{model}"] = f"parse_error: {e}"

    return {"generations": cells, "mean_recall_fuzzy": recall_cells}


# ── FreeRTOS census ────────────────────────────────────────────────────────────

def census_freertos() -> dict:
    harnesses = load_freertos_harnesses()
    locs = [h["loc"] for h in harnesses]
    modules = defaultdict(int)
    for h in harnesses:
        modules[h["module"]] += 1
    return {
        "n_harnesses": len(harnesses),
        "total_loc": sum(locs),
        "mean_loc": round(sum(locs) / len(locs), 1) if locs else 0,
        "median_loc": sorted(locs)[len(locs) // 2] if locs else 0,
        "modules": dict(modules),
        "harnesses": harnesses,
        "llm_runs": "none_yet",
    }


# ── LoC histogram ──────────────────────────────────────────────────────────────

def plot_loc_histogram(aws_locs, s2n_locs, freertos_locs):
    fig, ax = plt.subplots(figsize=(9, 5))
    bins = range(0, max(aws_locs + s2n_locs + freertos_locs) + 20, 10)
    ax.hist(aws_locs,      bins=bins, alpha=0.7, label=f"aws-c-common (n={len(aws_locs)})",  color="#2196F3")
    ax.hist(s2n_locs,      bins=bins, alpha=0.7, label=f"s2n-tls stuffer (n={len(s2n_locs)})", color="#4CAF50")
    ax.hist(freertos_locs, bins=bins, alpha=0.7, label=f"FreeRTOS-Plus-TCP (n={len(freertos_locs)})", color="#FF9800")
    ax.set_xlabel("Lines of code (non-blank, non-comment)")
    ax.set_ylabel("Number of harnesses")
    ax.set_title("GT Harness LoC Distribution — Phase 0 Census")
    ax.legend()
    fig.tight_layout()
    out = FIG_DIR / "phase0_loc_hist.png"
    fig.savefig(out, dpi=150)
    print(f"  saved: {out}")


# ── taxonomy check ─────────────────────────────────────────────────────────────

def census_taxonomy() -> dict:
    if not TAXONOMY_CSV.exists():
        return {"error": f"not found: {TAXONOMY_CSV}"}
    import csv
    rows = list(csv.DictReader(TAXONOMY_CSV.open()))
    cats = defaultdict(int)
    for r in rows:
        cats[r.get("category", "UNKNOWN")] += 1
    return {
        "total_rows": len(rows),
        "columns": list(rows[0].keys()) if rows else [],
        "category_counts": dict(cats),
    }


# ── main ───────────────────────────────────────────────────────────────────────

def main():
    DATA_DIR.mkdir(exist_ok=True)
    FIG_DIR.mkdir(exist_ok=True)
    REPORT_DIR.mkdir(exist_ok=True)

    print("── aws-c-common ──")
    aws = census_aws()
    print(f"  {aws['n_harnesses']} harnesses, {aws['total_loc']} total LoC, "
          f"mean {aws['mean_loc']} LoC, mean {aws['mean_helpers']} helper calls")
    aws_llm = census_aws_llm()

    print("── s2n-tls ──")
    s2n = census_s2n()
    print(f"  {s2n['n_harnesses']} harnesses, {s2n['total_loc']} total LoC, "
          f"mean {s2n['mean_loc']} LoC")
    s2n_llm = census_s2n_llm()

    print("── FreeRTOS-Plus-TCP ──")
    freertos = census_freertos()
    print(f"  {freertos['n_harnesses']} unique harnesses, {freertos['total_loc']} total LoC, "
          f"mean {freertos['mean_loc']} LoC")
    print(f"  modules: {freertos['modules']}")

    print("── Taxonomy CSV ──")
    tax = census_taxonomy()
    print(f"  {tax.get('total_rows', '?')} rows, categories: {tax.get('category_counts', {})}")

    print("── LoC histogram ──")
    aws_locs      = [h["loc"] for h in aws["harnesses"]]
    s2n_locs      = [h["loc"] for h in s2n["harnesses"]]
    freertos_locs = [h["loc"] for h in freertos["harnesses"]]
    plot_loc_histogram(aws_locs, s2n_locs, freertos_locs)

    # ── Save census JSON ───────────────────────────────────────────────────────
    census = {
        "aws_c_common":       {**{k: v for k, v in aws.items() if k != "harnesses"}, **aws_llm},
        "s2n_tls":            {**{k: v for k, v in s2n.items() if k != "harnesses"}, **s2n_llm},
        "freertos_plus_tcp":  {k: v for k, v in freertos.items() if k != "harnesses"},
        "taxonomy":           tax,
    }
    census_path = DATA_DIR / "phase0_census.json"
    census_path.write_text(json.dumps(census, indent=2))
    print(f"  saved: {census_path}")

    # ── Print LLM generation table ─────────────────────────────────────────────
    print("\n── LLM generation census (aws-c-common) ──")
    for cell, info in sorted(aws_llm["generations"].items()):
        recall = aws_llm["mean_recall_fuzzy"].get(cell, "n/a")
        print(f"  {cell:20s}  attempted={info.get('attempted','?'):3}  "
              f"compiled={info.get('compiled','?'):3}  recall={recall}")

    print("\n── LLM generation census (s2n-tls) ──")
    for cell, info in sorted(s2n_llm["generations"].items()):
        recall = s2n_llm["mean_recall_fuzzy"].get(cell, "n/a")
        print(f"  {cell:20s}  attempted={info.get('attempted','?'):3}  "
              f"compiled={info.get('compiled','?'):3}  recall={recall}")

    # ── Write phase_0 report ───────────────────────────────────────────────────
    report = build_report(aws, aws_llm, s2n, s2n_llm, freertos, tax)
    report_path = REPORT_DIR / "phase_0.md"
    report_path.write_text(report)
    print(f"\n  report: {report_path}")


def build_report(aws, aws_llm, s2n, s2n_llm, freertos, tax) -> str:
    gen_aws = aws_llm["generations"]
    rec_aws = aws_llm["mean_recall_fuzzy"]
    gen_s2n = s2n_llm["generations"]
    rec_s2n = s2n_llm["mean_recall_fuzzy"]

    def gen_row(label, info, recall):
        return (f"| {label:22s} | {info.get('attempted','?'):>9} | "
                f"{info.get('compiled','?'):>8} | {str(recall):>10} |")

    aws_rows = "\n".join(
        gen_row(k, v, rec_aws.get(k, "n/a"))
        for k, v in sorted(gen_aws.items())
    )
    s2n_rows = "\n".join(
        gen_row(k, v, rec_s2n.get(k, "n/a"))
        for k, v in sorted(gen_s2n.items())
    )

    freertos_modules = "\n".join(
        f"  - {mod}: {cnt}" for mod, cnt in sorted(freertos["modules"].items())
    )

    return f"""# Phase 0 Report

## What I did

- Sparse-cloned aws-c-common, s2n-tls, FreeRTOS-Plus-TCP into `study_derivability/corpora/`
- Counted GT harnesses and LoC for all three corpora
- Audited existing LLM result directories (aws-c-common, s2n-tls)
- Checked taxonomy CSV shape
- Generated `figures/phase0_loc_hist.png`

## Key numbers

### GT Harness Census

| Corpus             | Harnesses | Total LoC | Mean LoC | Median LoC |
|--------------------|:---------:|:---------:|:--------:|:----------:|
| aws-c-common       | {aws['n_harnesses']:>9} | {aws['total_loc']:>9} | {aws['mean_loc']:>8} | {aws['median_loc']:>10} |
| s2n-tls (stuffer)  | {s2n['n_harnesses']:>9} | {s2n['total_loc']:>9} | {s2n['mean_loc']:>8} | {s2n['median_loc']:>10} |
| FreeRTOS-Plus-TCP  | {freertos['n_harnesses']:>9} | {freertos['total_loc']:>9} | {freertos['mean_loc']:>8} | {freertos['median_loc']:>10} |

### LLM Generation Census — aws-c-common

| Cell (cond_model)     | Attempted | Compiled | Mean Recall |
|-----------------------|:---------:|:--------:|:-----------:|
{aws_rows}

### LLM Generation Census — s2n-tls

| Cell (cond_model)     | Attempted | Compiled | Mean Recall |
|-----------------------|:---------:|:--------:|:-----------:|
{s2n_rows}

### FreeRTOS LLM runs: **none yet** (Phase 3 task)

### Taxonomy CSV
- File: `experiment_aws_cbmc/annotation/annotated_missed_properties.csv`
- Rows: {tax.get('total_rows', '?')}
- Columns: {', '.join(tax.get('columns', []))}
- Category distribution: {tax.get('category_counts', {})}

### FreeRTOS modules found ({freertos['n_harnesses']} unique harnesses):
{freertos_modules}

## Interesting observations

1. **s2n per-assertion match data is missing** [likely real]: The s2n evaluation
   only has aggregate recall numbers (`s2n_recall_condA.json`), not the
   `unmatched_gt_harness` lists that aws-c-common's `cross_verify_results_*.json`
   provides. Building the per-assertion dataset for s2n will require re-running
   the matcher. This is a Phase 1 gap, not a blocker.

2. **FreeRTOS has 3 duplicate harnesses** [likely real]: `OutputARPRequest` appears
   in 3 variant directories (buffer_alloc1, buffer_alloc2, FreeRTOS_OutputARPRequest)
   and `vSocketBind` in 3 config variants. Deduplication keeps the first found.
   The PI's shortlist should specify which variant to use.

3. **FreeRTOS LoC spread is wider** [don't know yet]: Mean LoC for FreeRTOS
   looks comparable to aws-c-common, but the network-parser harnesses (DNS, TCP)
   are much larger (200–300 LoC) while ARP simple functions are tiny (<30 LoC).
   This may affect parse complexity and LLM generation quality differently.

## Questions for the PI

1. **FreeRTOS shortlist**: `freertos_shortlist.md` was not found in the workspace.
   I have enumerated all {freertos['n_harnesses']} unique FreeRTOS harnesses above.
   Please confirm the 40-function Tier A/B/C selection, especially for the
   duplicate-variant functions (OutputARPRequest, vSocketBind).

2. **s2n per-assertion data**: The existing s2n evaluation lacks per-assertion
   match detail. For Phase 1, should I re-run the assertion matcher on the
   s2n LLM outputs (existing harnesses in `experiment_s2n/results/`)?
   Or does the PI have a more detailed match file I missed?

3. **aws-c-common: 83 vs 84 harnesses**: The dataset directory has 84 `funcN_*`
   entries. The paper uses 83. Is one function excluded? If so, which one and why?
   (This matters for the assertion-level join in Phase 1.)

4. **Taxonomy join key**: The 191-row taxonomy uses normalized expression strings.
   For Phase 1's assertion-level join, should I match on normalized expression
   (fuzzy) or on (func, line_number)?

## What I propose to do next

Confirm PI answers to questions 1 and 2, then start Phase 1:
parse all GT harnesses with libclang to extract every `assert()` and
`__CPROVER_assume()` call, build `data/gt_assertions.csv`, and join with
the existing 191-row taxonomy.
"""


if __name__ == "__main__":
    main()
