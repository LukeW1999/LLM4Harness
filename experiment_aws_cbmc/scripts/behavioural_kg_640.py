import os,shutil
#!/usr/bin/env python3,shutil
"""6.4.0 behavioural rename-immune KG check (111/115 on 5.95). For each silenced
bug (llm640==SUCCESS in silenced_640.json), run CBMC 6.4.0 on EVERY LLM iteration
harness: a legitimate catch = iter FAILs on mutant AND SUCCEEDs on original src.
never-written unless some valid iteration ever caught it."""
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
# gpt-oss conditions with feedback-loop iteration histories
CONDS=["A_gptoss120b","H_gptoss120b","M_gptoss120b","G_gptoss120b","Oracle_gptoss120b"]
SIL=json.load(open(EXP/"evaluation/silenced_640.json"))["verdicts"]

def verdict(func,src_path,harness,idx):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    ps=[str(p) for p in cfg["project_sources"]]
    if idx is None or idx>=len(ps): return "NOIDX"
    ps[idx]=str(src_path)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(harness)]+ps)
    try: o=subprocess.run(cmd,capture_output=True,text=True,timeout=240).stdout
    except subprocess.TimeoutExpired: return "TIMEOUT"
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    if "VERIFICATION FAILED" in o: return "FAIL"
    return "UNKNOWN"

def check_one(cond,func,mutant):
    idx=rmo.get_mutated_source_idx(func)
    if idx is None: return (cond,func,mutant,None,0,"no_idx")
    orig=Path(FUNC_CONFIGS[func]["project_sources"][idx])
    mc=MUT/func/f"{mutant}.c"
    d=EXP/f"results/feedback_loop_{cond}"/func
    hs=sorted(d.glob("iter_*_harness.c")) if d.exists() else []
    if not mc.exists() or not hs: return (cond,func,mutant,None,0,"no_hist")
    ever=False
    for h in hs:
        vm=verdict(func,mc,h,idx)
        if vm in ("FAIL","SAT"):
            vo=verdict(func,orig,h,idx)
            if vo in ("SUCCESS","UNSAT"): ever=True; break
    return (cond,func,mutant,ever,len(hs),"ok")

def main():
    t0=time.time()
    tasks=[(v["cond"],v["func"],v["mutant"]) for v in SIL if v["cond"] in CONDS and v["llm640"]=="SUCCESS"]
    print(f"silenced across gpt-oss conds @6.4.0: {len(tasks)}",flush=True)
    out=[]
    with ProcessPoolExecutor(max_workers=14) as ex:
        futs=[ex.submit(check_one,*t) for t in tasks]; n=0
        for fu in as_completed(futs):
            out.append(fu.result()); n+=1
            if n%25==0: print(f"  {n}/{len(tasks)} ({time.time()-t0:.0f}s)",flush=True)
    decided=[r for r in out if r[3] is not None]
    ever=[r for r in decided if r[3]]
    kg=[r for r in decided if not r[3]]
    res={"cbmc":"6.4.0","conds":CONDS,"silenced_total":len(tasks),"re_runnable_decided":len(decided),
         "never_written_behavioural":len(kg),"ever_caught":len(ever),
         "ever_caught_list":[(r[0],r[1],r[2]) for r in ever],
         "pct_never_written_of_decided":round(100*len(kg)/len(decided),1) if decided else None,
         "elapsed_s":round(time.time()-t0)}
    json.dump({"summary":res,"rows":[{"cond":r[0],"func":r[1],"mutant":r[2],"ever_caught":r[3],"n_iter":r[4],"note":r[5]} for r in out]},
              open(EXP/"evaluation/behavioural_kg_640.json","w"),indent=1)
    print("\n=== BEHAVIOURAL KG @6.4.0 ===")
    for k,v in res.items():
        if isinstance(v,list): print(f"  {k}: {len(v)} {v[:6]}")
        else: print(f"  {k}: {v}")
if __name__=="__main__": main()
