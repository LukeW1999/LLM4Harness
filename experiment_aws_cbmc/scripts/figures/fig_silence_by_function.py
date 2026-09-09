#!/usr/bin/env python3
"""
Figure D — where the silences sit.

One cell per (condition, function): how many of that function's mutants the
condition silences. The point is the contrast between the two corpora. On
aws-c-common the gpt-oss silences pile into one function, so a reader is right
to ask whether the finding is one function's story; on s2n-tls the same
conditions spread across five to nine, which is where the cross-corpus claim
gets its dispersion.

  python3 scripts/figures/fig_silence_by_function.py
"""
import collections
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import style as S  # noqa: E402
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np  # noqa: E402

AWS = ["Oracle_gptoss120b", "K_gptoss120b", "A_gptoss120b", "H_gptoss120b",
       "M_gptoss120b", "A_claude", "H_claude", "M_claude"]

def aws_matrix():
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    cells = collections.Counter()
    for cond in AWS:
        v = llm.get(cond, {})
        for (f, m) in canon:
            if v.get((f, m)) == "SUCCESS":
                cells[(cond, f)] += 1
    return cells, AWS

def s2n_matrix():
    rows = S.load("s2n_640.json")["rows"]
    cells = collections.Counter()
    for r in rows:
        if r["gt"] == "FAIL" and r["llm"] == "SUCCESS":
            cells[(r["cond"], r["func"])] += 1
    conds = sorted({c for c, _f in cells})
    return cells, conds

def short(func):
    return func.replace("aws_", "").replace("s2n_stuffer_", "")

def panel(ax, cells, conds, title):
    funcs = sorted({f for _c, f in cells}, key=lambda f: -sum(
        n for (c, ff), n in cells.items() if ff == f))
    M = np.array([[cells.get((c, f), 0) for f in funcs] for c in conds], dtype=float)
    M[M == 0] = np.nan
    ax.imshow(M, aspect="auto", cmap="YlOrBr", vmin=0, vmax=max(1, np.nanmax(M)))
    ax.set_xticks(range(len(funcs)), [short(f) for f in funcs], rotation=60,
                  ha="right", fontsize=5.2)
    ax.set_yticks(range(len(conds)),
                  [f"{S.COND_LABEL.get(c, c)} / {S.model_of(c)}" for c in conds], fontsize=6.4)
    for i in range(len(conds)):
        for j in range(len(funcs)):
            if not np.isnan(M[i, j]):
                ax.text(j, i, int(M[i, j]), ha="center", va="center", fontsize=5.4,
                        color="white" if M[i, j] > 0.6 * np.nanmax(M) else "#333333")
    ax.set_title(title, loc="left", fontsize=7.5)
    ax.grid(False)

def main():
    S.setup()
    aws_cells, aws_conds = aws_matrix()
    s2n_cells, s2n_conds = s2n_matrix()
    fig, (ax1, ax2) = plt.subplots(
        2, 1, figsize=(S.TEXTWIDTH, 2.9),
        gridspec_kw={"height_ratios": [len(aws_conds) + 3, len(s2n_conds) + 3]})
    panel(ax1, aws_cells, aws_conds, "(a) aws-c-common")
    panel(ax2, s2n_cells, s2n_conds, "(b) s2n-tls")
    fig.tight_layout(h_pad=1.2)
    S.save(fig, "fig_silence_by_function")

if __name__ == "__main__":
    main()
