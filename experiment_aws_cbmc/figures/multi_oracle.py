"""Multi-oracle architecture diagram for Year 3 plan slide."""
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch

MYBLUE = "#29629E"
MYGREEN = "#2E7D32"
MYRED = "#B71C1C"
MYORANGE = "#E67E22"
MYGRAY = "#646464"

fig, ax = plt.subplots(figsize=(11, 5.5))
ax.set_xlim(0, 11)
ax.set_ylim(0, 5.5)
ax.axis("off")


def box(x, y, w, h, label, fill, edge, fontsize=11, fontweight="normal", textcolor="black"):
    p = FancyBboxPatch(
        (x, y), w, h,
        boxstyle="round,pad=0.04,rounding_size=0.15",
        linewidth=1.4, facecolor=fill, edgecolor=edge,
    )
    ax.add_patch(p)
    ax.text(x + w / 2, y + h / 2, label, ha="center", va="center",
            fontsize=fontsize, fontweight=fontweight, color=textcolor, wrap=True)


def arrow(x1, y1, x2, y2, color="black", lw=1.4, style="->"):
    a = FancyArrowPatch(
        (x1, y1), (x2, y2),
        arrowstyle=style, mutation_scale=14,
        linewidth=lw, color=color,
    )
    ax.add_patch(a)


# Input box (left)
box(0.2, 2.3, 2.0, 0.9,
    "LLM-generated\nfunction contract",
    fill="#E8EEF5", edge=MYBLUE, fontsize=11, fontweight="bold")

# Three oracle boxes (middle)
box(3.4, 4.1, 3.4, 0.9, "(1)  ESBMC verifier",
    fill="#E8EEF5", edge=MYBLUE, fontsize=12, fontweight="bold", textcolor=MYBLUE)
ax.text(5.1, 3.8, "soundness check", ha="center", va="top",
        fontsize=10, style="italic", color=MYGRAY)

box(3.4, 2.3, 3.4, 0.9, "(2)  Mutation testing",
    fill="#EAF3EA", edge=MYGREEN, fontsize=12, fontweight="bold", textcolor=MYGREEN)
ax.text(5.1, 2.0, "discriminative power", ha="center", va="top",
        fontsize=10, style="italic", color=MYGRAY)

box(3.4, 0.5, 3.4, 0.9, "(3)  Expert-GT match",
    fill="#F8E5E5", edge=MYRED, fontsize=12, fontweight="bold", textcolor=MYRED)
ax.text(5.1, 0.2, "completeness (extends LLM4Harness taxonomy)",
        ha="center", va="top", fontsize=10, style="italic", color=MYGRAY)

# Aggregator (right)
box(8.2, 2.3, 2.6, 0.9,
    "Multi-oracle\nquality verdict",
    fill="#FCEFD9", edge=MYORANGE, fontsize=11, fontweight="bold")

# Arrows: input → 3 oracles
arrow(2.2, 2.85, 3.4, 4.55, color=MYBLUE)
arrow(2.2, 2.75, 3.4, 2.75, color=MYGREEN)
arrow(2.2, 2.65, 3.4, 0.95, color=MYRED)

# Arrows: 3 oracles → aggregator
arrow(6.8, 4.55, 8.2, 2.85, color=MYBLUE)
arrow(6.8, 2.75, 8.2, 2.75, color=MYGREEN)
arrow(6.8, 0.95, 8.2, 2.65, color=MYRED)

# Title at top
ax.text(5.5, 5.25, "Year 3 — Multi-Oracle Spec Audit Framework",
        ha="center", va="center", fontsize=13, fontweight="bold", color=MYBLUE)

plt.tight_layout()
plt.savefig("/home/weiqi/Verification/LLM4Harness/experiment_aws_cbmc/figures/multi_oracle.png",
            dpi=200, bbox_inches="tight", facecolor="white")
print("Saved multi_oracle.png")
