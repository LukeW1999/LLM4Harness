import sys,json,subprocess,time,os,shutil
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor,as_completed
HERE=Path(__file__).resolve().parent; EXP=HERE.parent; sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS,COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"; COND="A_claude"
ALL=[(v["func"],v["mutant"],v["gt640"]) for v in json.load(open(EXP/"evaluation/gtfail_640.json"))["verdicts"]]
def llm(func,mut):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    idx=rmo.get_mutated_source_idx(func); h=rmo.get_final_harness(EXP/f"results/feedback_loop_{COND}"/func); mc=MUT/func/f"{mut}.c"
    if idx is None or not h or not mc.exists(): return "NOHARNESS"
    ps=[str(p) for p in cfg["project_sources"]]; ps[idx]=str(mc)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(h)]+ps)
    try:o=subprocess.run(cmd,capture_output=True,text=True,timeout=200).stdout
    except subprocess.TimeoutExpired:return "TIMEOUT"
    return "SUCCESS" if "VERIFICATION SUCCESSFUL" in o else ("FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
def work(t):f,m,g=t;return(f,m,g,llm(f,m))
t0=time.time()
with ProcessPoolExecutor(max_workers=16) as ex:
    out=list(ex.map(work,ALL))
# reverse cell = GT SUCCESS & LLM FAIL
rev=[(f,m) for f,m,g,l in out if g=="SUCCESS" and l=="FAIL"]
import collections
res={"cond":COND,"reverse_cell_raw":len(rev),"reverse_funcs":len(set(f for f,m in rev)),
     "paper_5.95_reverse_raw":81,"elapsed_s":round(time.time()-t0),
     "note":"raw GT-success&LLM-fail; paper 81 is after assume-envelope>=HGT + behavioural filters"}
json.dump({"summary":res,"reverse":sorted(rev)},open(EXP/"evaluation/reverse_640.json","w"),indent=1)
print(json.dumps(res))
