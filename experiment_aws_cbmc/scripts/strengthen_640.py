import sys,json,subprocess,glob,os,shutil
from pathlib import Path
HERE=Path(__file__).resolve().parent; EXP=HERE.parent; sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS,COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"
sil=json.load(open(EXP/"evaluation/silenced_640.json"))["verdicts"]
def cbmc(func,harness,mutant=None):
    cfg=FUNC_CONFIGS.get(func)
    if not cfg: return "NOCFG"
    ps=[str(p) for p in cfg["project_sources"]]
    if mutant:
        idx=rmo.get_mutated_source_idx(func); ps[idx]=str(mutant)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(harness)]+ps)
    try:o=subprocess.run(cmd,capture_output=True,text=True,timeout=200).stdout
    except subprocess.TimeoutExpired:return "TIMEOUT"
    return "SUCCESS" if "VERIFICATION SUCCESSFUL" in o else ("FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
for cond,paper in [("A_claude","11/16"),("A_gptoss120b","0/41")]:
    silset=[(v["func"],v["mutant"]) for v in sil if v["cond"]==cond and v["llm640"]=="SUCCESS"]
    caught=0; total=len(silset); nofix=0
    for func,mut in silset:
        rep=EXP/f"results/feedback_loop_{cond}/{func}/repaired_b2_harness.c"
        if not rep.exists(): nofix+=1; continue
        if cbmc(func,rep,MUT/func/f"{mut}.c")=="FAIL": caught+=1
    print(f"{cond}: strengthening recovered {caught}/{total}  (paper {paper}); funcs w/o repair harness among silenced: {nofix}")
