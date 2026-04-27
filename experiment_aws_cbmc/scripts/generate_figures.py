#!/usr/bin/env python3
"""
generate_figures.py
===================
Generate publication-quality figures for the EMSE journal paper.

Figures:
  1. conditions_bar.pdf      — Recall by condition A-F (Claude+Qwen), null result + E/F
  2. crossmodel_heatmap.pdf  — Per-family recall: Claude vs Qwen, A vs E
  3. taxonomy_pie.pdf        — Distribution of missed property categories
  4. iter1_vs_best.pdf       — Iter-1 vs best-iter for A vs E (both models)
  5. replication.pdf         — Cross-library A vs E replication (aws-c-common + s2n-tls)

Usage:
    python generate_figures.py              # all figures
    python generate_figures.py --fig 1      # specific figure
"""
import sys
import json
import argparse
from pathlib import Path
from collections import defaultdict, Counter
import numpy as np

try:
    import matplotlib
    matplotlib.use("Agg")
    import matplotlib.pyplot as plt
    import matplotlib.patches as mpatches
    MATPLOTLIB_OK = True
except ImportError:
    MATPLOTLIB_OK = False
    print("WARNING: matplotlib not available, cannot generate figures")

SCRIPT_DIR = Path(__file__).parent
EVAL_DIR = SCRIPT_DIR.parent / "evaluation"
S2N_EVAL_DIR = SCRIPT_DIR.parent.parent / "experiment_s2n" / "evaluation"
ANNOT_DIR = SCRIPT_DIR.parent / "annotation"
FIG_DIR = SCRIPT_DIR.parent / "figures"
FIG_DIR.mkdir(exist_ok=True)

ANCHOR_FUNCS = {
    'aws_array_list_init_static', 'aws_linked_list_push_back', 'aws_byte_buf_init',
    'aws_byte_cursor_from_array', 'aws_add_size_checked', 'aws_string_new_from_c_str',
    'aws_ring_buffer_init',
}

FAMILIES = ['array_list', 'linked_list', 'byte_buf', 'byte_cursor', 'math', 'string', 'ring_buffer']
FAMILY_LABELS = {
    'array_list': 'array\\_list', 'linked_list': 'linked\\_list',
    'byte_buf': 'byte\\_buf', 'byte_cursor': 'byte\\_cursor',
    'math': 'math', 'string': 'string', 'ring_buffer': 'ring\\_buffer',
}

def family(fn):
    for fam in FAMILIES:
        if fam in fn:
            return fam
    return 'math'


def load_best(variant, exclude=None):
    path = EVAL_DIR / f"iter_recall_{variant}.json"
    if not path.exists():
        return {}
    data = json.loads(path.read_text())
    best = defaultdict(float)
    for r in data["rows"]:
        fn = r["func"]
        if exclude and fn in exclude:
            continue
        best[fn] = max(best[fn], r["recall_fuzzy"])
    return dict(best)


def load_iter1(variant, exclude=None):
    path = EVAL_DIR / f"iter_recall_{variant}.json"
    if not path.exists():
        return {}
    data = json.loads(path.read_text())
    iter1 = {}
    for r in data["rows"]:
        fn = r["func"]
        if exclude and fn in exclude:
            continue
        if r["iter"] == 1:
            iter1[fn] = r["recall_fuzzy"]
    return iter1


