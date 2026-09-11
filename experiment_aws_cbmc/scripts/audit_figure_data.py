"""Check that the figures plot the numbers the registry holds.

paper_numbers_640.py proves the prose numbers are true and
audit_paper_coverage.py proves none escaped it, but the figures are a separate
pipeline that neither touches. Figure 3 classified a silence per function while
the layer table classified per mutant, and the two disagreed on three of eight
rows before anyone noticed. This imports each figure's own data functions and
compares what they would draw against the registry.

  python3 scripts/audit_figure_data.py
"""
import importlib.util
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "figures"))


def registry():
    spec = importlib.util.spec_from_file_location("pn", HERE / "paper_numbers_640.py")
    mod = importlib.util.module_from_spec(spec)
    argv, sys.argv = sys.argv, [sys.argv[0]]
    spec.loader.exec_module(mod)
    sys.argv = argv
    return mod


# condition key -> the registry's name for it
COND = {"G_gptoss120b": "Single", "A_gptoss120b": "Baseline", "H_gptoss120b": "Neutral",
        "M_gptoss120b": "Bounded", "Oracle_gptoss120b": "Oracle",
        "A_claude": "Baseline/Claude", "H_claude": "Neutral/Claude",
        "M_claude": "Bounded/Claude"}
LAYER = {"never ran": "never ran", "illegal setup": "illegal setup",
         "unreachable bug": "unreachable fault", "missing assert": "missing assertion"}


def check(rows, name):
    bad = [r for r in rows if abs(r[2] - r[3]) > r[4]]
    print(f"\n== {name}: {len(rows)} values, {len(bad)} disagree")
    for what, where, drawn, want, _ in bad:
        print(f"   {where:26} {what:20} figure={drawn} registry={want}")
    return len(bad)


def main():
    m = registry()
    bad = 0

    # ── Figure 3, both panels ───────────────────────────────────────────────
    import fig_mechanism_stack as F3
    drawn = F3.counts()
    rows = []
    for key, label in COND.items():
        want = m._layers(label)
        got = drawn[key]
        never = got["dead_loop"] + got["dead_assume"]
        rows.append(("never ran", label, never, want["never ran"], 0.5))
        for fig_key, reg_key in (("illegal", "illegal setup"),
                                 ("unreachable", "unreachable bug"),
                                 ("missing", "missing assert")):
            rows.append((LAYER[reg_key], label, got.get(fig_key, 0), want[reg_key], 0.5))
    bad += check(rows, "Figure 3(b), layer counts")

    disp = F3.disposition()
    rows = []
    for key, label in COND.items():
        caught, sil, unres = disp[key]
        rows.append(("Sil/GT %", label, sil, m.sil_gt(label), 0.15))
        rows.append(("caught+sil+unres", label, caught + sil + unres, 100.0, 0.05))
    bad += check(rows, "Figure 3(a), disposition")

    # ── Figure 2, both axes ─────────────────────────────────────────────────
    import fig_passrate_vs_completeness as F2
    pr, sg = F2.pass_rates(), F2.silgt_runs()
    rows = []
    for key, label in COND.items():
        if key in pr and label in m.COND:
            rows.append(("pass % (tabled run)", label, pr[key][0], m.passrate(label), 0.15))
        if key in sg:
            rows.append(("Sil/GT % (tabled run)", label, sg[key][0], m.sil_gt(label), 0.15))
    bad += check(rows, "Figure 2, plotted points")

    print(f"\n---- {bad} figure value(s) disagree with the registry")
    return bad


if __name__ == "__main__":
    sys.exit(1 if main() else 0)
