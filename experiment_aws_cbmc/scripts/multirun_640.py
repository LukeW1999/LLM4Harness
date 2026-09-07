import sys,json,subprocess,time,os,shutil
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor,as_completed
HERE=Path(__file__).resolve().parent; EXP=HERE.parent; sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS,COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"
GTF=[(v["func"],v["mutant"]) for v in json.load(open(EXP/"evaluation/gtfail_640.json"))["verdicts"] if v["gt640"]=="FAIL"]
CONDS=["A_gptoss120b_r2","A_gptoss120b_r3"]
def llm(cond,func,mut):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    idx=rmo.get_mutated_source_idx(func); h=rmo.get_final_harness(EXP/f"results/feedback_loop_{cond}"/func); mc=MUT/func/f"{mut}.c"
    if idx is None or not h or not mc.exists(): return "NOHARNESS"
    ps=[str(p) for p in cfg["project_sources"]]; ps[idx]=str(mc)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(h)]+ps)
    try:o=subprocess.run(cmd,capture_output=True,text=True,timeout=200).stdout
    except subprocess.TimeoutExpired:return "TIMEOUT"
    return "SUCCESS" if "VERIFICATION SUCCESSFUL" in o else ("FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
def work(t):c,f,m=t;return(c,llm(c,f,m))
t0=time.time(); tasks=[(c,f,m) for c in CONDS for f,m in GTF]
out=[]
with ProcessPoolExecutor(max_workers=16) as ex:
    for r in ex.map(work,tasks): out.append(r)
res={}
for c in CONDS: res[c]=sum(1 for cc,v in out if cc==c and v=="SUCCESS")
res["_paper_5.95"]={"A_r2":34,"A_r3":36,"A_primary":41}; res["elapsed_s"]=round(time.time()-t0)
json.dump(res,open(EXP/"evaluation/multirun_640.json","w"),indent=1)
print(json.dumps(res))
