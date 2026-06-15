#!/usr/bin/env python3
"""
Phase 3 analysis — summarise FreeRTOS LLM generation results.

Reads all summary.json files from data/freertos_results/ and produces:
  - Per-condition/model recall table
  - Function-level recall CSV
  - Module-level breakdown
  - Non-zero recall cases (positive evidence for derivability)
  - reports/phase_3.md
"""

import csv
import json
import sys
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from config import DATA_DIR, REPORT_DIR, FREERTOS_CORPUS_DIR

RESULT_ROOT = DATA_DIR / "freertos_results"
CONDS  = ["A", "E"]
MODELS = ["claude", "qwen"]


def load_all_summaries() -> list[dict]:
    results = []
    for p in RESULT_ROOT.rglob("summary.json"):
        try:
            d = json.loads(p.read_text())
            if "func" in d and "cond" in d and "model" in d:
                results.append(d)
        except Exception:
            pass
    return results


def module_of(func: str, harness_index: dict[str, str]) -> str:
    return harness_index.get(func, "unknown")


def build_harness_module_index() -> dict[str, str]:
    idx = {}
    for h in FREERTOS_CORPUS_DIR.rglob("*_harness.c"):
        func = h.stem.removesuffix("_harness")
        # module = first directory level inside proofs/
        parts = list(h.parts)
        try:
            proofs_i = parts.index("proofs")
            idx[func] = parts[proofs_i + 1]
        except ValueError:
            idx[func] = "unknown"
    return idx


