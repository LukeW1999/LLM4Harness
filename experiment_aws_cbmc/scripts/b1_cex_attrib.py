#!/usr/bin/env python3
"""B1: cex-grounded KG vs AOC via assumption-relaxation re-run (CBMC-decided, rename-immune)."""
import sys, re, json, tempfile, argparse
from pathlib import Path
sys.path.insert(0, "/root/experiment_aws_cbmc/scripts")
import run_mutation_oracle_cbmc as O   # run_cbmc_on_mutant, FUNC_CONFIGS, get_mutated_source_idx, get_final_harness
from pathlib import Path as P
EVAL = P("/root/experiment_aws_cbmc/evaluation")
RES  = P("/root/experiment_aws_cbmc/results")
MUT  = P("/root/experiment_aws_cbmc/mutants")

BOUND = re.compile(r'__CPROVER_assume\s*\([^;]*(<=|<|>=|>)\s*[A-Za-z0-9_]+\s*\)\s*;')

def strip_bound_assumes(text):
    out=[]; removed=0
    for ln in text.splitlines(keepends=True):
        if BOUND.search(ln) and 'is_valid' not in ln and 'VALID' not in ln:
            removed+=1; continue
        out.append(ln)
    return "".join(out), removed

def orig_src(func):
    cfg=O.FUNC_CONFIGS.get(func,{}); idx=O.get_mutated_source_idx(func)
    if idx is None: return None,None
    return P(cfg["project_sources"][idx]), idx

def classify(cond, timeout=120):
    ds=f"feedback_loop_{cond}"
    op=EVAL/f"mutation_oracle_cbmc_{ds}.json"
    sil=[r for r in json.load(open(op))["results"] if r.get("silenced")]
    out=[]
    for r in sil:
        func=r["func"]; mut=r["mutant"]
        src,idx=orig_src(func)
        hp=O.get_final_harness(RES/ds/func)
        mutc=MUT/func/f"{mut}.c"
        if not (src and hp and mutc.exists()):
            out.append((func,mut,"NO_FILES",0)); continue
        stripped,nrem=strip_bound_assumes(hp.read_text(errors='replace'))
        if nrem==0:
            out.append((func,mut,"KG_no_bounds",0)); continue   # nothing to relax -> KG
        tf=tempfile.NamedTemporaryFile("w",suffix="_harness.c",delete=False,dir="/tmp")
        tf.write(stripped); tf.close(); thp=P(tf.name)
        r_orig=O.run_cbmc_on_mutant(func, src,  thp, timeout, idx)   # stripped harness on ORIGINAL
        r_mut =O.run_cbmc_on_mutant(func, mutc, thp, timeout, idx)   # stripped harness on MUTANT
        if r_orig=="FAIL":
            lab="INCONCLUSIVE_relax_broke_validity"
        elif r_orig in ("SUCCESS","UNKNOWN") and r_mut=="FAIL":
            lab="AOC_confirmed"
        elif r_orig=="SUCCESS" and r_mut=="SUCCESS":
            lab="KG_confirmed"
        else:
            lab=f"OTHER({r_orig}/{r_mut})"
        out.append((func,mut,lab,nrem))
    return out

if __name__=="__main__":
    ap=argparse.ArgumentParser(); ap.add_argument("--cond",required=True); ap.add_argument("--timeout",type=int,default=120)
    a=ap.parse_args()
    res=classify(a.cond,a.timeout)
    from collections import Counter
    c=Counter(lab for _,_,lab,_ in res)
    print(f"\n===== B1 cex-grounded attribution: {a.cond} (n={len(res)} silenced) =====")
    for func,mut,lab,nrem in res: print(f"  {func:<38} {mut:<14} {lab:<32} (bounds_removed={nrem})")
    print("  --- summary ---")
    for lab,n in c.most_common(): print(f"  {lab:<40} {n}")
    json.dump([{"func":f,"mutant":m,"label":l,"bounds_removed":n} for f,m,l,n in res],
              open(f"/root/experiment_aws_cbmc/evaluation/b1_cexattrib_{a.cond}.json","w"), indent=1)
