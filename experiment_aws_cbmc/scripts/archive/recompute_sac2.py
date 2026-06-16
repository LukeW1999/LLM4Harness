#!/usr/bin/env python3
"""Strict SAC v2: exact harness match + verdict-based UNKNOWN-trigger (robust)."""
import json, re
from pathlib import Path
import importlib.util as u
spec=u.spec_from_file_location("A","/root/experiment_aws_cbmc/scripts/attribution_analysis.py")
A=u.module_from_spec(spec); spec.loader.exec_module(A)
EVAL=Path("/root/experiment_aws_cbmc/evaluation"); RES=Path("/root/experiment_aws_cbmc/results")
def norm(s):
    s=s.strip()
    if s.lower().startswith("assert(") and s.endswith((");",")")):
        s=s[s.index("(")+1:]; s=s.rsplit(")",1)[0]  # strip outer assert( ... )
    s=s.rstrip(";")
    return re.sub(r'\s+','',s)
def verdicts(ds,func):
    p=RES/ds/func/"summary.json"
    if not p.exists(): return {}
    return {it.get("iter"):it.get("verify") for it in json.loads(p.read_text()).get("iterations",[])}
def strict(ds,func,gts,its):
    if not its: return list(gts),[]
    maxit=max(its); vmap=verdicts(ds,func)
    pres={it:set(norm(a) for a in asl) for it,asl in its.items()}
    final=pres.get(maxit,set())
    kg,sac=[],[]
    for ga in gts:
        gn=norm(ga)
        present_iters=[it for it in sorted(pres) if gn in pres[it]]
        if not present_iters: kg.append(ga); continue
        if gn in final: continue
        last=present_iters[-1]            # last iter where present; removed at last->last+1
        if str(vmap.get(last,"")).upper().startswith("UNKNOWN"):
            sac.append(ga)                 # removed under UNKNOWN -> genuine SAC
        else:
            kg.append(ga)                  # removed under FAIL/other -> gap/correction
    return kg,sac
def recompute(cond):
    op=EVAL/f"mutation_oracle_cbmc_feedback_loop_{cond}.json"
    if not op.exists(): return None
    sil=[r for r in json.load(open(op))["results"] if r.get("silenced")]
    from collections import defaultdict
    byf=defaultdict(list)
    for r in sil: byf[r["func"]].append(r)
    s={"kg":0,"sac":0,"aoc":0,"unknown":0,"sac_funcs":[]}
    for func,muts in byf.items():
        kg,sac=strict("feedback_loop_"+cond,func,A.get_gt_asserts(func),A.get_llm_iter_asserts("feedback_loop_"+cond,func))
        n=len(muts)
        if sac: s["sac"]+=n; s["sac_funcs"].append((func,n,[norm(x) for x in sac]))
        elif kg: s["kg"]+=n
        else:
            isa,_=A.detect_aoc("feedback_loop_"+cond,func,A.get_llm_iter_asserts("feedback_loop_"+cond,func)); s["aoc" if isa else "unknown"]+=n
    s["total"]=len(sil); return s
print(f"{'condition':<20}{'tot':>4}{'KG':>5}{'SAC':>5}{'AOC':>5}{'Unk':>5}  SAC funcs")
for c in ["A_gptoss120b","M_gptoss120b","A_deepseekv4flash","A_claude","H_claude","M_claude","A_claude_r2"]:
    s=recompute(c)
    if s is None: print(f"{c:<20} (no data)"); continue
    sf="; ".join(f"{f}({n}):{t}" for f,n,t in s["sac_funcs"])
    print(f"{c:<20}{s['total']:>4}{s['kg']:>5}{s['sac']:>5}{s['aoc']:>5}{s['unknown']:>5}  {sf}")