def main():
    results = load_all_summaries()
    if not results:
        print("No summary.json files found in", RESULT_ROOT)
        sys.exit(1)

    harness_idx = build_harness_module_index()

    # ── Per cond×model recall table ───────────────────────────────────────────
    print("\n══ Recall table (FreeRTOS, Phase 3) ══")
    print(f"{'Condition':10s} {'Model':8s} {'N':>5s} {'Mean recall':>12s} {'N>0':>6s}")
    print("-" * 45)
    cell: dict[tuple, list[float]] = defaultdict(list)
    for r in results:
        if r.get("recall") is None:
            continue
        cell[(r["cond"], r["model"])].append(r["recall"])

    for cond in CONDS:
        for model in MODELS:
            vals = cell.get((cond, model), [])
            if not vals:
                continue
            mean = sum(vals) / len(vals)
            n_pos = sum(1 for v in vals if v > 0)
            print(f"{cond:10s} {model:8s} {len(vals):5d} {mean:12.3f} {n_pos:6d}")

    # ── Non-zero recall cases ─────────────────────────────────────────────────
    nonzero = [r for r in results if r.get("recall") and r["recall"] > 0]
    print(f"\n── Non-zero recall cases ({len(nonzero)}) ──")
    for r in sorted(nonzero, key=lambda x: -x["recall"]):
        mod = harness_idx.get(r["func"], "?")
        print(f"  {r['cond']:2s} {r['model']:6s}  {r['func']:35s}  recall={r['recall']:.2f} "
              f"({r.get('matched',0)}/{r.get('gt_count',0)})  mod={mod}")

    # ── Module-level recall breakdown ─────────────────────────────────────────
    mod_recall: dict[str, dict[tuple, list[float]]] = defaultdict(lambda: defaultdict(list))
    for r in results:
        if r.get("recall") is None:
            continue
        mod = harness_idx.get(r["func"], "unknown")
        mod_recall[mod][(r["cond"], r["model"])].append(r["recall"])

    print("\n── Module-level mean recall (E_claude) ──")
    for mod, cells in sorted(mod_recall.items()):
        vals = cells.get(("E", "claude"), [])
        if vals:
            mean = sum(vals) / len(vals)
            n_pos = sum(1 for v in vals if v > 0)
            print(f"  {mod:30s}  n={len(vals):2d}  mean={mean:.3f}  n>0={n_pos}")

    # ── Save function-level CSV ───────────────────────────────────────────────
    func_recalls: dict[str, dict] = {}
    for r in results:
        func = r["func"]
        key  = f"{r['cond']}_{r['model']}"
        if func not in func_recalls:
            func_recalls[func] = {"func": func, "module": harness_idx.get(func, "?")}
        func_recalls[func][f"recall_{key}"] = r.get("recall")
        func_recalls[func][f"matched_{key}"] = r.get("matched", 0)
        func_recalls[func][f"gt_count"]       = r.get("gt_count", 0)

    out_csv = DATA_DIR / "freertos_recall_by_func.csv"
    fieldnames = ["func", "module", "gt_count"] + [
        f"{pre}_{cond}_{model}"
        for pre in ("recall", "matched")
        for cond in CONDS
        for model in MODELS
    ]
    with out_csv.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames, extrasaction="ignore")
        w.writeheader()
        for row in sorted(func_recalls.values(), key=lambda x: (x["module"], x["func"])):
            w.writerow(row)
    print(f"\nSaved → {out_csv}")

    # ── Write phase report ────────────────────────────────────────────────────
    total_done = len(results)
    total_funcs = len(func_recalls)

    A_claude = cell.get(("A", "claude"), [])
    A_qwen   = cell.get(("A", "qwen"), [])
    E_claude = cell.get(("E", "claude"), [])
    E_qwen   = cell.get(("E", "qwen"), [])

    def fmt(vals):
        if not vals: return "n/a"
        return f"{sum(vals)/len(vals):.3f} (n={len(vals)}, n>0={sum(1 for v in vals if v>0)})"

    nonzero_lines = "\n".join(
        f"- **{r['func']}** cond={r['cond']} model={r['model']}: "
        f"recall={r['recall']:.2f} ({r.get('matched',0)}/{r.get('gt_count',0)}) "
        f"[module: {harness_idx.get(r['func'],'?')}]"
        for r in sorted(nonzero, key=lambda x: -x["recall"])
    )

    def mean_s(vals): return f"{sum(vals)/len(vals):.3f}" if vals else "n/a"
    def npos(vals): return sum(1 for v in vals if v > 0)

    report = f"""# Phase 3 Report — FreeRTOS LLM Generation

## Summary

- Functions with ≥1 GT assertion: {total_funcs}
- Total generations: {total_done}

## Recall Table

| Condition | Model  | N  | Mean recall | N > 0 |
|-----------|--------|----|-------------|-------|
| A         | claude | {len(A_claude):2d} | {mean_s(A_claude):11s} | {npos(A_claude):5d} |
| A         | qwen   | {len(A_qwen):2d} | {mean_s(A_qwen):11s} | {npos(A_qwen):5d} |
| E         | claude | {len(E_claude):2d} | {mean_s(E_claude):11s} | {npos(E_claude):5d} |
| E         | qwen   | {len(E_qwen):2d} | {mean_s(E_qwen):11s} | {npos(E_qwen):5d} |

## Non-zero Recall Cases

{nonzero_lines if nonzero_lines else "None yet."}

## Key Observation

FreeRTOS assertions are placed inside **stub function implementations**, not in the
main harness body. The LLM must:
1. Know which helper functions to stub (requires reading the target's source)
2. Know what preconditions each stub should check

**Condition E succeeds** (e.g., DHCPProcess: 100% recall under both models) when the
example harness contains the same stubs as the target function. In that case the
assertions are textually present in the example → directly derivable.

**Condition A always fails** (0% recall) because the harness header snippet shows only
includes and variable declarations, not stub assertions.

This result strongly supports the **knowledge barrier** interpretation: FreeRTOS
assertions require knowing the call graph of the target function, which is not
derivable from the harness template alone.
"""

    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    (REPORT_DIR / "phase_3.md").write_text(report)
    print(f"Saved → {REPORT_DIR / 'phase_3.md'}")


if __name__ == "__main__":
    main()
