#!/usr/bin/env python3
"""6.4.0 s2n assume-relaxation cross-check (mirrors s2n_b1_relax.py / a3 on 6.4.0).
For each s2n silenced (cond,func,mutant): strip __CPROVER_assume BOUND constraints from
the LLM final harness, re-run CBMC 6.4.0 on ORIGINAL vs MUTANT.
  KG_confirmed  : stripped harness still SUCCESS on mutant (SUCCESS/SUCCESS)  -> knowledge gap
  KG_no_bounds  : no bound assumes to strip                                   -> KG
  AOC_confirmed : stripping bounds makes it FAIL on mutant (was silenced)     -> over-constraint
  INCONCLUSIVE  : stripped harness FAILs on original (relaxation broke validity)"""
import shutil
import sys,re,json,glob,tempfile,time,os
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor,as_completed
sys.path.insert(0,"scripts"); import cbmc_runner as C
EXP=Path("/home/weiqi/research/projects/LLM4Harness/experiment_aws_cbmc")
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
S2N="/home/weiqi/research/projects/LLM4Harness/study_derivability/corpora/s2n-tls"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]
BOUND=re.compile(r'__CPROVER_assume\s*\([^;]*(<=|<|>=|>)\s*[A-Za-z0-9_]+\s*\)\s*;')
def strip_bounds(text):
    out=[];n=0
    for ln in text.splitlines(keepends=True):
        if BOUND.search(ln): n+=1; continue
        out.append(ln)
    return "".join(out),n
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
import subprocess
def final_llm(cond,func):
    hs=sorted(glob.glob(str(EXP/f"results/feedback_loop_{cond}/{func}/iter_*_harness.c")),key=lambda p:int(p.split('iter_')[1].split('_')[0]))
    return hs[-1] if hs else None
def orig_source(func,mn):
    for s in C.FUNC_CONFIGS[func]["sources"]:
        if Path(s).name==mn: return remap(s)
    return None
rows=json.load(open(EXP/"evaluation/s2n_640.json"))["rows"]
def classify(t):
    cond,func,mutant=t
    mn=json.load(open(EXP/f"mutants_s2n/{func}/metadata.json")).get("mutated_name")
    llm=final_llm(cond,func);
    if not llm: return (cond,func,mutant,"NOHARNESS")
    text=open(llm).read(); stripped,nb=strip_bounds(text)
    if nb==0: return (cond,func,mutant,"KG_no_bounds")
    orig=orig_source(func,mn); mc=str(EXP/f"mutants_s2n/{func}/{mutant}")
    if not mc.endswith(".c"): mc+=".c"
    with tempfile.NamedTemporaryFile("w",suffix="_harness.c",delete=False) as tf:
        tf.write(stripped); tmp=tf.name
    try:
        vo=run(func,tmp,orig,mn); vm=run(func,tmp,mc,mn)
    finally:
        os.unlink(tmp)
    if vo not in ("SUCCESS",): return (cond,func,mutant,f"INCONCLUSIVE(orig={vo})")
    if vm=="FAIL": return (cond,func,mutant,"AOC_confirmed")
    return (cond,func,mutant,"KG_confirmed")
def main():
    t0=time.time()
    sil={c:[(f,m) for cc,f,m,g,l in [(r["cond"],r["func"],r["mutant"],r["gt"],r["llm"]) for r in rows] if cc==c and g=="FAIL" and l=="SUCCESS"] for c in ("A_claude","A_gptoss120b")}
    tasks=[(c,f,m) for c in sil for (f,m) in sil[c]]
    print(f"s2n silenced tasks: {{k:len(v) for k,v in sil.items()}} -> {len(tasks)}",flush=True)
    out=[]
    with ProcessPoolExecutor(max_workers=12) as ex:
        futs=[ex.submit(classify,t) for t in tasks]; n=0
        for fu in as_completed(futs):
            out.append(fu.result()); n+=1
            if n%20==0: print(f"  {n}/{len(tasks)} ({time.time()-t0:.0f}s)",flush=True)
    import collections
    res={}
    for c in sil:
        cs=[o[3] for o in out if o[0]==c]; tot=len(cs)
        kg=sum(1 for x in cs if x in("KG_confirmed","KG_no_bounds"))
        aoc=sum(1 for x in cs if x=="AOC_confirmed")
        inc=sum(1 for x in cs if x.startswith("INCONCLUSIVE"))
        res[c]={"total":tot,"KG":kg,"AOC":aoc,"INCONCLUSIVE":inc,
                "KG_pct":round(100*kg/tot,1) if tot else 0,"AOC_pct":round(100*aoc/tot,1) if tot else 0,
                "breakdown":dict(collections.Counter(cs))}
    res["paper_5.95"]={"A_claude":{"KG_pct":96.5,"AOC_pct":3.5},"A_gptoss120b":{"KG_pct":59.5,"AOC_pct":35.7}}
    res["elapsed_s"]=round(time.time()-t0)
    json.dump({"summary":res,"rows":[{"cond":o[0],"func":o[1],"mutant":o[2],"cls":o[3]} for o in out]},
              open(EXP/"evaluation/s2n_relax_640.json","w"),indent=1)
    print("\n=== s2n assume-relaxation @6.4.0 ===")
    for c in sil: print(f"  {c:16} KG={res[c]['KG_pct']}%  AOC={res[c]['AOC_pct']}%  (n={res[c]['total']}, inc={res[c]['INCONCLUSIVE']})  {res[c]['breakdown']}")
if __name__=="__main__": main()
