#!/usr/bin/env python3
"""
Figure F — how much active deletion you find depends on how you look for it.

Three rules over the same live silences: a token-Jaccard overlap test, the
exact-match rule the paper reports (an assertion written verbatim, gone from the
final harness, with CBMC UNKNOWN at the iteration it vanished), and a behavioural
re-run that asks CBMC whether any iteration actually caught the mutant. The
overlap rule scores semantically opposite assertions as matches and inflates the
count, which is the measurement-validity result of the paper.

  python3 scripts/figures/fig_deletion_attribution.py
"""
import importlib.util
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import style as S  # noqa: E402
import matplotlib.pyplot as plt  # noqa: E402
from scipy.stats import beta  # noqa: E402

CONDS = ["A_gptoss120b", "H_gptoss120b", "M_gptoss120b", "G_gptoss120b",
         "Oracle_gptoss120b", "A_claude", "H_claude", "M_claude"]

def _load_module(name, path):
    spec = importlib.util.spec_from_file_location(name, path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod

def jaccard_deletions(group_sizes):
    """Silences the token-overlap rule would call deletions: a GT assertion
    'appears' in an earlier iteration by Jaccard and is absent from the final.
    Counted in mutants, like every other attribution, not in groups."""
    A = _load_module("A", S.EXP / "scripts/attribution_analysis.py")
    n = 0
    for (cond, func), size in group_sizes.items():
        gts = A.get_gt_asserts(func)
        its = A.get_llm_iter_asserts(f"feedback_loop_{cond}", func)
        if not gts or not its:
            continue
        for g in gts:
            any_, final = A.classify_gt_assert(g, its)
            if any_ and not final:
                n += size
                break
    return n

def cp(k, n):
    lo = beta.ppf(0.025, k, n - k + 1) if k > 0 else 0.0
    hi = beta.ppf(0.975, k + 1, n - k) if k < n else 1.0
    return 100 * lo, 100 * hi

def main():
    S.setup()
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    dead = S.dead_groups()

    import collections
    group_sizes = collections.Counter()
    for cond in CONDS:
        v = llm.get(cond, {})
        for (f, m) in canon:
            if v.get((f, m)) == "SUCCESS" and (cond, f) not in dead:
                group_sizes[(cond, f)] += 1
    live_n = sum(group_sizes.values())

    jac = jaccard_deletions(group_sizes)
    exact = 2      # Claude Baseline, aws_ring_buffer_buf_belongs_to_pool
    behav = 2      # gpt-oss Baseline, aws_ptr_eq
    union = exact + behav

    methods = [("token-Jaccard $\\geq$ 0.45", jac, S.MECH["deleted"]),
               ("exact match + CBMC verdict", exact, "#8C564B"),
               ("behavioural re-run", behav, "#7F7F7F"),
               ("union of the two strict rules", union, "#333333")]

    fig, ax = plt.subplots(figsize=(S.TEXTWIDTH * 0.62, 1.75))
    ys = list(range(len(methods)))[::-1]
    for y, (label, n, col) in zip(ys, methods):
        pct = 100 * n / live_n
        lo, hi = cp(n, live_n)
        ax.barh(y, pct, height=0.6, color=col)
        ax.plot([lo, hi], [y, y], color="#222222", lw=1)
        ax.text(hi + 0.4, y, f"{n} of {live_n}", va="center", fontsize=6.5, color="#333333")
    ax.set_yticks(ys, [m[0] for m in methods])
    ax.set_xlabel("live silences attributed to active deletion (%)")
    ax.set_xlim(0, 14)
    ax.grid(axis="y", visible=False)
    S.save(fig, "fig_deletion_attribution")
    print(f"jaccard={jac} exact={exact} behavioural={behav} live={live_n}")

if __name__ == "__main__":
    main()
