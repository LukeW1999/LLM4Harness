#!/usr/bin/env python3
"""Recompute the GT-fail set on CBMC 6.4.0 (AWS aws-c-common CI's pinned version),
paper methodology (built-in checks off). GT-only, reuses AWS expert harnesses. No LLM."""
import os,shutil
import sys, json, subprocess, time
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

# CBMC is memory-hungry: one process can hold a gigabyte, so a fixed worker
# count that fits one machine will OOM another. Default to cores-1 and let
# CBMC_WORKERS override.
def _workers():
    import os as _os
    return int(_os.environ.get("CBMC_WORKERS") or max(1, (_os.cpu_count() or 8) - 1))
HERE=Path(__file__).resolve().parent; EXP=HERE.parent
sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]
MUT=EXP/"mutants"
STORE=json.load(open(EXP/"evaluation/mutation_oracle_cbmc_feedback_loop_A_gptoss120b.json"))["results"]
def gt(func,mutant):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    idx=rmo.get_mutated_source_idx(func)
    gt_h=rmo.GT_PROOFS_DIR/func/f"{func}_harness.c"
    mc=MUT/func/f"{mutant}.c"
    if idx is None or not gt_h.exists() or not mc.exists(): return "MISSING"
    ps=[str(p) for p in cfg["project_sources"]]; ps[idx]=str(mc)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(gt_h)]+ps)
    try: o=subprocess.run(cmd,capture_output=True,text=True,timeout=240).stdout
    except subprocess.TimeoutExpired: return "TIMEOUT"
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    if "VERIFICATION FAILED" in o: return "FAIL"
    return "UNKNOWN"
def work(r): return (r["func"],r["mutant"],gt(r["func"],r["mutant"]),r["gt"])
def main():
    t0=time.time(); out=[]
    with ProcessPoolExecutor(max_workers=_workers()) as ex:
        futs=[ex.submit(work,r) for r in STORE]; n=0
        for f in as_completed(futs):
            out.append(f.result()); n+=1
            if n%150==0: print(f"  {n}/{len(STORE)} ({time.time()-t0:.0f}s)",flush=True)
    fail640=sum(1 for _,_,g,_ in out if g=="FAIL")
    fail595=sum(1 for r in STORE if r["gt"]=="FAIL")
    # transitions
    import collections
    trans=collections.Counter((r_gt,g640) for _,_,g640,r_gt in out)
    set595={(f,m) for f,m,g,rg in out if rg=="FAIL"}
    set640={(f,m) for f,m,g,rg in out if g=="FAIL"}
    res={"cbmc":"6.4.0","config":"checks off (--no-standard-checks --no-unwinding-assertions)",
         "gtfail_595":fail595,"gtfail_640":fail640,
         "both_fail":len(set595&set640),"only595":len(set595-set640),"only640":len(set640-set595),
         "transitions_595_to_640":{f"{a}->{b}":c for (a,b),c in sorted(trans.items())},
         "elapsed_s":round(time.time()-t0)}
    json.dump({"summary":res,"verdicts":[{"func":f,"mutant":m,"gt640":g,"gt595":rg} for f,m,g,rg in out]},
              open(EXP/"evaluation/gtfail_640.json","w"),indent=1)
    print("\n=== GT-FAIL @ 6.4.0 vs 5.95.1 ==="); [print(f"  {k}: {v}") for k,v in res.items()]
if __name__=="__main__": main()
