#!/usr/bin/env python3
"""
Figure G — every run, not just the one in the table.

Left: the silenced share of each generation, so the reader can see which
orderings survive repetition. Right: the dead-scaffold share of each run, which
is what makes a single run misleading (Bounded is 0 of 30 dead in one run and
51 of 82 across its repeats).

  python3 scripts/figures/fig_run_variation.py
"""
import collections
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import style as S  # noqa: E402
import matplotlib.pyplot as plt  # noqa: E402

FAMILY = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
          "G_gptoss120b", "A_claude", "H_claude", "M_claude"]

def per_run():
    """family -> [(run label, Sil/GT %, dead share of that run's silences)]"""
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    dead = S.dead_groups()
    out = collections.defaultdict(list)
    for cond, v in llm.items():
        fam = re.sub(r"_r\d+$", "", cond)
        if fam not in FAMILY:
            continue
        sil = [(f, m) for (f, m) in canon if v.get((f, m)) == "SUCCESS"]
        if not sil:
            out[fam].append((cond, 0.0, None))
            continue
        d = sum(1 for (f, _m) in sil if (cond, f) in dead)
        out[fam].append((cond, 100.0 * len(sil) / len(canon), 100.0 * d / len(sil)))
    return out

def main():
    S.setup()
    runs = per_run()
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(S.TEXTWIDTH, 2.45), sharey=True)
    ypos = list(range(len(FAMILY)))[::-1]

    for y, fam in zip(ypos, FAMILY):
        col = S.MODEL[S.model_of(fam)]
        pts = runs.get(fam, [])
        xs = [p[1] for p in pts]
        if len(xs) > 1:
            ax1.plot([min(xs), max(xs)], [y, y], color=col, lw=0.8, alpha=0.35, zorder=1)
        for i, (_c, x, _d) in enumerate(pts):
            ax1.plot(x, y, "o" if i == 0 else "o", ms=5 if i == 0 else 3.2,
                     mfc=col if i == 0 else "white", mec=col, mew=0.9, zorder=3)
        ds = [p[2] for p in pts if p[2] is not None]
        if len(ds) > 1:
            ax2.plot([min(ds), max(ds)], [y, y], color=col, lw=0.8, alpha=0.35, zorder=1)
        for i, d in enumerate(ds):
            ax2.plot(d, y, "o", ms=5 if i == 0 else 3.2,
                     mfc=col if i == 0 else "white", mec=col, mew=0.9, zorder=3)

    ax1.set_yticks(ypos, [f"{S.COND_LABEL[f]} / {S.model_of(f)}" for f in FAMILY])
    ax1.set_xlabel("silenced share of GT-fail set (%)")
    ax1.set_title("(a) how much each run silences", loc="left")
    ax2.set_xlabel("dead scaffolds (% of the run's silences)")
    ax2.set_title("(b) how much of it never ran", loc="left")
    for ax in (ax1, ax2):
        ax.grid(axis="y", visible=False)
    ax1.plot([], [], "o", ms=5, color="#666666", label="run reported in the table")
    ax1.plot([], [], "o", ms=3.2, mfc="white", mec="#666666", label="repeat runs")
    ax1.legend(loc="upper center", bbox_to_anchor=(0.55, -0.30), ncol=2,
               frameon=False, handlelength=1.0, fontsize=6.2)
    S.save(fig, "fig_run_variation")

if __name__ == "__main__":
    main()
