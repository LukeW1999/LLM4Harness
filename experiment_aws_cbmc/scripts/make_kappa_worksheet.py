#!/usr/bin/env python3
"""
make_kappa_worksheet.py — build the blind rating sheet for the second rater.

The four layers of RQ2 are decided by CBMC, so a human cannot re-adjudicate them
without re-running it. What a human can decide, and what the tooling might get
wrong, is the question the exact-match rule answers mechanically and claims is
rename-immune: does the LLM harness express the property the expert's catching
assertion checks, in any syntactic form at all?

That is the rateable claim, so that is what the sheet asks. Each of the 32 live
groups gets the expert's assertions and the LLM harness's assertions side by
side, in shuffled order, with no layer label and no hint of the tool's answer.
The rater writes Y, N or ? per group; `score_kappa.py` then compares those with
the rule's own verdict and reports Cohen's kappa.

  python3 scripts/make_kappa_worksheet.py
"""
import collections, glob, json, random, re, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "figures"))
sys.path.insert(0, str(HERE))
import style as S  # noqa: E402

EXP = HERE.parent
GT_PROOFS = Path("/home/weiqi/Verification/aws-c-common/verification/cbmc/proofs")
PAPER8 = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
          "G_gptoss120b", "A_claude", "H_claude", "M_claude"]

def asserts(path):
    if not path.exists():
        return []
    return [re.sub(r"\s+", " ", l).strip()
            for l in path.read_text(errors="replace").splitlines()
            if re.match(r"\s*(?:__CPROVER_)?assert\s*\(", l)]

def final(cond, func):
    d = EXP / "results" / f"feedback_loop_{cond}" / func
    its = sorted(d.glob("iter_*_harness.c"),
                 key=lambda p: int(re.search(r"iter_(\d+)_", p.name).group(1)))
    return its[-1] if its else None

def main():
    canon, llm = S.gt_fail_set(), S.llm_verdicts()
    fullp = {(r["cond"], r["func"]): r
             for r in json.load(open(EXP / "evaluation/reachability_full_640.json"))["rows"]}
    dead = {k for k, r in fullp.items() if r.get("head") != "FAIL"}
    dead |= {k for k in S.dead_groups() if k not in fullp}

    rows = []
    for cond in PAPER8:
        v = llm.get(cond, {})
        counts = collections.Counter(f for (f, m) in canon
                                     if v.get((f, m)) == "SUCCESS" and (cond, f) not in dead)
        for func, n in counts.items():
            h = final(cond, func)
            rows.append({"cond": cond, "func": func, "silences": n,
                         "gt": asserts(GT_PROOFS / func / f"{func}_harness.c"),
                         "llm": asserts(h) if h else []})

    random.Random(20260910).shuffle(rows)
    out = ["# Blind rating sheet: is the expert's property expressed at all?",
           "",
           "For each item below you see the expert harness's assertions and the LLM",
           "harness's assertions for the same function. You do not see which layer the",
           "tooling assigned, and the items are shuffled.",
           "",
           "One question per item:",
           "",
           "> Does the LLM harness assert the property the expert's assertions check,",
           "> in any form at all, however it is written or named?",
           "",
           "Answer `Y`, `N`, or `?` on the ANSWER line. `Y` means the property is there",
           "in some syntactic form, even paraphrased, split across assertions, or under",
           "different variable names. `N` means it is absent. `?` means you cannot tell",
           "from the assertions alone.",
           "",
           "Do not open the repository while rating. Do not look up the function.",
           "",
           "---", ""]
    for i, r in enumerate(rows, 1):
        out.append(f"## Item {i:02d}  (`{r['func']}`)")
        out.append("")
        out.append("Expert harness asserts:")
        out.append("```c")
        out += (r["gt"] or ["(none extracted)"])
        out.append("```")
        out.append("")
        out.append("LLM harness asserts:")
        out.append("```c")
        out += (r["llm"] or ["(none)"])
        out.append("```")
        out.append("")
        out.append("ANSWER: ")
        out.append("")
        out.append("---")
        out.append("")
    (EXP / "adjudication").mkdir(exist_ok=True)
    (EXP / "adjudication/kappa_worksheet.md").write_text("\n".join(out))
    json.dump([{"item": i, "cond": r["cond"], "func": r["func"], "silences": r["silences"]}
               for i, r in enumerate(rows, 1)],
              open(EXP / "adjudication/kappa_key.json", "w"), indent=1)
    print(f"wrote adjudication/kappa_worksheet.md ({len(rows)} items)")
    print("wrote adjudication/kappa_key.json (item -> group; do not read while rating)")

if __name__ == "__main__":
    main()
