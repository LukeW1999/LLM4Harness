from pathlib import Path
import sys,subprocess,glob,json,time,os,shutil
from concurrent.futures import ProcessPoolExecutor,as_completed

# CBMC is memory-hungry: one process can hold a gigabyte, so a fixed worker
# count that fits one machine will OOM another. Default to cores-1 and let
# CBMC_WORKERS override.
def _workers():
    import os as _os
    return int(_os.environ.get("CBMC_WORKERS") or max(1, (_os.cpu_count() or 8) - 1))
sys.path.insert(0,"scripts"); import cbmc_runner as C
EXP=Path("/home/weiqi/research/projects/LLM4Harness/experiment_aws_cbmc")
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
S2N="/home/weiqi/research/projects/LLM4Harness/study_derivability/corpora/s2n-tls"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]
def remap(x): return str(x).replace("/root/s2n-tls",S2N)
def run(func,harness,mc,mn):
    cfg=C.FUNC_CONFIGS.get(func)
    if not cfg or "sources" not in cfg: return "NOCFG"
    srcs=[mc if Path(s).name==mn else remap(s) for s in cfg["sources"]]
    cmd=[CBMC]+[remap(f) for f in cfg["flags"]]+list(cfg["unwind"])+(list(cfg["unwindset"]) if cfg.get("unwindset") else [])+MATCH+["--function",cfg["harness_entry"],harness]+srcs
    try:o=subprocess.run(cmd,capture_output=True,text=True,timeout=150).stdout
    except subprocess.TimeoutExpired:return "TIMEOUT"
    if "VERIFICATION FAILED" in o: return "FAIL"
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    return "UNKNOWN"
def final_llm(cond,func):
    hs=sorted(glob.glob(str(EXP/f"results/feedback_loop_{cond}/{func}/iter_*_harness.c")),key=lambda p:int(p.split('iter_')[1].split('_')[0]))
    return hs[-1] if hs else None
FUNCS=sorted(p.name for p in (EXP/"mutants_s2n").iterdir() if p.is_dir())
# The GT harness is re-run per condition, so cost is linear in this list.
CONDS=(os.environ.get("S2N_CONDS") or "A_claude,A_gptoss120b").split(",")
def task_list():
    T=[]
    for func in FUNCS:
        mn=json.load(open(EXP/f"mutants_s2n/{func}/metadata.json")).get("mutated_name")
        gt=f"{S2N}/tests/cbmc/proofs/{func}/{func}_harness.c"
        if not Path(gt).exists(): continue
        muts=sorted(glob.glob(str(EXP/f"mutants_s2n/{func}/mutant_*.c")))
        for cond in CONDS:
            llm=final_llm(cond,func)
            if not llm: continue
            for mc in muts: T.append((cond,func,mc,mn,gt,llm))
    return T
def work(t):
    cond,func,mc,mn,gt,llm=t
    return (cond,func,Path(mc).name,run(func,gt,mc,mn),run(func,llm,mc,mn))
def main():
    t0=time.time(); T=task_list()
    print(f"tasks: {len(T)} ({len(FUNCS)} funcs x {len(CONDS)} conds x mutants)",flush=True)
    out=[]
    with ProcessPoolExecutor(max_workers=_workers()) as ex:
        futs=[ex.submit(work,t) for t in T]; n=0
        for f in as_completed(futs):
            out.append(f.result()); n+=1
            if n%300==0: print(f"  {n}/{len(T)} ({time.time()-t0:.0f}s)",flush=True)
    res={}
    for cond in CONDS:
        rows=[(f,m,g,l) for c,f,m,g,l in out if c==cond]
        gtf=set((f,m) for f,m,g,l in rows if g=="FAIL")
        sil=[(f,m) for f,m,g,l in rows if g=="FAIL" and l=="SUCCESS"]
        import collections
        spread=collections.Counter(f for f,m in sil)
        res[cond]={"gtfail":len(gtf),"silenced":len(sil),"funcs_with_silence":len(spread),
                   "max_func_share_pct":round(100*max(spread.values())/len(sil),0) if sil else 0}
    res["paper_5.95"]={"gtfail":253,"claude_sil":57,"gptoss_sil":42}
    res["elapsed_s"]=round(time.time()-t0)
    json.dump({"summary":res,"rows":[{"cond":c,"func":f,"mutant":m,"gt":g,"llm":l} for c,f,m,g,l in out]},
              open(EXP/"evaluation/s2n_640.json","w"),indent=1)
    print("\n=== s2n @ 6.4.0 ==="); print(json.dumps(res,indent=1))
if __name__=="__main__": main()
