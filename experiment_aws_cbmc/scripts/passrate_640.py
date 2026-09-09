#!/usr/bin/env python3
"""Recompute RQ1 pass rate on CBMC 6.4.0: run each condition's final H_LLM on the
ORIGINAL (unmutated) function; SUCCESS = accepted. Reuses stored harnesses (no LLM)."""
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
CONDS=(sys.argv[sys.argv.index("--conds")+1].split(",") if "--conds" in sys.argv else ["A_gptoss120b","H_gptoss120b","M_gptoss120b","G_gptoss120b","Oracle_gptoss120b","A_claude","H_claude","M_claude"])
def verify(cond,func):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg or "project_sources" not in cfg: return None
    h=rmo.get_final_harness(EXP/f"results/feedback_loop_{cond}"/func)
    if not h: return None
    ps=[str(p) for p in cfg["project_sources"]]  # ORIGINAL, no mutant
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(h)]+ps)
    try: o=subprocess.run(cmd,capture_output=True,text=True,timeout=240).stdout
    except subprocess.TimeoutExpired: return "TIMEOUT"
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    if "VERIFICATION FAILED" in o: return "FAIL"
    if any(b in o for b in ("PARSING ERROR","CONVERSION ERROR","fatal error")): return "COMPILE"
    return "UNKNOWN"
def work(t):
    cond,func=t; return (cond,func,verify(cond,func))
def main():
    t0=time.time()
    tasks=[]
    for c in CONDS:
        cdir=EXP/f"results/feedback_loop_{c}"
        for func in FUNC_CONFIGS:
            if (cdir/func).is_dir() and rmo.get_final_harness(cdir/func):
                tasks.append((c,func))
    print(f"tasks: {len(tasks)}",flush=True)
    out=[]
    with ProcessPoolExecutor(max_workers=_workers()) as ex:
        futs=[ex.submit(work,t) for t in tasks]; n=0
        for f in as_completed(futs):
            out.append(f.result()); n+=1
            if n%150==0: print(f"  {n}/{len(tasks)} ({time.time()-t0:.0f}s)",flush=True)
    PAPER={"A_gptoss120b":28.9,"Oracle_gptoss120b":84.3}  # primary-run values in paper
    rows={}
    for c in CONDS:
        vs=[v for cc,_,v in out if cc==c and v is not None]
        N=len(vs); succ=sum(1 for v in vs if v=="SUCCESS")
        rows[c]={"N":N,"pass_640":succ,"pass_pct_640":round(100*succ/N,1) if N else None,
                 "paper_primary_pct":PAPER.get(c)}
    json.dump({"cbmc":"6.4.0","per_condition":rows,"elapsed_s":round(time.time()-t0),
               "verdicts":[{"cond":c,"func":f,"v":v} for c,f,v in out]},
              open(EXP/(sys.argv[sys.argv.index("--out")+1] if "--out" in sys.argv else "evaluation/passrate_640.json"),"w"),indent=1)
    print("\n=== PASS RATE @ 6.4.0 (reused primary-run harnesses) ===")
    print(f"{'cond':20} {'N':>4} {'pass':>5} {'pass%_640':>10} {'paper_primary%':>14}")
    for c in CONDS:
        r=rows[c]; print(f"{c:20} {r['N']:>4} {r['pass_640']:>5} {str(r['pass_pct_640']):>10} {str(r['paper_primary_pct']):>14}")
if __name__=="__main__": main()