def figure1_conditions_bar():
    """Two-panel bar chart: recall by condition A-F for Claude (left) and A-E for Qwen (right)."""
    claude_conds = [
        ("A", "NL docs",            load_best("feedback_loop_A_claude")),
        ("B", "No NL",              load_best("feedback_loop_B_claude")),
        ("C", "NL+CoT",             load_best("feedback_loop_C_claude")),
        ("D", "No NL+CoT",          load_best("feedback_loop_D_claude")),
        ("E", "same-family GT†",    load_best("feedback_loop_E_claude", exclude=ANCHOR_FUNCS)),
        ("F", "wrong-family GT†",   load_best("feedback_loop_F_claude", exclude=ANCHOR_FUNCS)),
    ]
    qwen_conds = [
        ("A", "NL docs",            load_best("feedback_loop_A")),
        ("B", "No NL",              load_best("feedback_loop_B")),
        ("C", "NL+CoT",             load_best("feedback_loop_C")),
        ("D", "No NL+CoT",          load_best("feedback_loop_D")),
        ("E", "same-family GT†",    load_best("feedback_loop_E", exclude=ANCHOR_FUNCS)),
    ]

    def _make_panel(ax, conds, title):
        labels, recalls, colors = [], [], []
        for cond, desc, best in conds:
            if not best:
                continue
            vals = list(best.values())
            labels.append(f"({cond})\n{desc}")
            recalls.append(sum(vals) / len(vals) * 100)
            colors.append("#2e7d32" if cond in ("E", "F") else "#1565c0")

        bars = ax.bar(range(len(labels)), recalls, color=colors, edgecolor='white', width=0.6)
        for bar, v in zip(bars, recalls):
            ax.text(bar.get_x() + bar.get_width() / 2, v + 0.5, f"{v:.1f}%",
                    ha='center', va='bottom', fontsize=8, fontweight='bold')
        ax.set_xticks(range(len(labels)))
        ax.set_xticklabels(labels, fontsize=8)
        ax.set_ylabel("Best-iter recall (%)", fontsize=10)
        ax.set_ylim(0, 75)
        ax.axhline(y=recalls[0], color='gray', linestyle='--', alpha=0.5, linewidth=1)
        ax.set_title(title, fontsize=11)

    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 4.5))
    _make_panel(ax1, claude_conds, "Claude — aws-c-common (n=76†)")
    _make_panel(ax2, qwen_conds,   "Qwen — aws-c-common (n=76†)")

    null_patch = mpatches.Patch(color='#1565c0', label='Null conditions (A≈B≈C≈D)')
    shot_patch = mpatches.Patch(color='#2e7d32', label='Few-shot GT example (E, F)')
    fig.legend(handles=[null_patch, shot_patch], loc='upper center',
               bbox_to_anchor=(0.5, 0.02), ncol=2, fontsize=9)
    fig.text(0.99, 0.03, "†Excl. 7 anchor functions", ha='right', fontsize=8, color='gray')

    plt.tight_layout(rect=[0, 0.06, 1, 1])
    out = FIG_DIR / "conditions_bar.pdf"
    plt.savefig(out, bbox_inches='tight')
    plt.close()
    print(f"Saved {out}")


def figure2_crossmodel_heatmap():
    """Grouped bar chart: per-family recall for Claude vs Qwen, A vs E."""
    ca = load_best("feedback_loop_A_claude")
    ce = load_best("feedback_loop_E_claude", exclude=ANCHOR_FUNCS)
    qa = load_best("feedback_loop_A")
    qe = load_best("feedback_loop_E", exclude=ANCHOR_FUNCS)

    common = sorted(set(ca) & set(ce) & set(qa) & set(qe))

    fam_stats = {}
    for fam in FAMILIES:
        fns = [f for f in common if family(f) == fam]
        if not fns:
            continue
        n = len(fns)
        fam_stats[fam] = {
            'ca': sum(ca[f] for f in fns) / n * 100,
            'ce': sum(ce[f] for f in fns) / n * 100,
            'qa': sum(qa[f] for f in fns) / n * 100,
            'qe': sum(qe[f] for f in fns) / n * 100,
            'n': n,
        }

    fams = [f for f in FAMILIES if f in fam_stats]
    x = np.arange(len(fams))
    width = 0.2

    fig, ax = plt.subplots(figsize=(10, 5))
    ax.bar(x - 1.5*width, [fam_stats[f]['ca'] for f in fams], width, label='Claude A', color='#1565c0', alpha=0.85)
    ax.bar(x - 0.5*width, [fam_stats[f]['ce'] for f in fams], width, label='Claude E', color='#1565c0', alpha=1.0, edgecolor='white')
    ax.bar(x + 0.5*width, [fam_stats[f]['qa'] for f in fams], width, label='Qwen A', color='#b71c1c', alpha=0.85)
    ax.bar(x + 1.5*width, [fam_stats[f]['qe'] for f in fams], width, label='Qwen E', color='#b71c1c', alpha=1.0, edgecolor='white')

    ax.set_xticks(x)
    ax.set_xticklabels([f.replace('_', '\\_') for f in fams], fontsize=9)
    ax.set_ylabel("Best-iter recall (%)", fontsize=11)
    ax.set_ylim(0, 100)
    ax.set_title("Per-family recall: Claude vs Qwen, Condition A vs E (n=76)", fontsize=12)
    ax.legend(fontsize=9, ncol=2)

    plt.tight_layout()
    out = FIG_DIR / "crossmodel_heatmap.pdf"
    plt.savefig(out, bbox_inches='tight')
    plt.close()
    print(f"Saved {out}")


