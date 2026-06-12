#!/usr/bin/env python3
"""B2: mutation-guided repair. Feed silenced-mutant diffs back to the generating LLM;
measure how many silenced bugs become caught. Re-evaluated with the canonical CBMC oracle."""
import sys, os, json, difflib, argparse, tempfile
from pathlib import Path
from collections import defaultdict
sys.path.insert(0, "/root/experiment_aws_cbmc/scripts")
import run_mutation_oracle_cbmc as O
from feedback_loop import extract_c_code, SYSTEM_PROMPT
EVAL=Path("/root/experiment_aws_cbmc/evaluation"); RES=Path("/root/experiment_aws_cbmc/results"); MUT=Path("/root/experiment_aws_cbmc/mutants")
CALL=None  # set in main

def get_call(model):
    if model=="openrouter":
        os.environ.setdefault("OPENROUTER_MODEL","openai/gpt-oss-120b")
        from call_openrouter_api import call_qwen as cq
    else:
        from call_claude_api import call_qwen as cq
    return cq

def minimal_diff(orig_path, mut_path):
    a=orig_path.read_text(errors='replace').splitlines(keepends=True)
    b=mut_path.read_text(errors='replace').splitlines(keepends=True)
    return "".join(difflib.unified_diff(a,b,fromfile="original",tofile="mutant",n=2))[:1500]

def repair_func(cond, func, muts, timeout=120):
    ds=f"feedback_loop_{cond}"
    cfg=O.FUNC_CONFIGS.get(func,{}); idx=O.get_mutated_source_idx(func)
    if idx is None: return None
    src=Path(cfg["project_sources"][idx])
    hp=O.get_final_harness(RES/ds/func)
    if not hp: return None
    harness=hp.read_text(errors='replace')
    diffs=[]
    for m in muts:
        mc=MUT/func/f"{m}.c"
        if mc.exists(): diffs.append(f"### Missed bug ({m}) -- diff introduced into the function:\n```diff\n{minimal_diff(src,mc)}\n```")
    prompt=f"""You wrote this CBMC harness for `{func}`. Mutation testing injected the bugs below into the function, and your harness FAILED to detect them (CBMC still reported VERIFICATION SUCCESSFUL). Add or strengthen assert() statements so the harness would catch each bug. KEEP all existing __CPROVER_assume() preconditions unchanged -- do not narrow the input space. Return ONLY the complete updated harness C code.

{chr(10).join(diffs)}

Your current harness:
```c
{harness}
```"""
    try:
        improved=extract_c_code(CALL(SYSTEM_PROMPT, prompt, temperature=0.0))
    except Exception as e:
        return {"func":func,"error":str(e)}
    tf=tempfile.NamedTemporaryFile("w",suffix="_harness.c",delete=False,dir="/tmp"); tf.write(improved); tf.close()
    rp=Path(tf.name)
    (RES/ds/func/"repaired_b2_harness.c").write_text(improved)
    r_orig=O.run_cbmc_on_mutant(func, src, rp, timeout, idx)
    per={m:(O.run_cbmc_on_mutant(func, MUT/func/f"{m}.c", rp, timeout, idx) if (MUT/func/f"{m}.c").exists() else "NO_MUT") for m in muts}
    caught=[m for m,v in per.items() if v=="FAIL"]
    return {"func":func,"orig_after":r_orig,"per_mutant":per,"n_silenced":len(muts),
            "n_caught":len(caught),"valid":(r_orig in ("SUCCESS","UNKNOWN")),"caught":caught}

if __name__=="__main__":
    ap=argparse.ArgumentParser()
    ap.add_argument("--cond",required=True); ap.add_argument("--func",default=None)
    ap.add_argument("--timeout",type=int,default=120); ap.add_argument("--model",default="claude")
    a=ap.parse_args()
    CALL=get_call(a.model)
    sil=[r for r in json.load(open(EVAL/f"mutation_oracle_cbmc_feedback_loop_{a.cond}.json"))["results"] if r.get("silenced")]
    byf=defaultdict(list)
    for r in sil: byf[r["func"]].append(r["mutant"])
    funcs=[a.func] if a.func else list(byf)
    results=[]
    for f in funcs:
        if f not in byf: continue
        print(f"\n--- B2 repair {a.cond}/{f} ({len(byf[f])} silenced) ---", flush=True)
        res=repair_func(a.cond,f,byf[f],a.timeout)
        if res:
            print(f"  valid_on_original={res.get('valid')}  caught {res.get('n_caught')}/{res.get('n_silenced')}: {res.get('caught')}  {res.get('error','')}")
            results.append(res)
    if results:
        tot_s=sum(r.get('n_silenced',0) for r in results); tot_c=sum(r.get('n_caught',0) for r in results)
        valid=[r for r in results if r.get('valid')]
        print(f"\n===== B2 SUMMARY {a.cond} ({a.model}): {tot_c}/{tot_s} silenced bugs now CAUGHT; {len(valid)}/{len(results)} funcs valid on original =====")
        json.dump(results, open(EVAL/f"b2_repair_{a.cond}.json","w"), indent=1)
