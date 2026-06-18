#!/usr/bin/env python3
"""Pinned-precision RQ1: pass-rate vs mutation-recall Spearman across 8 conditions.
Reuses analyze_rq1.load_condition/analyze_condition for an identical pass-rate definition.
"""
import json, sys
from pathlib import Path
sys.path.insert(0, "scripts")
from analyze_rq1 import load_condition, analyze_condition  # noqa
from scipy.stats import spearmanr, pearsonr

EXP = Path(".")
EVAL = EXP / "evaluation"

# (label, result-dir-condition-suffix)
CONDS = [
    ("A", "A_gptoss120b_pin"),
    ("G", "G_gptoss120b_pin"),
    ("H", "H_gptoss120b_pin"),
    ("I", "I_gptoss120b_pin"),
    ("J", "J_gptoss120b_pin"),
    ("K", "K_gptoss120b_pin"),
    ("M", "M_gptoss120b_pin"),
    ("Oracle", "Oracle_gptoss120b_pin"),
]

def recall_for(cond):
    f = EVAL / f"mutation_oracle_cbmc_feedback_loop_{cond}_gptoss120b_pin.json"
    rows = json.loads(f.read_text())["results"]
    gt = sum(1 for r in rows if r["gt"] == "FAIL")
    sil = sum(1 for r in rows if r["silenced"])
    return gt, sil, 100.0 * sil / gt if gt else 0.0

rows = []
for label, cname in CONDS:
    data = load_condition(cname)
    if not data:
        print(f"!! {label}: result dir missing", file=sys.stderr); continue
    m = analyze_condition(label, data)
    gt, sil, rec = recall_for(label)
    rows.append((label, m["n"], m["pass_pct"], gt, sil, rec))

print(f"{'cond':<7}{'n':>4}{'pass%':>8}{'GT':>5}{'sil':>5}{'recall%':>9}")
for label, n, pp, gt, sil, rec in rows:
    print(f"{label:<7}{n:>4}{pp:>8.1f}{gt:>5}{sil:>5}{rec:>9.1f}")

passes = [r[2] for r in rows]
recalls = [r[5] for r in rows]
rho, p = spearmanr(passes, recalls)
pr, pp_ = pearsonr(passes, recalls)
print()
print(f"n conditions      = {len(rows)}")
print(f"Spearman rho      = {rho:.4f}   (p = {p:.4f})")
print(f"Pearson  r        = {pr:.4f}   (p = {pp_:.4f})")
print(f"pass% range       = {min(passes):.1f} .. {max(passes):.1f}")
print(f"recall% range     = {min(recalls):.1f} .. {max(recalls):.1f}")
