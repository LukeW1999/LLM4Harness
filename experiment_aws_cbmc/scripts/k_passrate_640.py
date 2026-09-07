import sys,json,subprocess,time,os,shutil
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor,as_completed
HERE=Path(__file__).resolve().parent; EXP=HERE.parent; sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS,COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; COND="K_gptoss120b"
def verify(func):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg or "project_sources" not in cfg: return None
    h=rmo.get_final_harness(EXP/f"results/feedback_loop_{COND}"/func)
    if not h: return None
    ps=[str(p) for p in cfg["project_sources"]]
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(h)]+ps)
    try:o=subprocess.run(cmd,capture_output=True,text=True,timeout=240).stdout
    except subprocess.TimeoutExpired:return "TIMEOUT"
    return "SUCCESS" if "VERIFICATION SUCCESSFUL" in o else ("FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
cdir=EXP/f"results/feedback_loop_{COND}"
funcs=[f for f in FUNC_CONFIGS if (cdir/f).is_dir() and rmo.get_final_harness(cdir/f)]
t0=time.time()
with ProcessPoolExecutor(max_workers=16) as ex:
    res=list(ex.map(verify,funcs))
N=sum(1 for r in res if r is not None); succ=res.count("SUCCESS")
out={"cond":"K_gptoss120b","N":N,"pass":succ,"pass_pct_640":round(100*succ/N,1),"paper_pass_pct":81.9,"elapsed_s":round(time.time()-t0)}
json.dump(out,open(EXP/"evaluation/k_passrate_640.json","w"),indent=1)
print(json.dumps(out))
