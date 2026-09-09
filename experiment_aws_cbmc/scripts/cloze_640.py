from pathlib import Path
import sys,json,subprocess,tempfile,re,collections,os,shutil
HERE=Path(__file__).resolve().parent; EXP=HERE.parent; sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS,COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"
GTP=rmo.GT_PROOFS_DIR
def norm(s): return re.sub(r'\s+','',s)
def blank(gt,target):
    tn=norm(target); out=[]; done=False
    for ln in gt.splitlines(keepends=True):
        if not done and 'assert' in ln and norm(ln.strip())==tn:
            done=True; out.append(re.match(r'\s*',ln).group(0)+"__CLOZE__\n"); continue
        out.append(ln)
    return "".join(out),done
def cbmc(func,htext,mutant=None):
    cfg=FUNC_CONFIGS.get(func); idx=rmo.get_mutated_source_idx(func)
    if not cfg or idx is None: return "NOCFG"
    ps=[str(p) for p in cfg["project_sources"]]
    if mutant: ps[idx]=str(mutant)
    tf=tempfile.NamedTemporaryFile("w",suffix="_harness.c",delete=False,dir="/tmp"); tf.write(htext); tf.close()
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[tf.name]+ps)
    try:o=subprocess.run(cmd,capture_output=True,text=True,timeout=150).stdout
    except subprocess.TimeoutExpired:return "TIMEOUT"
    return "SUCCESS" if "VERIFICATION SUCCESSFUL" in o else ("FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
# silenced mutants per (condition) func from 6.4.0 silenced set
sil=json.load(open(EXP/"evaluation/silenced_640.json"))["verdicts"]
def sils(cond,func): return [v["mutant"] for v in sil if v["cond"]==cond and v["func"]==func and v["llm640"]=="SUCCESS"]
FILES=[("cloze_A_gptoss120b_openrouter.json","A_gptoss120b"),
       ("cloze_A_gptoss120b_claude.json","A_gptoss120b"),
       ("cloze_A_claude_claude.json","A_claude")]
total=0; recovered=0; dead=0; detail=[]
for fn,cond in FILES:
    p=EXP/"evaluation"/fn
    if not p.exists(): continue
    for e in json.load(open(p)):
        if e.get("status")!="OK": continue
        func=e["func"]; gtp=Path(str(GTP))/func/f"{func}_harness.c"
        if not gtp.exists(): continue
        bl,ok=blank(gtp.read_text(errors='replace'), e["assert"])
        if not ok: continue
        filled=bl.replace("__CLOZE__", e["filled"])
        sm=sils(cond,func)
        # A fill can only be scored where the condition still has a silenced
        # mutant for it to catch; under 6.4.0 aws_nospec_mask has none, so those
        # attempts are dead rather than failed.
        if not sm:
            dead+=1; continue
        orig=cbmc(func,filled); catches=sum(1 for m in sm if cbmc(func,filled,MUT/func/f"{m}.c")=="FAIL")
        ok_fill=(orig=="SUCCESS" and catches>=1)
        total+=1; recovered+=1 if ok_fill else 0
        detail.append({"file":fn,"func":func,"orig":orig,"catches":catches,"n_sil":len(sm),"recovered":ok_fill})
res={"cbmc":"6.4.0","total_fills":total,"recovered":recovered,"dead_no_silenced_mutant":dead,"paper":"27/28","detail":detail}
json.dump(res,open(EXP/"evaluation/cloze_640.json","w"),indent=1)
print(f"cloze @ 6.4.0: {recovered}/{total} recovered, {dead} dead (paper 27/28)")
