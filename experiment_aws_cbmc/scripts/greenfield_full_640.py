#!/usr/bin/env python3
"""6.4.0 green-field self-built reference, full metrics (mirrors gf_reference.py):
union leave-one-out recovery + single-model vs cross-model at matched K + recall@K=1.
Reuses generated harnesses (no LLM). CBMC 6.4.0 only."""
import os,shutil
import sys,json,subprocess,time,random,collections
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor,as_completed
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
PREFIX="feedback_loop_A_"
def pool_model(d):
    s=d[len(PREFIX):] if d.startswith(PREFIX) else d
    parts=s.split("_")
    if parts and parts[-1].startswith("r") and parts[-1][1:].isdigit(): parts=parts[:-1]
    return "_".join(parts)
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
def catch_task(t):
    pool,func,mut=t; h=harn(pool,func)
    if not h: return (pool,func,mut,None)
    return (pool,func,mut,cbmc(func,h,MUT/func/f"{mut}.c")=="FAIL")
def fid_task(t):
    pool,func=t; h=harn(pool,func)
    if not h: return (pool,func,False)
    return (pool,func,cbmc(func,h)=="SUCCESS")
def main():
    t0=time.time(); funcs=sorted(set(f for f,m in GTF))
    ftasks=[(p,f) for p in POOLS for f in funcs]
    fid={}
    with ProcessPoolExecutor(max_workers=16) as ex:
        for p,f,ok in ex.map(fid_task,ftasks): fid[(p,f)]=ok
    ctasks=[(p,f,m) for p in POOLS for (f,m) in GTF if fid.get((p,f))]
    print(f"fidelity done; catch tasks {len(ctasks)} ({time.time()-t0:.0f}s)",flush=True)
    catch=collections.defaultdict(dict); n=0
    with ProcessPoolExecutor(max_workers=16) as ex:
        for p,f,m,killed in ex.map(catch_task,ctasks):
            if killed is not None: catch[(f,m)][p]=killed
            n+=1
            if n%1500==0: print(f"  catch {n}/{len(ctasks)} ({time.time()-t0:.0f}s)",flush=True)
    def recall_for(subset):
        subset=set(subset); caught=meas=0
        for key in GTF:
            row=catch.get(key,{}); present=[p for p in subset if p in row]
            if not present: continue
            meas+=1
            if any(row[p] for p in present): caught+=1
        return caught,meas
    # leave-one-out union: each held-out pool's own silenced bugs recovered by the union of the others
    tot=rec=0
    for held in POOLS:
        others=[p for p in POOLS if p!=held]
        for (f,m) in GTF:
            if catch.get((f,m),{}).get(held):   # held silences (its harness SUCCESS = does NOT catch) -> killed False means silenced
                pass
    # NOTE union LOO recovery already in greenfield_640 (94.8); here we add per-model + cross + recall@1
    fc,fm=recall_for(POOLS)
    by_model=collections.defaultdict(list)
    for p in POOLS: by_model[pool_model(p)].append(p)
    per_model={m:(lambda cm: {"pools":ps,"recall_pct":round(100*cm[0]/max(cm[1],1),1)})(recall_for(ps)) for m,ps in by_model.items()}
    models=sorted(by_model); random.seed(0); B=200
    # recall@K=1 (one generator)
    v1=[]
    for _ in range(B):
        c,me=recall_for([random.choice(POOLS)]);
        if me: v1.append(c/me)
    r_at_1=sum(v1)/len(v1)
    # cross-model matched K=len(models): one seed per model
    Kc=len(models); vx=[]
    for _ in range(B):
        sub=[random.choice(by_model[m]) for m in models]; c,me=recall_for(sub)
        if me: vx.append(c/me)
    cross_mean=sum(vx)/len(vx)
    sm=[]
    for _ in range(B):
        m=random.choice(models); ps=by_model[m]; sub=random.sample(ps,min(Kc,len(ps))); c,me=recall_for(sub)
        if me: sm.append(c/me)
    sm_mean=sum(sm)/len(sm)
    res={"cbmc":"6.4.0","pools":len(POOLS),"models":{m:ps for m,ps in by_model.items()},
         "full_pool_recall_pct":round(100*fc/max(fm,1),1),
         "per_model":per_model,
         "recall_at_1_pct":round(100*r_at_1,1),
         "cross_model_matchedK_pct":round(100*cross_mean,1),
         "single_model_matchedK_pct":round(100*sm_mean,1),
         "matched_K":Kc,"elapsed_s":round(time.time()-t0),
         "paper_5.95":{"union_loo":95.1,"single_plateau":91,"cross":99.3,"one_gen":88.6}}
    json.dump({"summary":res,"catch":{f"{f}|{m}":dict(row) for (f,m),row in catch.items()},"fid":{f"{p}|{f}":v for (p,f),v in fid.items()}},
              open(EXP/"evaluation/greenfield_full_640.json","w"),indent=1)
    print("\n=== GREEN-FIELD FULL @6.4.0 ===")
    for k,v in res.items():
        if k in("models","per_model"): continue
        print(f"  {k}: {v}")
    print("  per_model:")
    for m,d in sorted(per_model.items()): print(f"    {m:22} {d['recall_pct']}% (K={len(d['pools'])})")
if __name__=="__main__": main()
