#!/usr/bin/env python3
"""
Figure A — pass rate against completeness, which is the RQ1 claim in one picture.

Left: the silenced share rises with pass rate instead of falling. Right: assertion
recall does not follow pass rate either. Bars give the min-max over every repeat
generation of that condition, so a reader can see which orderings survive
repetition (only Oracle's distance from the rest does) without hunting in prose.

  python3 scripts/figures/fig_passrate_vs_completeness.py
"""
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import style as S  # noqa: E402
import matplotlib.pyplot as plt  # noqa: E402

CONDS = ["G_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
         "K_gptoss120b", "Oracle_gptoss120b"]

def pass_rates():
    pr = S.load("passrate_640.json")["per_condition"]
    out = {c: r["pass_pct_640"] for c, r in pr.items()}
    out["K_gptoss120b"] = S.load("k_passrate_640.json")["pass_pct_640"]
    return out

def silgt_runs():
    """condition family -> every run's Sil/GT %."""
    runs = {}
    prim = S.load("silenced_640.json")["summary"]["per_condition"]
    for c, r in prim.items():
        runs.setdefault(c, []).append(r["SilGT_640_pct"])
    runs.setdefault("K_gptoss120b", []).append(
        S.load("kllama_oracle_640.json")["per_condition"]["K_gptoss120b"]["SilGT_pct"])
    try:
        for c, r in S.load("silenced_repeats_640.json")["summary"]["per_condition"].items():
            runs.setdefault(re.sub(r"_r\d+$", "", c), []).append(r["SilGT_640_pct"])
    except FileNotFoundError:
        pass
    return runs

def recall():
    out = {}
    for c in CONDS:
        letter, model = c.split("_", 1)
        p = S.EVAL / f"cross_verify_results_cond{letter}_{model}.json"
        if not p.exists():
            continue
        import json
        e = json.load(open(p))
        vals = [x["harness_recall"] for x in e if x["gt_harness_count"] > 0]
        out[c] = sum(vals) / len(vals)
    return out

def baseline_without_cluster():
    """Baseline's Sil/GT once the one dominant function is dropped."""
    canon = S.gt_fail_set()
    v = S.llm_verdicts()["A_gptoss120b"]
    keep = [k for k in canon if k[0] != "aws_byte_buf_cat"]
    sil = sum(1 for k in keep if v.get(k) == "SUCCESS")
    return 100.0 * sil / len(canon)

def main():
    S.setup()
    pr, runs, rc = pass_rates(), silgt_runs(), recall()
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(S.TEXTWIDTH, 2.15), sharex=True)

    for c in CONDS:
        x, ys = pr[c], runs.get(c, [])
        if not ys:
            continue
        primary = ys[0]
        if len(ys) > 1:
            ax1.plot([x, x], [min(ys), max(ys)], color=S.MODEL["gpt-oss"], lw=1.2,
                     alpha=0.5, zorder=1, marker="_", ms=4)
        ax1.plot(x, primary, "o", ms=4.5, color=S.MODEL["gpt-oss"], zorder=3)
        ax1.annotate(S.COND_LABEL[c], (x, primary), textcoords="offset points",
                     xytext=(0, 7 if c != "M_gptoss120b" else -11), ha="center", fontsize=6.5)

    bx, by = pr["A_gptoss120b"], baseline_without_cluster()
    ax1.plot(bx, by, "o", ms=4.5, mfc="white", mec=S.MODEL["gpt-oss"], mew=1, zorder=3)
    ax1.annotate("Baseline less its\none dominant function", (bx, by),
                 textcoords="offset points", xytext=(8, -3), va="center",
                 fontsize=6, color="#555555")

    ax1.annotate("", xy=(pr["Oracle_gptoss120b"], runs["Oracle_gptoss120b"][0] - 3),
                 xytext=(pr["A_gptoss120b"], runs["A_gptoss120b"][0] + 3),
                 arrowprops=dict(arrowstyle="->", ls="--", lw=0.8, color="#666666"))
    ax1.annotate("+ expert preconditions", (60, 24), fontsize=6.5, color="#444444", ha="center")
    ax1.set_xlabel("verifier pass rate (%)")
    ax1.set_ylabel("silenced share of GT-fail set (%)")
    ax1.set_title("(a) more acceptance, more silencing", loc="left")

    for c in CONDS:
        if c in rc:
            ax2.plot(pr[c], rc[c], "o", ms=4.5, color=S.MODEL["gpt-oss"])
            ax2.annotate(S.COND_LABEL[c], (pr[c], rc[c]), textcoords="offset points",
                         xytext=(0, 7), ha="center", fontsize=6.5)
    ax2.set_xlabel("verifier pass rate (%)")
    ax2.set_ylabel("assertion recall")
    ax2.set_ylim(0.20, 0.44)
    ax2.set_title("(b) and no more of the expert's assertions", loc="left")

    S.save(fig, "fig_passrate_vs_completeness")

if __name__ == "__main__":
    main()
