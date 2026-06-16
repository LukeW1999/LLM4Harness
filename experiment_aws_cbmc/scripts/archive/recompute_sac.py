#!/usr/bin/env python3
"""Strict SAC recompute: exact-match (not Jaccard) + UNKNOWN-triggered deletion."""
import json, re, sys
from pathlib import Path
import importlib.util as u
spec=u.spec_from_file_location("A","/root/experiment_aws_cbmc/scripts/attribution_analysis.py")
A=u.module_from_spec(spec); spec.loader.exec_module(A)
EVAL=Path("/root/experiment_aws_cbmc/evaluation")
RES=Path("/root/experiment_aws_cbmc/results")

def norm(s):
    s=re.sub(r'^\s*assert\s*\(','',s.strip()); s=re.sub(r'\)\s*;?\s*$','',s)
    return re.sub(r'\s+','',s)

def iter_log(dataset, func):
    p=RES/dataset/func/"iteration_log.json"
    if not p.exists(): return []
    try: return json.loads(p.read_text())
    except: return []

def strict_classify(dataset, func, gt_asserts, llm_iters):
    """Return (kg_list, sac_list) under strict rules."""
    if not llm_iters: return list(gt_asserts), []
    maxit=max(llm_iters.keys())
    llm_norm_by_iter={it:set(norm(a) for a in asl) for it,asl in llm_iters.items()}
    final=llm_norm_by_iter.get(maxit,set())
    log=iter_log(dataset, func)
    kg, sac = [], []
    for ga in gt_asserts:
        gn=norm(ga)
        appeared=any(gn in s for s in llm_norm_by_iter.values())
        in_final=gn in final
        if not appeared:
            kg.append(ga); continue
        if in_final:
            continue  # present in final -> caught, not a gap
        # appeared then gone: check deletion trigger
        unknown_triggered=False; fail_triggered=False
        for e in log:
            if e.get("action") in ("delete","weaken") and norm(str(e.get("assert_text",""))) == gn:
                if e.get("triggered_violation"): fail_triggered=True
                else: unknown_triggered=True
        if unknown_triggered:
            sac.append(ga)          # genuine SAC: exact assertion removed under UNKNOWN
        else:
            kg.append(ga)           # removed under FAIL (self-correction) or untracked -> treat as gap
    return kg, sac

def recompute(cond):
    op=EVAL/f"mutation_oracle_cbmc_feedback_loop_{cond}.json"
    if not op.exists(): return None
    res=json.load(open(op))["results"]
    sil=[r for r in res if r.get("silenced")]
    if not sil: return {"total":0,"kg":0,"sac":0,"aoc":0,"unknown":0}
    from collections import defaultdict
    byf=defaultdict(list)
    for r in sil: byf[r["func"]].append(r)
    summ={"kg":0,"sac":0,"aoc":0,"unknown":0}
    for func,muts in byf.items():
        gts=A.get_gt_asserts(func); its=A.get_llm_iter_asserts(cond, func)
        kg,sac=strict_classify(cond, func, gts, its)
        n=len(muts)
        if sac: summ["sac"]+=n
        elif kg: summ["kg"]+=n
        else:
            is_aoc,_=A.detect_aoc(cond, func, its)
            summ["aoc" if is_aoc else "unknown"]+=n
    summ["total"]=len(sil)
    return summ

print(f"{'condition':<22}{'tot':>4}{'KG':>5}{'SAC(strict)':>12}{'AOC':>5}{'Unk':>5}   (old SAC%)")
oldsac={"A_claude":37.5,"H_claude":0,"M_claude":0,"A_gptoss120b":0,"M_gptoss120b":0,
        "A_deepseekv4flash":0}
for c in ["A_gptoss120b","M_gptoss120b","A_deepseekv4flash","A_claude","H_claude","M_claude","A_claude_r2"]:
    s=recompute(c)
    if s is None: print(f"{c:<22} (no data)"); continue
    pct=100*s["sac"]/max(1,s["total"])
    print(f"{c:<22}{s['total']:>4}{s['kg']:>5}{s['sac']:>5} ({pct:>4.1f}%){s['aoc']:>5}{s['unknown']:>5}   old={oldsac.get(c,'?')}")