def figure3_taxonomy_pie():
    """Pie chart of missed property taxonomy categories."""
    annot_path = ANNOT_DIR / "annotated_missed_properties.csv"
    if not annot_path.exists():
        print("annotation file not found, skipping taxonomy pie")
        return

    import csv
    cats = Counter()
    with open(annot_path) as f:
        for row in csv.DictReader(f):
            cats[row['category']] += 1

    # Structural vs non-structural
    structural = {'VALIDITY_PRED', 'FRAME_COND', 'LEN_INVARIANT', 'STRUCT_PTR', 'LEN_CHANGE'}
    total = sum(cats.values())

    # Sort for clarity
    sorted_cats = sorted(cats.items(), key=lambda x: -x[1])
    labels = [c for c, _ in sorted_cats]
    sizes = [n for _, n in sorted_cats]
    colors_struct = ['#c62828' if c in structural else '#1565c0' for c in labels]

    fig, ax = plt.subplots(figsize=(7, 6))
    wedges, texts, autotexts = ax.pie(
        sizes, labels=[f"{c}\n({n})" for c, n in sorted_cats],
        autopct='%1.0f%%', startangle=140,
        colors=colors_struct, pctdistance=0.82,
        textprops={'fontsize': 8}
    )
    for at in autotexts:
        at.set_fontsize(7)

    struct_pct = sum(cats[c] for c in structural) / total * 100
    ax.set_title(
        f"Taxonomy of missed CBMC properties (n={total})\n"
        f"Structural categories (red) = {struct_pct:.0f}%",
        fontsize=11)

    struct_patch = mpatches.Patch(color='#c62828', label='Structural (VALIDITY_PRED, FRAME, LEN, PTR)')
    other_patch = mpatches.Patch(color='#1565c0', label='Non-structural (TYPE, OVERFLOW, ZERO, RETVAL)')
    ax.legend(handles=[struct_patch, other_patch], loc='lower center',
              bbox_to_anchor=(0.5, -0.08), fontsize=8)

    plt.tight_layout()
    out = FIG_DIR / "taxonomy_pie.pdf"
    plt.savefig(out, bbox_inches='tight')
    plt.close()
    print(f"Saved {out}")


def figure4_iter1_vs_best():
    """Grouped bar chart: iter-1 vs best-iter for A and E (Claude + Qwen)."""
    data = {
        'Claude A': {
            'iter1': load_iter1("feedback_loop_A_claude"),
            'best':  load_best("feedback_loop_A_claude"),
        },
        'Claude E': {
            'iter1': load_iter1("feedback_loop_E_claude", exclude=ANCHOR_FUNCS),
            'best':  load_best("feedback_loop_E_claude", exclude=ANCHOR_FUNCS),
        },
        'Qwen A': {
            'iter1': load_iter1("feedback_loop_A"),
            'best':  load_best("feedback_loop_A"),
        },
        'Qwen E': {
            'iter1': load_iter1("feedback_loop_E", exclude=ANCHOR_FUNCS),
            'best':  load_best("feedback_loop_E", exclude=ANCHOR_FUNCS),
        },
    }

    # Use functions common to all
    all_common = sorted(
        set(data['Claude A']['best']) & set(data['Claude E']['best']) &
        set(data['Qwen A']['best']) & set(data['Qwen E']['best'])
    )

    labels = list(data.keys())
    iter1_means = []
    best_means = []
    for label in labels:
        fns = [f for f in all_common if f in data[label]['iter1']]
        n = len(fns)
        iter1_means.append(sum(data[label]['iter1'][f] for f in fns) / n * 100 if fns else 0)
        best_means.append(sum(data[label]['best'][f] for f in fns) / n * 100 if fns else 0)

    x = np.arange(len(labels))
    width = 0.35

    fig, ax = plt.subplots(figsize=(8, 4.5))
    b1 = ax.bar(x - width/2, iter1_means, width, label='Iter 1 (before feedback)', color='#ef6c00', alpha=0.85)
    b2 = ax.bar(x + width/2, best_means, width, label='Best iter (after feedback)', color='#1565c0', alpha=0.85)

    for bars in [b1, b2]:
        for bar in bars:
            h = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2, h + 0.5, f"{h:.0f}%",
                    ha='center', va='bottom', fontsize=8)

    ax.set_xticks(x)
    ax.set_xticklabels(labels, fontsize=10)
    ax.set_ylabel("Recall (%)", fontsize=11)
    ax.set_ylim(0, 75)
    ax.set_title("Iter-1 vs Best-iter Recall: Impact of GT Example", fontsize=12)
    ax.legend(fontsize=10)

    plt.tight_layout()
    out = FIG_DIR / "iter1_vs_best.pdf"
    plt.savefig(out, bbox_inches='tight')
    plt.close()
    print(f"Saved {out}")


