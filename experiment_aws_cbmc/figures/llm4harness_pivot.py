"""LLM4Harness pivot flow diagram for Slide 6."""
import matplotlib.pyplot as plt
from matplotlib.patches import FancyBboxPatch, FancyArrowPatch

MYBLUE = "#29629E"
MYGREEN = "#2E7D32"
MYRED = "#B71C1C"
MYORANGE = "#E67E22"
MYGRAY = "#646464"

fig, ax = plt.subplots(figsize=(13, 2.0))
ax.set_xlim(0, 13)
ax.set_ylim(0, 2.0)
ax.axis("off")


def box(x, y, w, h, label, fill, edge, fontsize=10, fontweight="normal"):
    p = FancyBboxPatch(
        (x, y), w, h,
        boxstyle="round,pad=0.04,rounding_size=0.12",
        linewidth=1.3, facecolor=fill, edgecolor=edge,
    )
    ax.add_patch(p)
    ax.text(x + w / 2, y + h / 2, label, ha="center", va="center",
            fontsize=fontsize, fontweight=fontweight, wrap=True)


def arrow(x1, y1, x2, y2, color="black"):
    a = FancyArrowPatch(
        (x1, y1), (x2, y2),
        arrowstyle="->", mutation_scale=14, linewidth=1.3, color=color,
    )
    ax.add_patch(a)


# Stage 1: original plan
box(0.1, 0.5, 2.1, 1.0,
    "SpecVerify\non NASA",
    fill="#EAEAEA", edge=MYGRAY, fontsize=10)

# Pivot signal 1
box(2.6, 0.5, 2.4, 1.0,
    "BMC bound design\nneeds expert",
    fill="#FCE6CC", edge=MYORANGE, fontsize=10, fontweight="bold")

# Stage 2: redirect
box(5.4, 0.5, 2.1, 1.0,
    "AWS verified\nsubjects",
    fill="#EAEAEA", edge=MYGRAY, fontsize=10)

# Pivot signal 2
box(7.9, 0.5, 2.4, 1.0,
    "AWS NL ≯\ncode itself",
    fill="#FCE6CC", edge=MYORANGE, fontsize=10, fontweight="bold")

# Stage 3: final framing
box(10.7, 0.5, 2.2, 1.0,
    "Code-only\nspec audit",
    fill="#D6E9D6", edge=MYGREEN, fontsize=10, fontweight="bold")

# Arrows
arrow(2.2, 1.0, 2.6, 1.0, color="black")
arrow(5.0, 1.0, 5.4, 1.0, color="black")
arrow(7.5, 1.0, 7.9, 1.0, color="black")
arrow(10.3, 1.0, 10.7, 1.0, color="black")

plt.tight_layout()
plt.savefig("/home/weiqi/Verification/LLM4Harness/experiment_aws_cbmc/figures/llm4harness_pivot.png",
            dpi=200, bbox_inches="tight", facecolor="white")
print("Saved llm4harness_pivot.png")
