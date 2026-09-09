#!/usr/bin/env python3
"""
s2n_convention_640.py — does the model carry aws-c-common's harness convention
into a corpus that does not use it?

aws-c-common proof harnesses include <proof_helpers/make_common_data_structures.h>.
s2n-tls has no such header; its own helpers live in
<cbmc_proof/make_common_datastructures.h>. A harness that includes the aws path
cannot be preprocessed against s2n, so CBMC returns no verdict at all and the
harness silences nothing for the reason that it never runs.

This counts, per condition, how many of the 25 s2n functions end their last
iteration still holding the wrong include, and how many iterations were spent.

Usage:  python3 scripts/s2n_convention_640.py
"""
import json, re, sys
from pathlib import Path

EXP = Path(__file__).resolve().parent.parent
CONDS = ["A_gptoss120b", "G_gptoss120b", "H_gptoss120b", "M_gptoss120b",
         "Oracle_gptoss120b", "A_claude", "H_claude", "M_claude"]
AWS = "proof_helpers/"
S2N = "cbmc_proof/"

def final(d):
    its = sorted(d.glob("iter_*_harness.c"),
                 key=lambda p: int(re.search(r"iter_(\d+)_", p.name).group(1)))
    return (its[-1], len(its)) if its else (None, 0)

def main():
    out = {}
    for cond in CONDS:
        root = EXP / "results" / f"feedback_loop_{cond}"
        funcs = sorted(p for p in root.glob("s2n_*") if p.is_dir()) if root.exists() else []
        wrong = right = iters = n = 0
        for d in funcs:
            f, k = final(d)
            if f is None:
                continue
            n += 1; iters += k
            src = f.read_text(errors="replace")
            wrong += AWS in src
            right += S2N in src
        out[cond] = {"funcs": n, "aws_include_at_end": wrong, "s2n_include_at_end": right,
                     "mean_iterations": round(iters / n, 1) if n else 0}
        print(f"{cond:20s} {n:2d} funcs  aws-include {wrong:2d}  s2n-include {right:2d}  "
              f"mean iters {out[cond]['mean_iterations']}")

    gpt = [c for c in CONDS if "gptoss" in c]
    cla = [c for c in CONDS if "claude" in c]
    agg = lambda cs: (sum(out[c]["aws_include_at_end"] for c in cs),
                      sum(out[c]["funcs"] for c in cs))
    gw, gn = agg(gpt); cw, cn = agg(cla)
    summary = {"gptoss_wrong": gw, "gptoss_funcs": gn, "gptoss_pct": round(100 * gw / gn, 1),
               "claude_wrong": cw, "claude_funcs": cn, "claude_pct": round(100 * cw / cn, 1)}
    print(f"\ngpt-oss {gw}/{gn} ({summary['gptoss_pct']}%)   "
          f"Claude {cw}/{cn} ({summary['claude_pct']}%)")
    json.dump({"summary": summary, "per_condition": out},
              open(EXP / "evaluation/s2n_convention_640.json", "w"), indent=1)

if __name__ == "__main__":
    main()
