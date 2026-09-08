#!/usr/bin/env python3
"""6.4.0 silenced counts for the probe conditions I (two-phase scaffold-then-fill)
and J (postcondition checklist), plus G multi-run repeats (r2,r3), over the 6.4.0
GT-fail set (397). Reuses generated harnesses (no LLM). CBMC only."""
import os,shutil
import sys, json, subprocess, time
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
HERE=Path(__file__).resolve().parent; EXP=HERE.parent
sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"
CONDS=["I_gptoss120b","J_gptoss120b","G_gptoss120b_r2","G_gptoss120b_r3"]
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
def work(t):
    cond,func,mut=t; return (cond,func,mut,llm(cond,func,mut))
def main():
    t0=time.time()
    avail=[c for c in CONDS if (EXP/f"results/feedback_loop_{c}").exists()]
    print(f"conds available: {avail}",flush=True)
    tasks=[(c,f,m) for c in avail for (f,m) in GTF]
    out=[]
    with ProcessPoolExecutor(max_workers=16) as ex:
        futs=[ex.submit(work,t) for t in tasks]; n=0
        for fu in as_completed(futs):
            out.append(fu.result()); n+=1
            if n%400==0: print(f"  {n}/{len(tasks)} ({time.time()-t0:.0f}s)",flush=True)
    N=len(GTF); rows={}
    for c in avail:
        vs=[v for (cc,_,_,v) in out if cc==c]
        sil=sum(1 for v in vs if v=="SUCCESS")
        rows[c]={"silenced_640":sil,"SilGT_pct":round(100*sil/N,1)}
    res={"cbmc":"6.4.0","gtfail_denom":N,"elapsed_s":round(time.time()-t0),"per_condition":rows,
         "note":"J=checklist(paper 52 on 5.95), I=two-phase, G r2/r3 multi-run(paper 40,43 on 5.95)"}
    json.dump({"summary":res,"verdicts":[{"cond":c,"func":f,"mutant":m,"llm640":v} for (c,f,m,v) in out]},
              open(EXP/"evaluation/probe_gmr_640.json","w"),indent=1)
    print("\n=== PROBE (I/J) + G MULTI-RUN @6.4.0 (denom 397) ===")
    for c in avail: print(f"  {c:22} silenced={rows[c]['silenced_640']:>4}  Sil/GT={rows[c]['SilGT_pct']}%")
if __name__=="__main__": main()
