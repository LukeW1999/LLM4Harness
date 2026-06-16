#!/usr/bin/env python3
"""Read-only: dump per-func final-iteration verify verdict for the 8 pinned
conditions. A func appears only if it has a summary.json (i.e. generation
completed -- truncated funcs are absent, which is what the common-complete
intersection needs). Mirrors paper_numbers.passrate semantics."""
import json, glob, os
RES = "/root/experiment_aws_cbmc/results"
CONDS = ["A", "G", "H", "I", "J", "K", "M", "Oracle"]
out = {}
for c in CONDS:
    d = f"{RES}/feedback_loop_{c}_gptoss120b_pin"
    m = {}
    for sj in glob.glob(f"{d}/*/summary.json"):
        func = os.path.basename(os.path.dirname(sj))
        try:
            s = json.load(open(sj))
            its = s.get("iterations", [])
            if its:
                m[func] = its[-1].get("verify")
        except Exception:
            pass
    out[c] = m
dst = "/root/experiment_aws_cbmc/evaluation/pinned_passverdicts.json"
json.dump(out, open(dst, "w"), indent=1)
print("counts:", {c: len(out[c]) for c in CONDS})
print("wrote", dst)
