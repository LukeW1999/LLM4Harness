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

# The eight oracle conditions the layer analysis covers. SpecFirst is excluded
# here as it is everywhere else: it has no GT-guided strengthening run, so its
# live silences would draw as an unlabelled residual and contradict the claim
# that every silence lands in exactly one layer.
ORDER = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
         "G_gptoss120b", "A_claude", "H_claude", "M_claude"]
# panel (a) classifies every GT-fail mutant, so its "silenced" spans both the dead
# and the live ones; it needs colours that cannot be read as mechanism swatches.
DISP = {"caught": "#A8DDB5", "silenced": "#762A83", "unresolved": "#DDDDDD"}
# Four layers, in the order a harness fails them. Every one is decided by CBMC:
# the probe for the first, and the GT-guided strengthening run for the rest.
KEYS = ["dead_loop", "dead_assume", "illegal", "unreachable", "missing", "untested"]

def counts():
    import glob, json
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    full = {(r["cond"], r["func"]): r
            for r in S.load("reachability_full_640.json")["rows"]}
    dead = {k for k, r in full.items() if r.get("head") != "FAIL"}
    dead |= {k for k in S.dead_groups() if k not in full}
    cause = {(r["cond"], r["func"]): r["cause"]
             for r in S.load("vacuity_cause_640.json")["rows"]}
    runs = {}
    for path in glob.glob(str(S.EXP / "evaluation/b2_repair_gt_*.json")):
        cond = path.rsplit("b2_repair_gt_", 1)[1][:-5]
        for r in json.load(open(path)):
            runs[(cond, r["func"])] = r

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
                continue
            r = runs.get((cond, f))
            if r is None:
                c["untested"] += 1
            elif r.get("orig_after") not in ("SUCCESS", "UNKNOWN"):
                c["illegal"] += 1
            elif r["n_caught"]:
                c["missing"] += 1
            else:
                c["unreachable"] += 1
        out[cond] = c
    return out

def disposition():
    """Each condition splits the GT-fail set three ways: caught, silenced, unresolved."""
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    out = {}
    for cond in ORDER:
        v = llm.get(cond, {})
        c = s_ = u = 0
        for k in canon:
            x = v.get(k)
            if x in ("FAIL", "SAT"):
                c += 1
            elif x == "SUCCESS":
                s_ += 1
            else:
                u += 1
        n = len(canon)
        out[cond] = (100 * c / n, 100 * s_ / n, 100 * u / n)
    return out

def main():
    S.setup()
    data = counts()
    disp = disposition()
    rows = list(ORDER)   # the pooled totals are quoted in the prose, not drawn

    colours = {"dead_loop": S.MECH["dead"], "dead_assume": "#8A8A8A",
               "illegal": S.MECH["narrowed"], "unreachable": S.MECH["deleted"],
               "missing": S.MECH["never"], "untested": S.MECH["unresolved"]}
    hatch = {"dead_assume": "//", "untested": ".."}

    fig, (axd, ax) = plt.subplots(1, 2, figsize=(S.TEXTWIDTH, 1.68),
                                  gridspec_kw={"width_ratios": [1.0, 1.35]}, sharey=True)
    ypos = list(range(len(rows)))[::-1]
    for y, cond in zip(ypos, rows):
        left = 0
        for k in KEYS:
            n = data[cond].get(k, 0)
            if not n:
                continue
            ax.barh(y, n, left=left, height=0.68, color=colours[k],
                    hatch=hatch.get(k), edgecolor="white", linewidth=0.6)
            if n >= 8:
                ax.text(left + n / 2, y, str(n), ha="center", va="center",
                        fontsize=6.5, color="white" if k.startswith("dead") else "#222222")
            left += n
        ax.text(left + 3, y, str(left), va="center", fontsize=6.5, color="#444444")

    for y, cond in zip(ypos, rows):
        caught, sil, unres = disp[cond]
        left = 0
        for val, col, hat in ((caught, DISP["caught"], None), (sil, DISP["silenced"], None),
                              (unres, DISP["unresolved"], "..")):
            axd.barh(y, val, left=left, height=0.68, color=col, hatch=hat,
                     edgecolor="white", linewidth=0.6)
            left += val
        axd.text(disp[cond][0] / 2, y, f"{caught:.0f}", ha="center", va="center",
                 fontsize=6.2, color="#0d3b2e")
        axd.text(disp[cond][0] + sil / 2, y, f"{sil:.0f}", ha="center", va="center",
                 fontsize=6.2, color="white")
    axd.set_xlim(0, 100)
    axd.set_xlabel("(a) the 397 GT-fail mutants, unwinding check off (%)")
    axd.grid(axis="y", visible=False)

    labels = [f"{S.COND_LABEL[c]} / {S.model_of(c)}" for c in rows]
    axd.set_yticks(ypos, labels)
    ax.set_xlabel("(b) mechanism behind the silences")
    ax.set_xlim(0, max(sum(data[c].values()) for c in rows) * 1.12)
    ax.grid(axis="y", visible=False)

    legend_a = [Patch(facecolor=DISP["caught"], label="caught"),
                Patch(facecolor=DISP["silenced"], label="silenced"),
                Patch(facecolor=DISP["unresolved"], hatch="..", label="unresolved")]
    legend = [Patch(facecolor=colours["dead_loop"], label="never ran: loop $>$ bound"),
              Patch(facecolor=colours["dead_assume"], hatch="//",
                    label="never ran: no feasible run"),
              Patch(facecolor=colours["illegal"], label="setup admits illegal states"),
              Patch(facecolor=colours["unreachable"], label="setup misses the fault"),
              Patch(facecolor=colours["missing"], label="assertion absent")]
    axd.legend(handles=legend_a, loc="lower left", bbox_to_anchor=(-0.02, 1.0), ncol=3,
               frameon=False, handlelength=1.0, columnspacing=0.8, fontsize=6.0)
    ax.legend(handles=legend, loc="lower left", bbox_to_anchor=(-0.02, 1.0), ncol=2,
              frameon=False, handlelength=1.0, columnspacing=0.8, fontsize=6.0)
    S.save(fig, "fig_mechanism_stack")

if __name__ == "__main__":
    main()
