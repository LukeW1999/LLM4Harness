#!/usr/bin/env python3
"""A3: s2n assume-relaxation cross-validation (B1 for the s2n corpus).

Aligns s2n KG attribution strength to the aws main corpus. For each s2n *silenced*
(func, mutant), strip __CPROVER_assume BOUND constraints from the LLM final harness
and re-run CBMC on the ORIGINAL vs the MUTANT source (s2n schema). CBMC-decided,
rename-immune. Mirrors scripts/b1_cex_attrib.py classification exactly; reuses
scripts/s2n_oracle.py for the s2n CBMC plumbing.

  KG_confirmed   : even with bounds relaxed, the relaxed harness still cannot catch
                   the mutant (SUCCESS on both original and mutant) -> knowledge gap.
  AOC_confirmed  : relaxing bounds makes the harness catch the mutant it previously
                   silenced (SUCCESS/UNKNOWN on original, FAIL on mutant) -> the
                   original silence was assume-over-constraint, not a knowledge gap.
  INCONCLUSIVE_relax_broke_validity : relaxation made the harness FAIL on the
                   original source (can't attribute).
  KG_no_bounds   : no bound assumes to relax -> silence is not bound-gaming -> KG.

Usage:  python3 scripts/s2n_b1_relax.py --cond A_claude            # full
        python3 scripts/s2n_b1_relax.py --cond A_claude --func s2n_stuffer_alloc   # smoke
Output: evaluation/b1_relax_s2n_<cond>.json
NOTE: CBMC-heavy; do NOT run concurrently with the cross-verify sweep (contention).
"""
import sys, re, json, tempfile, argparse, glob
from pathlib import Path
sys.path.insert(0, "/root/experiment_aws_cbmc/scripts")
import cbmc_runner as C
import s2n_oracle as S

# Corpus-agnostic; copied verbatim from b1_cex_attrib.strip_bound_assumes to avoid
# importing the aws-coupled b1 module (which loads the aws oracle on import).
BOUND = re.compile(r'__CPROVER_assume\s*\([^;]*(<=|<|>=|>)\s*[A-Za-z0-9_]+\s*\)\s*;')
def strip_bound_assumes(text):
    out = []; removed = 0
    for line in text.splitlines(keepends=True):
        if BOUND.search(line):
            removed += 1; continue
        out.append(line)
    return "".join(out), removed

def orig_source(func, mutated_name):
    """The unmutated source file in cfg whose basename == mutated_name (so feeding it
    to s2n_oracle.cbmc_on as the 'mutant' is a no-op substitution == original)."""
    for s in C.FUNC_CONFIGS[func]["sources"]:
        if Path(s).name == mutated_name:
            return s
    return None

def classify(cond, timeout=90, func_filter=None):
    op = S.EVAL / f"mutation_oracle_s2n_{cond}.json"
    sil = [r for r in json.load(open(op))["results"] if r.get("silenced")]
    if func_filter:
        sil = [r for r in sil if r["func"] == func_filter]
    out = []
    for r in sil:
        func, mut = r["func"], r["mutant"]
        meta = json.load(open(S.MUT / func / "metadata.json")); mn = meta.get("mutated_name")
        lh = S.llm_harness(func, f"feedback_loop_{cond}")
        osrc = orig_source(func, mn)
        mutc = S.MUT / func / f"{mut}.c"
        if not (lh and osrc and mutc.exists()):
            out.append((func, mut, "NO_FILES", 0)); continue
        stripped, nrem = strip_bound_assumes(Path(lh).read_text(errors="replace"))
        if nrem == 0:
            out.append((func, mut, "KG_no_bounds", 0)); continue   # nothing to relax -> KG
        tf = tempfile.NamedTemporaryFile("w", suffix="_harness.c", delete=False, dir="/tmp")
        tf.write(stripped); tf.close()
        r_orig = S.cbmc_on(func, tf.name, osrc, mn, timeout)   # stripped harness on ORIGINAL
        r_mut  = S.cbmc_on(func, tf.name, mutc, mn, timeout)   # stripped harness on MUTANT
        if r_orig == "FAIL":
            lab = "INCONCLUSIVE_relax_broke_validity"
        elif r_orig in ("SUCCESS", "UNKNOWN") and r_mut == "FAIL":
            lab = "AOC_confirmed"
        elif r_orig == "SUCCESS" and r_mut == "SUCCESS":
            lab = "KG_confirmed"
        else:
            lab = f"OTHER({r_orig}/{r_mut})"
        out.append((func, mut, lab, nrem))
    return out

if __name__ == "__main__":
    ap = argparse.ArgumentParser()
    ap.add_argument("--cond", required=True)
    ap.add_argument("--func", default=None, help="restrict to one function (smoke test)")
    ap.add_argument("--timeout", type=int, default=90)
    a = ap.parse_args()
    res = classify(a.cond, a.timeout, a.func)
    from collections import Counter
    cnt = Counter(lab for _, _, lab, _ in res)
    kg  = cnt.get("KG_confirmed", 0) + cnt.get("KG_no_bounds", 0)
    aoc = cnt.get("AOC_confirmed", 0)
    tot = len(res)
    print(f"{a.cond}: {dict(cnt)}")
    print(f"  KG={kg}  AOC={aoc}  total={tot}  KG%={100*kg/tot if tot else 0:.1f}")
    outp = S.EVAL / f"b1_relax_s2n_{a.cond}.json"
    json.dump([{"func": f, "mutant": m, "label": l, "n_relaxed": n} for f, m, l, n in res],
              open(outp, "w"), indent=2)
    print(f"wrote {outp}")
