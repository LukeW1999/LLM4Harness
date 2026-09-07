import os,shutil
#!/usr/bin/env python3,shutil
"""Recompute silenced set on CBMC 6.4.0 for all 8 conditions, over the 6.4.0 GT-fail
set (397). Reuses the already-generated H_LLM harnesses (NO LLM/API). CBMC only."""
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
CONDS=["A_gptoss120b","H_gptoss120b","M_gptoss120b","G_gptoss120b","Oracle_gptoss120b","A_claude","H_claude","M_claude"]
OLD={"A_gptoss120b":41,"H_gptoss120b":37,"M_gptoss120b":30,"G_gptoss120b":1,"Oracle_gptoss120b":158,"A_claude":16,"H_claude":16,"M_claude":11}
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
    t0=time.time(); tasks=[(c,f,m) for c in CONDS for (f,m) in GTF]
    print(f"total tasks: {len(tasks)} ({len(CONDS)} conds x {len(GTF)} GT-fail mutants)",flush=True)
    out=[]
    with ProcessPoolExecutor(max_workers=16) as ex:
        futs=[ex.submit(work,t) for t in tasks]; n=0
        for fu in as_completed(futs):
            out.append(fu.result()); n+=1
            if n%400==0: print(f"  {n}/{len(tasks)} ({time.time()-t0:.0f}s)",flush=True)
    N=len(GTF)  # 397
    rows={}
    for c in CONDS:
        vs=[v for (cc,_,_,v) in out if cc==c]
        sil=sum(1 for v in vs if v=="SUCCESS"); cat=sum(1 for v in vs if v=="FAIL")
        rows[c]={"silenced_640":sil,"silenced_595":OLD[c],"catch_640":cat,
                 "SilGT_640_pct":round(100*sil/N,1),"CatchGT_640_pct":round(100*cat/N,1)}
    res={"cbmc":"6.4.0","gtfail_denom_640":N,"gtfail_denom_595":370,"elapsed_s":round(time.time()-t0),"per_condition":rows}
    json.dump({"summary":res,"verdicts":[{"cond":c,"func":f,"mutant":m,"llm640":v} for (c,f,m,v) in out]},
              open(EXP/"evaluation/silenced_640.json","w"),indent=1)
    print("\n=== SILENCED @ 6.4.0 (denom 397) vs 5.95.1 (denom 370) ===")
    print(f"{'cond':20} {'sil640':>7} {'sil595':>7} {'Sil/GT640%':>11} {'Catch640%':>10}")
    for c in CONDS:
        r=rows[c]; print(f"{c:20} {r['silenced_640']:>7} {r['silenced_595']:>7} {r['SilGT_640_pct']:>11} {r['CatchGT_640_pct']:>10}")
if __name__=="__main__": main()
