#!/usr/bin/env python3
"""Cloze test: blank one GT assertion at a time, ask the model to fill it.
Scores: (a) normalized exact match vs the removed assertion;
        (b) CBMC-decided: filled harness SUCCESS on original AND FAIL on the
            mutants this function silenced (knowledge actually recovered)."""
import sys, os, json, re, argparse, tempfile
from pathlib import Path
from collections import defaultdict
sys.path.insert(0, "/root/experiment_aws_cbmc/scripts")
import run_mutation_oracle_cbmc as O
import importlib.util as u
spec=u.spec_from_file_location("av2","/root/experiment_aws_cbmc/scripts/attribution_v2.py")
av2=u.module_from_spec(spec); spec.loader.exec_module(av2)
A=av2.A; norm=av2.norm
from feedback_loop import extract_c_code
EVAL=Path("/root/experiment_aws_cbmc/evaluation"); MUT=Path("/root/experiment_aws_cbmc/mutants")
GT=Path(A.GT_PROOFS_DIR) if hasattr(A,'GT_PROOFS_DIR') else None
SYSP="You are an expert in CBMC formal verification of C programs. Output only the single missing assert() statement, nothing else."

def get_call(model):
    if model=="openrouter":
        os.environ.setdefault("OPENROUTER_MODEL","openai/gpt-oss-120b")
        from call_openrouter_api import call_qwen as cq
    else:
        from call_claude_api import call_qwen as cq
    return cq

def blank_harness(gt_text, target):
    # remove the line containing the target assertion, insert marker
    lines=gt_text.splitlines(keepends=True); out=[]; done=False
    tn=norm(target)
    for ln in lines:
        if not done and 'assert' in ln and norm(ln.strip()) == tn:
            indent=re.match(r'\s*', ln).group(0)
            out.append(f"{indent}/* CLOZE: exactly one assertion is missing here -- fill it in */\n")
            done=True; continue
        out.append(ln)
    return ("".join(out), done)

def func_source(func):
    cfg=O.FUNC_CONFIGS.get(func,{}); idx=O.get_mutated_source_idx(func)
    if idx is None: return ""
    src=Path(cfg["project_sources"][idx]).read_text(errors='replace')
    i=src.find(func+"(")
    return src[max(0,i-200):i+1500] if i>=0 else ""

def cbmc_check(func, filled_harness_text, sil_mutants, timeout=90):
    cfg=O.FUNC_CONFIGS.get(func,{}); idx=O.get_mutated_source_idx(func)
    if idx is None: return None
    src=Path(cfg["project_sources"][idx])
    tf=tempfile.NamedTemporaryFile("w",suffix="_harness.c",delete=False,dir="/tmp"); tf.write(filled_harness_text); tf.close()
    hp=Path(tf.name)
    r_orig=O.run_cbmc_on_mutant(func, src, hp, timeout, idx)
    catches=0
    for m in sil_mutants:
        mc=MUT/func/f"{m}.c"
        if mc.exists() and O.run_cbmc_on_mutant(func, mc, hp, timeout, idx)=="FAIL": catches+=1
    return {"orig":r_orig,"catches":catches,"n_mut":len(sil_mutants)}

def main():
    ap=argparse.ArgumentParser()
    ap.add_argument("--cond",required=True)   # e.g. A_gptoss120b (KG funcs source)
    ap.add_argument("--model",default="claude")
    ap.add_argument("--timeout",type=int,default=90)
    a=ap.parse_args()
    CALL=get_call(a.model)
    ds=f"feedback_loop_{a.cond}"
    op=EVAL/f"mutation_oracle_cbmc_feedback_loop_{a.cond}.json"
    sil=[r for r in json.load(open(op))["results"] if r.get("silenced")]
    byf=defaultdict(list)
    for r in sil: byf[r["func"]].append(r["mutant"])
    results=[]
    for func,muts in sorted(byf.items()):
        gts=A.get_gt_asserts(func)
        if not gts: continue
        its=A.get_llm_iter_asserts(ds,func)
        allllm=set(norm(x) for asl in its.values() for x in asl) if its else set()
        missed=[g for g in gts if norm(g) not in allllm]   # never written exactly = KG candidates
        gt_path=Path(str(A.GT_PROOFS_DIR))/func/f"{func}_harness.c"
        if not gt_path.exists(): continue
        gt_text=gt_path.read_text(errors='replace')
        for ga in missed:
            blanked,ok=blank_harness(gt_text,ga)
            if not ok: results.append({"func":func,"assert":ga.strip(),"status":"BLANK_FAIL"}); continue
            prompt=f"""Below is an expert CBMC proof harness for `{func}` with EXACTLY ONE assert() statement removed (marked /* CLOZE */). Based on the function's semantics, write the single missing assertion.

Function source (excerpt):
```c
{func_source(func)}
```

Harness with one assertion blanked:
```c
{blanked}
```
Reply with ONLY the missing assert statement."""
            try:
                resp=CALL(SYSP,prompt,temperature=0.0)
            except Exception as e:
                results.append({"func":func,"assert":ga.strip(),"status":f"API_ERR:{e}"}); continue
            m=re.search(r'assert\s*\(.*?\)\s*;', resp, re.S)
            filled=(m.group(0) if m else resp.strip()[:200])
            exact = norm(filled)==norm(ga)
            filled_text=blanked.replace("/* CLOZE: exactly one assertion is missing here -- fill it in */", filled)
            cb=cbmc_check(func,filled_text,muts,a.timeout)
            rec={"func":func,"assert":ga.strip(),"filled":filled.strip(),"exact":exact,
                 "cbmc":cb,"status":"OK"}
            results.append(rec)
            cstr = f"orig={cb['orig']} catches={cb['catches']}/{cb['n_mut']}" if cb else "cbmc=NA"
            print(f"[{func}] exact={exact} {cstr}\n   GT:     {ga.strip()}\n   filled: {filled.strip()}", flush=True)
    n=[r for r in results if r["status"]=="OK"]
    ex=sum(1 for r in n if r["exact"])
    val=sum(1 for r in n if r.get("cbmc") and r["cbmc"]["orig"] in ("SUCCESS","UNKNOWN"))
    rec_=sum(1 for r in n if r.get("cbmc") and r["cbmc"]["catches"]>0 and r["cbmc"]["orig"] in ("SUCCESS","UNKNOWN"))
    print(f"\n===== CLOZE SUMMARY {a.cond} fill-model={a.model}: n={len(n)} exact={ex} valid-on-orig={val} catches-silenced-mutant(recovered)={rec_} =====")
    json.dump(results, open(EVAL/f"cloze_{a.cond}_{a.model}.json","w"), indent=1)

if __name__=="__main__": main()