def figure5_replication():
    """Grouped bar chart: A vs E across libraries and models (replication figure)."""
    # Load s2n recall data
    def load_s2n(fname, excl_func=None):
        path = S2N_EVAL_DIR / fname
        if not path.exists():
            return []
        data = json.loads(path.read_text())
        return [r["recall_fuzzy"] for r in data if r.get("func") != excl_func]

    s2n_A = load_s2n("s2n_recall_condA.json")
    s2n_E = load_s2n("s2n_recall_condE.json", excl_func="s2n_stuffer_init")

    # aws-c-common Claude A vs E (n=76)
    ca = load_best("feedback_loop_A_claude")
    ce = load_best("feedback_loop_E_claude", exclude=ANCHOR_FUNCS)
    common_ce = [f for f in ca if f in ce]
    aws_claude_A = [ca[f] for f in common_ce]
    aws_claude_E = [ce[f] for f in common_ce]

    # aws-c-common Qwen A vs E (n=76)
    qa = load_best("feedback_loop_A")
    qe = load_best("feedback_loop_E", exclude=ANCHOR_FUNCS)
    common_qe = [f for f in qa if f in qe]
    aws_qwen_A = [qa[f] for f in common_qe]
    aws_qwen_E = [qe[f] for f in common_qe]

    groups = [
        ("Claude\naws-c-common\n(n=76)", aws_claude_A, aws_claude_E),
        ("Qwen\naws-c-common\n(n=76)", aws_qwen_A, aws_qwen_E),
        ("Qwen\ns2n-tls\n(n=24)", s2n_A, s2n_E),
    ]

    x = np.arange(len(groups))
    width = 0.35

    fig, ax = plt.subplots(figsize=(9, 5))
    A_means = [np.mean(g[1]) * 100 for g in groups]
    E_means = [np.mean(g[2]) * 100 for g in groups]

    b1 = ax.bar(x - width/2, A_means, width, label='Cond. A (NL docs, no example)',
                color='#1565c0', alpha=0.85)
    b2 = ax.bar(x + width/2, E_means, width, label='Cond. E (NL + same-family GT example)',
                color='#2e7d32', alpha=0.85)

    for bars in [b1, b2]:
        for bar in bars:
            h = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2, h + 0.5, f"{h:.1f}%",
                    ha='center', va='bottom', fontsize=9, fontweight='bold')

    ax.set_xticks(x)
    ax.set_xticklabels([g[0] for g in groups], fontsize=10)
    ax.set_ylabel("Mean recall (%)", fontsize=11)
    ax.set_ylim(0, 75)
    ax.set_title("Few-shot GT Example (+9pp) Replicates Across Libraries and Models", fontsize=12)
    ax.legend(fontsize=10, loc='upper left')

    plt.tight_layout()
    out = FIG_DIR / "replication.pdf"
    plt.savefig(out, bbox_inches='tight')
    plt.close()
    print(f"Saved {out}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--fig", type=int, choices=[1, 2, 3, 4, 5], help="Generate specific figure")
    args = parser.parse_args()

    if not MATPLOTLIB_OK:
        print("matplotlib not available")
        return

    if args.fig is None or args.fig == 1:
        figure1_conditions_bar()
    if args.fig is None or args.fig == 2:
        figure2_crossmodel_heatmap()
    if args.fig is None or args.fig == 3:
        figure3_taxonomy_pie()
    if args.fig is None or args.fig == 4:
        figure4_iter1_vs_best()
    if args.fig is None or args.fig == 5:
        figure5_replication()


if __name__ == "__main__":
    main()
