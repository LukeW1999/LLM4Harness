#!/usr/bin/env python3
"""
lint_baseline_640.py — could a cheap static check have found these silences?

The oracle costs a mutation corpus and two CBMC runs per mutant. A reviewer is
entitled to ask what a lint would have caught for free, so we run three of the
obvious ones over the same final harnesses and score them as detectors of the
silencing groups the oracle certifies.

  no-retval   no assertion mentions the value the function under test returns
  fewer       the harness has fewer assertions than the expert's for that function
  none-after  no assertion at all follows the last call site

Precision matters more than recall here. A lint that flags most harnesses buys
nothing: the question is whether flagging is informative about silencing, so we
report both rates and their ratio rather than recall alone.

  python3 scripts/lint_baseline_640.py
"""
import collections, json, re, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "figures"))
import style as S  # noqa: E402

EXP = HERE.parent
PAPER8 = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
          "G_gptoss120b", "A_claude", "H_claude", "M_claude"]
GT_PROOFS = Path("/home/weiqi/Verification/aws-c-common/verification/cbmc/proofs")

def final_harness(cond, func):
    d = EXP / "results" / f"feedback_loop_{cond}" / func
    its = sorted(d.glob("iter_*_harness.c"),
                 key=lambda p: int(re.search(r"iter_(\d+)_", p.name).group(1)))
    return its[-1].read_text(errors="replace") if its else None

def asserts(src):
    return [l for l in src.splitlines() if re.match(r"\s*(?:__CPROVER_)?assert\s*\(", l)]

def lints(src, func, gt_src):
    a = asserts(src)
    call = re.compile(rf"^\s*(?:[\w\s\*]+=\s*)?{re.escape(func)}\s*\(")
    lines = src.splitlines()
    calls = [i for i, l in enumerate(lines) if call.match(l)]
    after = [l for i, l in enumerate(lines) if calls and i > calls[-1]
             and re.match(r"\s*(?:__CPROVER_)?assert\s*\(", l)]
    # the value the call is bound to, if any
    ret = None
    for l in lines:
        m = re.match(rf"\s*(?:[\w\s\*]+\s)?(\w+)\s*=\s*{re.escape(func)}\s*\(", l)
        if m:
            ret = m.group(1); break
    return {
        "no-retval": ret is None or not any(re.search(rf"\b{re.escape(ret)}\b", x) for x in a),
        "fewer": len(a) < len(asserts(gt_src)) if gt_src else False,
        "none-after": not after,
    }

def main():
    canon, llm = S.gt_fail_set(), S.llm_verdicts()
    names = ["no-retval", "fewer", "none-after"]
    tab = {n: collections.Counter() for n in names}
    groups = 0
    for cond in PAPER8:
        v = llm.get(cond, {})
        funcs = {f for (f, _m) in canon}
        for f in sorted(funcs):
            src = final_harness(cond, f)
            if src is None:
                continue
            gtp = GT_PROOFS / f / f"{f}_harness.c"
            gt = gtp.read_text(errors="replace") if gtp.exists() else None
            silencing = any(v.get((f, m)) == "SUCCESS" for (ff, m) in canon if ff == f)
            groups += 1
            for n, flagged in lints(src, f, gt).items():
                tab[n][("sil" if silencing else "clean", "flag" if flagged else "pass")] += 1

    print(f"{groups} (condition, function) groups\n")
    print(f"{'lint':12s} {'flags silencing':>16s} {'flags clean':>13s} {'precision':>10s} {'recall':>8s}")
    for n in names:
        tp = tab[n][("sil", "flag")]; fn = tab[n][("sil", "pass")]
        fp = tab[n][("clean", "flag")]; tn = tab[n][("clean", "pass")]
        prec = tp / (tp + fp) if tp + fp else 0
        rec = tp / (tp + fn) if tp + fn else 0
        base = (tp + fn) / (tp + fn + fp + tn)
        print(f"{n:12s} {tp:6d}/{tp+fn:<9d} {fp:6d}/{fp+tn:<6d} "
              f"{100*prec:9.1f}% {100*rec:7.1f}%   (base rate {100*base:.1f}%)")
    json.dump({n: {str(k): v for k, v in tab[n].items()} for n in names},
              open(EXP / "evaluation/lint_baseline_640.json", "w"), indent=1)

if __name__ == "__main__":
    main()
