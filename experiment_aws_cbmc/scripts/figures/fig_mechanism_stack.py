#!/usr/bin/env python3
"""
Figure B — what happened to the assertion that should have caught the bug.

Absolute counts, not percentages, because the conditions differ by an order of
magnitude in how much they silence and a percentage hides that. The dead
scaffolds are split by cause (a loop outrunning the proof's bound, or an
assumption set with no feasible execution) since the two call for different
fixes. Replaces the Dead/NW/Del/Nar columns of the oracle table.

  python3 scripts/figures/fig_mechanism_stack.py
"""
import collections
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import style as S  # noqa: E402
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.patches import Patch  # noqa: E402

ORDER = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
         "G_gptoss120b", "A_claude", "H_claude", "M_claude"]
KEYS = ["dead_loop", "dead_assume", "never", "narrowed", "deleted", "unresolved"]

def counts():
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    dead = S.dead_groups()
    labels = S.load("adjudicated_mechanism.json")["labels"]
    cause = {(r["cond"], r["func"]): r["cause"]
             for r in S.load("vacuity_cause_640.json")["rows"]}
    lab2key = {"NW": "never", "Nar": "narrowed", "Del": "deleted", "Unres": "unresolved"}

    out = {}
    for cond in ORDER:
        c = collections.Counter()
        v = llm.get(cond, {})
        for (f, m) in canon:
            if v.get((f, m)) != "SUCCESS":
                continue
            if (cond, f) in dead:
                why = cause.get((cond, f), "unwind-truncation")
                c["dead_loop" if why == "unwind-truncation" else "dead_assume"] += 1
            else:
                c[lab2key.get(labels.get(cond, {}).get(f, "Unres"), "unresolved")] += 1
        out[cond] = c
    return out

def main():
    S.setup()
    data = counts()
    rows = ORDER + ["POOLED"]
    pooled = collections.Counter()
    for c in ORDER:
        pooled.update(data[c])
    data["POOLED"] = pooled

    colours = {"dead_loop": S.MECH["dead"], "dead_assume": "#8A8A8A",
               "never": S.MECH["never"], "narrowed": S.MECH["narrowed"],
               "deleted": S.MECH["deleted"], "unresolved": S.MECH["unresolved"]}
    hatch = {"dead_assume": "//", "unresolved": ".."}

    fig, ax = plt.subplots(figsize=(S.TEXTWIDTH, 2.75))
    ypos = list(range(len(rows)))[::-1]
    for y, cond in zip(ypos, rows):
        left = 0
        for k in KEYS:
            n = data[cond].get(k, 0)
            if not n:
                continue
            ax.barh(y, n, left=left, height=0.62, color=colours[k],
                    hatch=hatch.get(k), edgecolor="white", linewidth=0.6)
            if n >= 8:
                ax.text(left + n / 2, y, str(n), ha="center", va="center",
                        fontsize=6.5, color="white" if k.startswith("dead") else "#222222")
            left += n
        ax.text(left + 3, y, str(left), va="center", fontsize=6.5, color="#444444")

    labels = [f"{S.COND_LABEL[c]} / {S.model_of(c)}" if c != "POOLED" else "all eight"
              for c in rows]
    ax.set_yticks(ypos, labels)
    ax.axhline(0.5, color="#cccccc", lw=0.8)
    ax.set_xlabel("silenced bugs (count)")
    ax.set_xlim(0, max(sum(data[c].values()) for c in rows) * 1.12)
    ax.grid(axis="y", visible=False)

    legend = [Patch(facecolor=colours["dead_loop"], label="dead: loop outruns the bound"),
              Patch(facecolor=colours["dead_assume"], hatch="//",
                    label="dead: assumptions admit no run"),
              Patch(facecolor=colours["never"], label="never written"),
              Patch(facecolor=colours["narrowed"], label="narrowed away"),
              Patch(facecolor=colours["deleted"], label="deleted to pass"),
              Patch(facecolor=colours["unresolved"], hatch="..", label="unresolved")]
    ax.legend(handles=legend, loc="lower center", bbox_to_anchor=(0.5, 1.01), ncol=3,
              frameon=False, handlelength=1.1, columnspacing=1.2, borderaxespad=0.0)
    S.save(fig, "fig_mechanism_stack")

if __name__ == "__main__":
    main()
