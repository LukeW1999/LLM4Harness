#!/usr/bin/env python3
"""A_gptoss120b catch rate on 6.4.0 over the GT-fail set, distinguishing COMPILE_ERROR
so we can report catch restricted to compilable mutants (paper line 297)."""
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
COND="A_gptoss120b"
GTF=[(v["func"],v["mutant"]) for v in json.load(open(EXP/"evaluation/gtfail_640.json"))["verdicts"] if v["gt640"]=="FAIL"]
def llm(func,mutant):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    idx=rmo.get_mutated_source_idx(func)
    h=rmo.get_final_harness(EXP/f"results/feedback_loop_{COND}"/func)
    mc=MUT/func/f"{mutant}.c"
    if idx is None or not h or not mc.exists(): return "NOHARNESS"
    ps=[str(p) for p in cfg["project_sources"]]; ps[idx]=str(mc)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(h)]+ps)
    try: r=subprocess.run(cmd,capture_output=True,text=True,timeout=240)
    except subprocess.TimeoutExpired: return "TIMEOUT"
    o=r.stdout+r.stderr
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    if "VERIFICATION FAILED" in o: return "FAIL"
    if "PARSING ERROR" in o or "CONVERSION ERROR" in o or "error:" in o or r.returncode not in (0,10): return "COMPILE_ERROR"
    return "UNKNOWN"
def work(t): f,m=t; return (f,m,llm(f,m))
def main():
    t0=time.time(); out=[]
    with ProcessPoolExecutor(max_workers=16) as ex:
        futs=[ex.submit(work,t) for t in GTF]
        for fu in as_completed(futs): out.append(fu.result())
    N=len(GTF)
    fail=sum(1 for _,_,v in out if v=="FAIL")
    ce=sum(1 for _,_,v in out if v=="COMPILE_ERROR")
    compilable=N-ce
    print(f"GT-fail N={N}  A catch(FAIL)={fail} ({100*fail/N:.1f}%)  compile_error={ce}  compilable={compilable}")
    print(f"catch over compilable only = {fail}/{compilable} = {100*fail/compilable:.1f}%")
if __name__=="__main__": main()
