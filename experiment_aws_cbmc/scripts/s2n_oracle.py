#!/usr/bin/env python3
"""s2n differential mutation oracle. For each (func,mutant): CBMC(GT,m) vs CBMC(LLM,m).
silenced = GT FAIL & LLM SUCCESS. Uses s2n schema (sources/flags), mutant substitutes mutated_name."""
import sys, os, json, subprocess, argparse, glob
from pathlib import Path
sys.path.insert(0,"/root/experiment_aws_cbmc/scripts")
import cbmc_runner as C
PROOF=Path("/root/s2n-tls/tests/cbmc/proofs"); MUT=Path("/root/experiment_aws_cbmc/mutants_s2n")
RES=Path("/root/experiment_aws_cbmc/results"); EVAL=Path("/root/experiment_aws_cbmc/evaluation")

def cbmc_on(func, harness, mutant, mutated_name, timeout=90):
    cfg=C.FUNC_CONFIGS[func]
    srcs=[str(mutant) if Path(s).name==mutated_name else str(s) for s in cfg["sources"]]
    cmd=["cbmc"]+list(cfg["flags"])+list(cfg["unwind"])+(list(cfg["unwindset"]) if cfg.get("unwindset") else [])+["--function",cfg["harness_entry"],str(harness)]+srcs
    try:
        o=subprocess.run(cmd,capture_output=True,text=True,timeout=timeout); out=o.stdout+o.stderr
    except subprocess.TimeoutExpired: return "TIMEOUT"
    if any(b in out for b in ("PARSING ERROR","CONVERSION ERROR","Invalid User Input","fatal error")): return "COMPILE_ERROR"
    if "VERIFICATION SUCCESSFUL" in out: return "SUCCESS"
    if "VERIFICATION FAILED" in out: return "FAIL"
    return "UNKNOWN"

def gt_harness(func): return PROOF/func/f"{func}_harness.c"
def llm_harness(func, ds):
    hs=sorted(glob.glob(str(RES/ds/func/"iter_*_harness.c")), key=lambda p:int(p.split("iter_")[1].split("_")[0]))
    return Path(hs[-1]) if hs else None

def run(cond, timeout=90, workers=8):
    ds=f"feedback_loop_{cond}"
    funcs=sorted(d.name for d in MUT.iterdir() if d.is_dir() and (MUT/d.name/"metadata.json").exists())
    results=[]
    from concurrent.futures import ThreadPoolExecutor
    tasks=[]
    for f in funcs:
        meta=json.load(open(MUT/f/"metadata.json")); mn=meta.get("mutated_name")
        lh=llm_harness(f,ds); gh=gt_harness(f)
        if not lh or not gh.exists(): continue
        for mc in sorted(glob.glob(str(MUT/f/"mutant_*.c"))):
            tasks.append((f,Path(mc).stem,Path(mc),mn,gh,lh))
    def work(t):
        f,mname,mc,mn,gh,lh=t
        gt=cbmc_on(f,gh,mc,mn,timeout); llm=cbmc_on(f,lh,mc,mn,timeout)
        return {"func":f,"mutant":mname,"gt":gt,"llm":llm,"silenced":(gt in("FAIL","SAT") and llm in("SUCCESS","UNSAT"))}
    print(f"{cond}: {len(tasks)} (func,mutant) tasks over {len(set(t[0] for t in tasks))} funcs", flush=True)
    with ThreadPoolExecutor(max_workers=workers) as ex:
        for i,r in enumerate(ex.map(work,tasks)):
            results.append(r)
            if (i+1)%100==0: print(f"  {i+1}/{len(tasks)} sil so far: {sum(1 for x in results if x['silenced'])}",flush=True)
    gtf=[r for r in results if r["gt"] in("FAIL","SAT")]; sil=[r for r in results if r["silenced"]]
    print(f"{cond}: GT-FAIL={len(gtf)} silenced={len(sil)} ({100*len(sil)/len(gtf) if gtf else 0:.1f}% Sil/GT)")
    json.dump({"summary":{"dataset":ds,"total":len(results),"gtfail":len(gtf),"silenced":len(sil)},"results":results},
              open(EVAL/f"mutation_oracle_s2n_{cond}.json","w"))
    from collections import Counter
    print("silenced by func:", dict(Counter(r["func"] for r in sil)))

if __name__=="__main__":
    ap=argparse.ArgumentParser(); ap.add_argument("--cond",required=True); ap.add_argument("--func"); ap.add_argument("--timeout",type=int,default=90); ap.add_argument("--workers",type=int,default=8)
    a=ap.parse_args()
    if a.func:  # smoke test single func
        ds=f"feedback_loop_{a.cond}"; meta=json.load(open(MUT/a.func/"metadata.json")); mn=meta["mutated_name"]
        gh=gt_harness(a.func); lh=llm_harness(a.func,ds)
        print(f"GT harness: {gh.exists()}  LLM harness: {lh}")
        nsil=0
        for mc in sorted(glob.glob(str(MUT/a.func/'mutant_*.c'))):
            gt=cbmc_on(a.func,gh,Path(mc),mn,a.timeout); llm=cbmc_on(a.func,lh,Path(mc),mn,a.timeout)
            s=gt in('FAIL','SAT') and llm in('SUCCESS','UNSAT'); nsil+=s
            print(f"  {Path(mc).stem}: GT={gt} LLM={llm} {'SILENCED' if s else ''}")
        print(f"silenced: {nsil}")
    else:
        run(a.cond,a.timeout,a.workers)
