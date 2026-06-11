#!/usr/bin/env python3
"""
attribution_v2.py -- rebuilt 3-independent-detector attribution (replaces Jaccard).

Detectors (independent):
  SAC-strict: GT assertion EXACT-matches an LLM assertion in iter k, CBMC verdict at
              iter k is UNKNOWN, and it is ABSENT (exact) in the final iter.
  KG:         GT assertion never EXACT-appears in any iter (or appeared then removed
              NOT under UNKNOWN = self-correction -> still a gap in final).
  AOC:        detect_aoc (assume-envelope over-constraint), assertion-independent.

Per-function synthesis priority (explicit):
  if any GT assertion is SAC-strict  -> SACRIFICE
  elif any GT assertion is missing-from-final (KG-cause) -> KNOWLEDGE-GAP
  elif detect_aoc (all GT assertions present in final, assumes over-constrained) -> AOC
  else -> UNRESOLVED
"""
import json, re
from pathlib import Path
from collections import defaultdict
import importlib.util as u
spec=u.spec_from_file_location("A","/root/experiment_aws_cbmc/scripts/attribution_analysis.py")
A=u.module_from_spec(spec); spec.loader.exec_module(A)
EVAL=Path("/root/experiment_aws_cbmc/evaluation"); RES=Path("/root/experiment_aws_cbmc/results")

def norm(s):
    s=s.strip()
    if s.lower().startswith("assert(") and (s.endswith(");") or s.endswith(")")):
        s=s[s.index("(")+1:]; s=s.rsplit(")",1)[0]
    return re.sub(r'\s+','',s.rstrip(";"))

def verdicts(ds,func):
    p=RES/ds/func/"summary.json"
    return {} if not p.exists() else {it.get("iter"):str(it.get("verify","")) for it in json.loads(p.read_text()).get("iterations",[])}

def classify_func(ds,func):
    gts=A.get_gt_asserts(func); its=A.get_llm_iter_asserts(ds,func)
    if not its: return "UNRESOLVED", {}
    maxit=max(its); vmap=verdicts(ds,func)
    pres={it:set(norm(a) for a in asl) for it,asl in its.items()}
    final=pres.get(maxit,set())
    sac_cause=False; kg_cause=False
    for ga in gts:
        gn=norm(ga)
        pi=[it for it in sorted(pres) if gn in pres[it]]
        if gn in final: continue                  # catching assertion present -> not a gap
        if not pi:                                 # never appeared
            kg_cause=True; continue
        last=pi[-1]
        if vmap.get(last,"").upper().startswith("UNKNOWN"):
            sac_cause=True                          # exact assertion removed under UNKNOWN
        else:
            kg_cause=True                           # removed under FAIL/other -> gap remains
    if sac_cause: return "SACRIFICE", {}
    if kg_cause:  return "KNOWLEDGE-GAP", {}
    isa,ev=A.detect_aoc(ds,func,its)
    return ("AOC", {"ev":ev[:2]}) if isa else ("UNRESOLVED", {})

def run(cond):
    op=EVAL/f"mutation_oracle_cbmc_feedback_loop_{cond}.json"
    if not op.exists(): return None
    ds=f"feedback_loop_{cond}"
    sil=[r for r in json.load(open(op))["results"] if r.get("silenced")]
    byf=defaultdict(list)
    for r in sil: byf[r["func"]].append(r)
    summ={"SACRIFICE":0,"KNOWLEDGE-GAP":0,"AOC":0,"UNRESOLVED":0}; fd={}
    for func,muts in byf.items():
        lab,_=classify_func(ds,func); summ[lab]+=len(muts); fd[func]=(lab,len(muts))
    summ["total"]=len(sil)
    return summ,fd

OLD={"A_gptoss120b":(90.2,0,9.8),"M_gptoss120b":(93.3,0,6.7),"A_claude":(18.8,37.5,25.0),
     "H_claude":(56.2,0,25.0),"M_claude":(45.5,0,27.3)}
print(f"{'condition':<20}{'tot':>4}{'KG':>5}{'SAC':>5}{'AOC':>5}{'Unr':>5}   old(KG/SAC/AOC)")
import sys
for c in ["A_gptoss120b","M_gptoss120b","A_deepseekv4flash","A_claude","H_claude","M_claude","A_claude_r2"]:
    r=run(c)
    if r is None: print(f"{c:<20} (no data)"); continue
    s,fd=r
    o=OLD.get(c); ostr=f"{o[0]}/{o[1]}/{o[2]}" if o else "-"
    print(f"{c:<20}{s['total']:>4}{s['KNOWLEDGE-GAP']:>5}{s['SACRIFICE']:>5}{s['AOC']:>5}{s['UNRESOLVED']:>5}   {ostr}")
    if "--detail" in sys.argv:
        for f,(lab,n) in sorted(fd.items()): print(f"      {f}: {lab} x{n}")
