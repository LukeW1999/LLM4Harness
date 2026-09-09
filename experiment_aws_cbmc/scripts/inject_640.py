#!/usr/bin/env python3
"""6.4.0 injection test (一.5): for each silenced bug, inject the GT harness's
assert() statements into the LLM final harness (before its closing brace) and
re-run CBMC on the mutant.
  FAIL    -> the catching assertion was genuinely absent (never-written): adding it
             catches the bug, so the LLM scaffold does reach the faulting state.
  SUCCESS -> even with the assertion the harness passes: the LLM scaffold never
             reaches the faulting state (reachability / scaffold, not never-written).
  COMPILE -> the GT assertion references variables absent from the LLM harness
             (cross-harness variable mismatch): inconclusive, excluded."""
import shutil
import sys, re, json, subprocess, tempfile, os, time
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

# CBMC is memory-hungry: one process can hold a gigabyte, so a fixed worker
# count that fits one machine will OOM another. Default to cores-1 and let
# CBMC_WORKERS override.
def _workers():
    import os as _os
    return int(_os.environ.get("CBMC_WORKERS") or max(1, (_os.cpu_count() or 8) - 1))
HERE=Path(__file__).resolve().parent; EXP=HERE.parent; sys.path.insert(0,str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
import run_mutation_oracle_cbmc as rmo
# CBMC 6.4.0 (the version aws-c-common's CI proofs run). Point CBMC640 at your
# 6.4.0 binary; falls back to whatever `cbmc` is on PATH.
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]; MUT=EXP/"mutants"
ASSERT=re.compile(r'^\s*assert\s*\(')
def gt_asserts(func):
    gt=rmo.GT_PROOFS_DIR/func/f"{func}_harness.c"
    if not gt.exists(): return []
    return [l.rstrip("\n") for l in open(gt) if ASSERT.match(l)]
def inject(text, asserts):
    # insert the GT asserts just before the final closing brace of the file
    i=text.rstrip().rfind("}")
    if i<0: return None
    block="\n    /* INJECTED GT assertions */\n"+"\n".join("    "+a.strip() for a in asserts)+"\n"
    return text[:i]+block+text[i:]
def run(func, harness_path, mutant_c):
    cfg=FUNC_CONFIGS.get(func); idx=rmo.get_mutated_source_idx(func)
    if not cfg or idx is None: return "NOCFG"
    ps=[str(p) for p in cfg["project_sources"]]; ps[idx]=str(mutant_c)
    cmd=([CBMC]+list(COMMON_FLAGS)+list(cfg.get("defines",[]))+list(cfg["unwind"])+list(cfg.get("unwindset") or [])+MATCH
         +["--function",f"{func}_harness"]+[str(p) for p in cfg["proof_sources"]]+[str(harness_path)]+ps)
    try: r=subprocess.run(cmd,capture_output=True,text=True,timeout=240)
    except subprocess.TimeoutExpired: return "TIMEOUT"
    o=r.stdout+r.stderr
    if "VERIFICATION FAILED" in o: return "FAIL"
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    if "error:" in o or "PARSING ERROR" in o or "CONVERSION ERROR" in o: return "COMPILE"
    return "UNKNOWN"
def work(t):
    cond,func,mutant=t
    llm=rmo.get_final_harness(EXP/f"results/feedback_loop_{cond}"/func)
    asr=gt_asserts(func)
    if not llm or not asr: return (cond,func,mutant,"NOHARNESS")
    inj=inject(open(llm).read(), asr)
    if inj is None: return (cond,func,mutant,"NOINJECT")
    with tempfile.NamedTemporaryFile("w",suffix="_harness.c",delete=False) as tf:
        tf.write(inj); tmp=tf.name
    try: v=run(func, tmp, MUT/func/f"{mutant}.c")
    finally: os.unlink(tmp)
    return (cond,func,mutant,v)
def main():
    t0=time.time()
    sil=json.load(open(EXP/"evaluation/silenced_640.json"))["verdicts"]
    tasks=[(v["cond"],v["func"],v["mutant"]) for v in sil if v["llm640"]=="SUCCESS"]
    print(f"silenced bugs to inject: {len(tasks)}",flush=True)
    out=[]
    with ProcessPoolExecutor(max_workers=_workers()) as ex:
        futs=[ex.submit(work,t) for t in tasks]; n=0
        for fu in as_completed(futs):
            out.append(fu.result()); n+=1
            if n%40==0: print(f"  {n}/{len(tasks)} ({time.time()-t0:.0f}s)",flush=True)
    import collections
    by=collections.Counter(v for *_,v in out)
    clean=[o for o in out if o[3] in ("FAIL","SUCCESS")]
    nfail=sum(1 for o in clean if o[3]=="FAIL"); nsucc=sum(1 for o in clean if o[3]=="SUCCESS")
    # per-function coverage
    funccov=collections.defaultdict(lambda:collections.Counter())
    for c,f,m,v in out: funccov[f][v]+=1
    res={"cbmc":"6.4.0","total_silenced":len(tasks),"verdict_dist":dict(by),
         "clean_compiles":len(clean),"never_written_FAIL":nfail,"reachability_SUCCESS":nsucc,
         "pct_never_written_of_clean":round(100*nfail/len(clean),1) if clean else None,
         "per_function":{f:dict(c) for f,c in sorted(funccov.items())},
         "elapsed_s":round(time.time()-t0)}
    json.dump({"summary":res,"rows":[{"cond":c,"func":f,"mutant":m,"verdict":v} for c,f,m,v in out]},
              open(EXP/"evaluation/inject_640.json","w"),indent=1)
    print("\n=== INJECTION TEST @6.4.0 ===")
    for k,v in res.items():
        if k=="per_function": continue
        print(f"  {k}: {v}")
    print("  per-function (clean FAIL/SUCCESS only shown):")
    for f,c in sorted(funccov.items()):
        cc={k:v for k,v in c.items() if k in ('FAIL','SUCCESS')}
        if cc: print(f"    {f:36} {dict(c)}")
if __name__=="__main__": main()
