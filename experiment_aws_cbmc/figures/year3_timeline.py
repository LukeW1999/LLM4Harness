"""Year 3 timeline / pacing comparison for Slide 9."""
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch

MYBLUE = "#29629E"
MYGREEN = "#2E7D32"
MYRED = "#B71C1C"
MYORANGE = "#E67E22"
MYGRAY = "#646464"

# Month axis: 2026/06 = month 0, 2027/12 = month 18
# Map "YYYY/MM" → month index from 2026/06
def m(s):
    y, mo = map(int, s.split("/"))
    return (y - 2026) * 12 + (mo - 6)

fig, ax = plt.subplots(figsize=(13, 5.0))
ax.set_xlim(-0.5, 19.5)
ax.set_ylim(-0.5, 4.0)

# Three rows: Late (top), Balanced, Early (bottom) — top to bottom
# Each row contains: LLM4Harness work bar + audit work bar + submission stars
rows = {
    "Early":    {"y": 0.5,
                 "llm_start": "2026/06", "llm_sub": "2027/02",
                 "audit_start": "2026/12", "audit_sub": "2027/10",
                 "llm_label": "FSE'27", "audit_label": "ICSE'28 R2"},
    "Balanced": {"y": 1.7,
                 "llm_start": "2026/06", "llm_sub": "2027/07",
                 "audit_start": "2027/05", "audit_sub": "2028/03",
                 "llm_label": "ICSE'28 R1", "audit_label": "ASE'28 R1"},
    "Late":     {"y": 2.9,
                 "llm_start": "2026/06", "llm_sub": "2027/08",
                 "audit_start": "2026/10", "audit_sub": "2027/07",
                 "llm_label": "ASE'27 R2", "audit_label": "ICSE'28 R1"},
}

bar_h = 0.35

for name, r in rows.items():
    y = r["y"]
    # LLM4Harness work bar
    x0, x1 = m(r["llm_start"]), m(r["llm_sub"])
    ax.add_patch(mpatches.FancyBboxPatch(
        (x0, y + 0.05), x1 - x0, bar_h,
        boxstyle="round,pad=0,rounding_size=0.08",
        facecolor=MYBLUE, alpha=0.85, edgecolor=MYBLUE, linewidth=0))
    ax.text((x0 + x1) / 2, y + 0.225, "LLM4Harness",
            ha="center", va="center", color="white",
            fontsize=9, fontweight="bold")
    # LLM4Harness submission marker
    ax.plot(x1, y + 0.225, marker="*", markersize=18, color=MYBLUE,
            markeredgecolor="white", markeredgewidth=1.5, zorder=5)
    ax.text(x1 + 0.3, y + 0.05, r["llm_label"],
            ha="left", va="top", fontsize=9, color=MYBLUE, fontweight="bold")

    # Audit work bar
    x0, x1 = m(r["audit_start"]), m(r["audit_sub"])
    ax.add_patch(mpatches.FancyBboxPatch(
        (x0, y - 0.50), x1 - x0, bar_h,
        boxstyle="round,pad=0,rounding_size=0.08",
        facecolor=MYGREEN, alpha=0.85, edgecolor=MYGREEN, linewidth=0))
    ax.text((x0 + x1) / 2, y - 0.325, "Year 3 audit",
            ha="center", va="center", color="white",
            fontsize=9, fontweight="bold")
    ax.plot(x1, y - 0.325, marker="*", markersize=18, color=MYGREEN,
            markeredgecolor="white", markeredgewidth=1.5, zorder=5)
    ax.text(x1 + 0.3, y - 0.5, r["audit_label"],
            ha="left", va="top", fontsize=9, color=MYGREEN, fontweight="bold")

    # Row label
    ax.text(-0.3, y - 0.15, name, ha="right", va="center",
            fontsize=12, fontweight="bold", color=MYGRAY)

# Thesis deadline (vertical line)
thesis_x = m("2027/12")
ax.axvline(thesis_x, color=MYRED, linestyle="--", linewidth=2, alpha=0.7, zorder=1)
ax.text(thesis_x, 3.7, "Thesis\n2027/12", ha="center", va="bottom",
        fontsize=10, color=MYRED, fontweight="bold")

# X axis: months
month_ticks = [0, 3, 6, 9, 12, 15, 18]
month_labels = ["2026/06", "2026/09", "2026/12", "2027/03", "2027/06", "2027/09", "2027/12"]
ax.set_xticks(month_ticks)
ax.set_xticklabels(month_labels, fontsize=9)
ax.set_yticks([])
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.spines["left"].set_visible(False)
ax.tick_params(axis="x", which="both", length=4)
ax.grid(axis="x", linestyle=":", alpha=0.3, zorder=0)

# Legend
star_blue = plt.Line2D([], [], marker="*", color=MYBLUE, linestyle="None",
                       markersize=14, markeredgecolor="white", markeredgewidth=1.2,
                       label="LLM4Harness submission")
star_green = plt.Line2D([], [], marker="*", color=MYGREEN, linestyle="None",
                        markersize=14, markeredgecolor="white", markeredgewidth=1.2,
                        label="Year 3 audit submission")
deadline = plt.Line2D([], [], color=MYRED, linestyle="--", linewidth=2,
                      label="Thesis deadline")
ax.legend(handles=[star_blue, star_green, deadline],
          loc="upper left", bbox_to_anchor=(0.0, -0.05), ncol=3,
          frameon=False, fontsize=10)

plt.tight_layout()
plt.savefig("/home/weiqi/Verification/LLM4Harness/experiment_aws_cbmc/figures/year3_timeline.png",
            dpi=200, bbox_inches="tight", facecolor="white")
print("Saved year3_timeline.png")
