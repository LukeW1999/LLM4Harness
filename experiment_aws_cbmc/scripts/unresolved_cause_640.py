#!/usr/bin/env python3
"""
unresolved_cause_640.py — why does a mutant get no verdict under an LLM harness?

Roughly half of some conditions' GT-fail mutants come back neither caught nor
silenced. A natural reading is that our two sides run different builds, since
the same mutant is decided under the expert harness. They do not: the expert and
the LLM run the same flags, defines, unwind bounds and source list, with the
mutant substituted at the same index, and the harness file is the only thing
that differs. So the cause is in the harness, and this says which.

Each unresolved group is re-run once and classified by what CBMC reports:

  missing header   the harness includes a file the corpus does not have
  type error       it contradicts a library declaration, e.g. writing through a
                   const field or omitting a struct member
  object limit     its setup allocates past CBMC's addressable-object bound
  timeout          no verdict within the budget

  CBMC640=... python3 scripts/unresolved_cause_640.py [--cond A_gptoss120b]
"""
import argparse, collections, json, os, subprocess, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE / "figures"))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS  # noqa: E402
import run_mutation_oracle_cbmc as rmo  # noqa: E402
import style as S  # noqa: E402

EXP = HERE.parent
CBMC = os.environ.get("CBMC640") or "cbmc"
MATCH = ["--no-standard-checks", "--no-unwinding-assertions"]
PAPER8 = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
          "G_gptoss120b", "A_claude", "H_claude", "M_claude"]

def classify(out):
    if "No such file or directory" in out:
        return "missing header"
    if "too many addressed objects" in out:
        return "object limit"
    if "CONVERSION ERROR" in out or "PARSING ERROR" in out:
        return "type error"
    return "other"

def run(func, mutant, harness):
    cfg = FUNC_CONFIGS[func]
    idx = rmo.get_mutated_source_idx(func)
    ps = [str(p) for p in cfg["project_sources"]]
    ps[idx] = str(EXP / f"mutants/{func}/{mutant}.c")
    cmd = ([CBMC] + list(COMMON_FLAGS) + list(cfg.get("defines", []))
           + list(cfg["unwind"]) + list(cfg.get("unwindset") or []) + MATCH
           + ["--function", f"{func}_harness"]
           + [str(p) for p in cfg["proof_sources"]] + [str(harness)] + ps)
    try:
        r = subprocess.run(cmd, capture_output=True, text=True, timeout=240)
        return classify(r.stdout + r.stderr)
    except subprocess.TimeoutExpired:
        return "timeout"

def main():
    ap = argparse.ArgumentParser(); ap.add_argument("--cond"); a = ap.parse_args()
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    kinds, rows = collections.Counter(), []
    for cond in ([a.cond] if a.cond else PAPER8):
        v = llm.get(cond, {})
        per = collections.Counter(f for (f, m) in canon
                                  if v.get((f, m)) not in ("SUCCESS", "FAIL", "SAT"))
        for func, n in per.items():
            h = rmo.get_final_harness(EXP / f"results/feedback_loop_{cond}" / func)
            if not h:
                kinds["no harness"] += n; continue
            mut = next(m for (ff, m) in canon if ff == func
                       and v.get((ff, m)) not in ("SUCCESS", "FAIL", "SAT"))
            k = run(func, mut, h)
            kinds[k] += n
            rows.append({"cond": cond, "func": func, "mutants": n, "cause": k})
            print(f"  {k:15s} {cond}/{func} ({n})", flush=True)
    total = sum(kinds.values())
    print(f"\n{total} unresolved (condition, mutant) pairs")
    for k, n in kinds.most_common():
        print(f"   {k:15s} {n:4d}  {100*n/total:5.1f}%")
    json.dump({"summary": {"total": total, "by_cause": dict(kinds)}, "rows": rows},
              open(EXP / "evaluation/unresolved_cause_640.json", "w"), indent=1)

if __name__ == "__main__":
    main()
