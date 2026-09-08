from pathlib import Path
import sys,json,subprocess,time,collections,os,shutil
from concurrent.futures import ProcessPoolExecutor,as_completed

# CBMC is memory-hungry: one process can hold a gigabyte, so a fixed worker
# count that fits one machine will OOM another. Default to cores-1 and let
# CBMC_WORKERS override.
def _workers():
    import os as _os
    return int(_os.environ.get("CBMC_WORKERS") or max(1, (_os.cpu_count() or 8) - 1))
HERE=Path(__file__).resolve().parent; EXP=HERE.parent; sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS,COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"
POOLS=["feedback_loop_A_claude","feedback_loop_A_claude_r2","feedback_loop_A_claude_r3","feedback_loop_A_claude_r4","feedback_loop_A_claude_r5",
       "feedback_loop_A_deepseekv4flash","feedback_loop_A_deepseekv4pro","feedback_loop_A_gpt55","feedback_loop_A_gptoss120b",
       "feedback_loop_A_gptoss120b_r2","feedback_loop_A_gptoss120b_r3","feedback_loop_A_llama3370binstruct","feedback_loop_A_qwen37max","feedback_loop_A_qwen37plus"]
GTF=[(v["func"],v["mutant"]) for v in json.load(open(EXP/"evaluation/gtfail_640.json"))["verdicts"] if v["gt640"]=="FAIL"]
def cbmc(func,harness,mutant=None):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    ps=[str(p) for p in cfg["project_sources"]]
    if mutant: idx=rmo.get_mutated_source_idx(func); ps[idx]=str(mutant)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(harness)]+ps)
    try:o=subprocess.run(cmd,capture_output=True,text=True,timeout=180).stdout
    except subprocess.TimeoutExpired:return "TIMEOUT"
    return "SUCCESS" if "VERIFICATION SUCCESSFUL" in o else ("FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
def harn(pool,func): return rmo.get_final_harness(EXP/f"results/{pool}"/func)
# tasks: for each pool, each GT-fail mutant -> LLM verdict; + fidelity per (pool,func)
funcs=sorted(set(f for f,m in GTF))
def catch_task(t):
    pool,func,mut=t; h=harn(pool,func)
    if not h: return (pool,func,mut,"NOHARNESS")
    return (pool,func,mut,cbmc(func,h,MUT/func/f"{mut}.c"))
def fid_task(t):
    pool,func=t; h=harn(pool,func)
    if not h: return (pool,func,"NOHARNESS")
    return (pool,func,cbmc(func,h))
def main():
    t0=time.time()
    ctasks=[(p,f,m) for p in POOLS for (f,m) in GTF]
    ftasks=[(p,f) for p in POOLS for f in funcs]
    catch={}; fid={}
    with ProcessPoolExecutor(max_workers=_workers()) as ex:
        n=0
        for p,f,m,v in ex.map(catch_task,ctasks):
            catch[(p,f,m)]=v; n+=1
            if n%1000==0: print(f"  catch {n}/{len(ctasks)} ({time.time()-t0:.0f}s)",flush=True)
        for p,f,v in ex.map(fid_task,ftasks): fid[(p,f)]=v
    def qualifies(p,f): return fid.get((p,f))=="SUCCESS"
    # each pool's own silenced bugs: GT-fail mutant where pool's harness=SUCCESS(caught? no -> silenced) i.e. verdict SUCCESS
    tot=0; rec=0
    for held in POOLS:
        for (f,m) in GTF:
            if catch.get((held,f,m))=="SUCCESS":  # held silences it
                tot+=1
                # union of OTHER qualifying harnesses catches it?
                if any(qualifies(p,f) and catch.get((p,f,m))=="FAIL" for p in POOLS if p!=held):
                    rec+=1
    res={"cbmc":"6.4.0","pools":len(POOLS),"held_out_silenced_total":tot,"recovered":rec,
         "recovery_pct":round(100*rec/tot,1) if tot else None,"paper_5.95":95.1,"elapsed_s":round(time.time()-t0)}
    json.dump(res,open(EXP/"evaluation/greenfield_640.json","w"),indent=1)
    print(json.dumps(res))
if __name__=="__main__": main()
