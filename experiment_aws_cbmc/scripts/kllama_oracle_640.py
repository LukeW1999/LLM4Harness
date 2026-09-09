#!/usr/bin/env python3
"""6.4.0 oracle Sil/GT for the two under-reported conditions: K (spec-first) and
Llama 3.3 (A/G/H), over the 6.4.0 GT-fail set (397). Reuses generated harnesses."""
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
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"
CONDS=["K_gptoss120b","A_llama3370binstruct","G_llama3370binstruct","H_llama3370binstruct"]
GTF=[(v["func"],v["mutant"]) for v in json.load(open(EXP/"evaluation/gtfail_640.json"))["verdicts"] if v["gt640"]=="FAIL"]
def llm(cond,func,mutant):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    idx=rmo.get_mutated_source_idx(func)
    h=rmo.get_final_harness(EXP/f"results/feedback_loop_{cond}"/func)
    mc=MUT/func/f"{mutant}.c"
    if idx is None or not h or not mc.exists(): return "NOHARNESS"
    ps=[str(p) for p in cfg["project_sources"]]; ps[idx]=str(mc)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(h)]+ps)
    try: o=subprocess.run(cmd,capture_output=True,text=True,timeout=240).stdout
    except subprocess.TimeoutExpired: return "TIMEOUT"
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    if "VERIFICATION FAILED" in o: return "FAIL"
    return "UNKNOWN"
def work(t): c,f,m=t; return (c,f,m,llm(c,f,m))
def main():
    t0=time.time()
    avail=[c for c in CONDS if (EXP/f"results/feedback_loop_{c}").exists()]
    tasks=[(c,f,m) for c in avail for (f,m) in GTF]
    print(f"conds {avail}; tasks {len(tasks)}",flush=True)
    out=[]
    with ProcessPoolExecutor(max_workers=_workers()) as ex:
        futs=[ex.submit(work,t) for t in tasks]; n=0
        for fu in as_completed(futs):
            out.append(fu.result()); n+=1
            if n%400==0: print(f"  {n}/{len(tasks)} ({time.time()-t0:.0f}s)",flush=True)
    N=len(GTF); rows={}
    for c in avail:
        vs=[v for (cc,_,_,v) in out if cc==c]
        sil=sum(1 for v in vs if v=="SUCCESS"); cat=sum(1 for v in vs if v=="FAIL")
        rows[c]={"silenced":sil,"catch":cat,"SilGT_pct":round(100*sil/N,1),"Catch_pct":round(100*cat/N,1),
                 "Unres_pct":round(100*(N-sil-cat)/N,1)}
    json.dump({"gtfail":N,"elapsed_s":round(time.time()-t0),"per_condition":rows,
               "verdicts":[{"cond":c,"func":f,"mutant":m,"llm640":v} for c,f,m,v in out]},
              open(EXP/"evaluation/kllama_oracle_640.json","w"),indent=1)
    print("\n=== K + Llama oracle @6.4.0 (denom 397) ===")
    for c in avail:
        r=rows[c]; print(f"  {c:26} Sil/GT={r['SilGT_pct']}%  Catch={r['Catch_pct']}%  Unres={r['Unres_pct']}%  (n={r['silenced']})")
if __name__=="__main__": main()
