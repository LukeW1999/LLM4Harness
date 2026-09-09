"""Shared look for the paper's figures.

One palette across every figure so a colour means the same thing everywhere,
sized for the acmsmall text block (\\textwidth = 395.8pt = 5.49in) and saved as
PDF with Type-42 fonts so the text stays selectable and ACM's checker is happy.
"""
import json
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402

EXP = Path(__file__).resolve().parent.parent.parent
EVAL = EXP / "evaluation"
OUT = EXP / "figures"

TEXTWIDTH = 5.49  # inches

# Colourblind-safe (Okabe-Ito). Models keep their colour across figures, and so
# do mechanisms, so a reader who learns the key once can read every panel.
MODEL = {"gpt-oss": "#0072B2", "Claude": "#E69F00", "Llama": "#009E73"}
MECH = {
    "dead":      "#4D4D4D",   # never reached a postcondition
    "never":     "#E69F00",   # written nowhere
    "narrowed":  "#56B4E9",   # assumption hid the buggy states
    "deleted":   "#D55E00",   # written, then removed
    "unresolved": "#BBBBBB",
}
MECH_LABEL = {"dead": "dead scaffold", "never": "never-written", "narrowed": "narrowed away",
              "deleted": "deleted to pass", "unresolved": "unresolved"}

def setup():
    plt.rcParams.update({
        "pdf.fonttype": 42, "ps.fonttype": 42,
        "font.size": 7.5, "axes.labelsize": 8, "axes.titlesize": 8,
        "xtick.labelsize": 7, "ytick.labelsize": 7, "legend.fontsize": 7,
        "axes.spines.top": False, "axes.spines.right": False,
        "axes.grid": True, "grid.alpha": 0.25, "grid.linewidth": 0.5,
        "figure.dpi": 200, "savefig.bbox": "tight", "savefig.pad_inches": 0.01,
    })

def load(name):
    return json.load(open(EVAL / name))

def save(fig, stem):
    OUT.mkdir(exist_ok=True)
    p = OUT / f"{stem}.pdf"
    fig.savefig(p)
    plt.close(fig)
    print(f"wrote {p}")
    return p

# ── the study's shared quantities, so no figure hard-codes them ──────────────
def gt_fail_set():
    gt = {(r["func"], r["mutant"]): r["gt640"] for r in load("gtfail_640.json")["verdicts"]}
    return {k for k, v in gt.items() if v == "FAIL"}

def llm_verdicts():
    """condition -> {(func, mutant): verdict} over every 6.4.0 sweep we have."""
    out = {}
    for f in ("silenced_640.json", "kllama_oracle_640.json", "silenced_repeats_640.json"):
        try:
            rows = load(f)["verdicts"]
        except FileNotFoundError:
            continue
        for r in rows:
            out.setdefault(r["cond"], {})[(r["func"], r["mutant"])] = r["llm640"]
    return out

def dead_groups():
    """(condition, function) pairs whose harness never reaches a postcondition."""
    out = set()
    for f in ("reachability_probe_640.json", "reachability_probe_repeats_640.json"):
        try:
            rows = load(f)["rows"]
        except FileNotFoundError:
            continue
        out |= {(r["cond"], r["func"]) for r in rows if r["probe"] == "SUCCESS"}
    return out

# Paper-facing names for the conditions.
COND_LABEL = {
    "G_gptoss120b": "Single", "A_gptoss120b": "Baseline", "H_gptoss120b": "Neutral",
    "M_gptoss120b": "Bounded", "K_gptoss120b": "SpecFirst", "Oracle_gptoss120b": "Oracle",
    "A_claude": "Baseline", "H_claude": "Neutral", "M_claude": "Bounded",
    "A_llama3370binstruct": "Baseline", "G_llama3370binstruct": "Single",
    "H_llama3370binstruct": "Neutral",
}

def model_of(cond):
    if "claude" in cond:
        return "Claude"
    if "llama" in cond:
        return "Llama"
    return "gpt-oss"
