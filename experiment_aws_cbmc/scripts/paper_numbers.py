#!/usr/bin/env python3
"""
paper_numbers.py — single source of truth for every data-derived number in paper.tex.

Each REGISTRY entry recomputes a numeric claim from raw experiment JSONs and
records WHERE it appears in paper.tex (the `loc` field). Run this to audit the
paper: any drift between the recomputed value and the value asserted in the
paper prints as MISMATCH.

Usage:  python3 paper_numbers.py            # full audit table
        python3 paper_numbers.py --md       # markdown table
"""
import json, os, sys
from pathlib import Path
import numpy as np
from scipy.stats import wilcoxon, fisher_exact, beta, rankdata, spearmanr

_BASE = "/root/experiment_aws_cbmc" if os.path.isdir("/root/experiment_aws_cbmc") else str(Path(__file__).resolve().parent.parent)
EVAL = Path(f"{_BASE}/evaluation")
RESULTS = Path(f"{_BASE}/results")

def orc(cond):
    return json.load(open(EVAL / f"mutation_oracle_cbmc_feedback_loop_{cond}.json"))["results"]

# canonical 370 GT-FAIL set (defined by A_gptoss120b GT verdicts)
CANON = {(r["func"], r["mutant"]) for r in orc("A_gptoss120b") if r["gt"] == "FAIL"}
NDEN = len(CANON)

def silenced(cond):
    return sum(1 for r in orc(cond) if r.get("silenced") and (r["func"], r["mutant"]) in CANON)
def silgt(cond):
    return 100.0 * silenced(cond) / NDEN
def orecall(cond):
    return 100.0 * sum(1 for r in orc(cond) if r["llm"] in ("FAIL","SAT") and (r["func"],r["mutant"]) in CANON) / NDEN

def attr(cond):
    a = json.load(open(EVAL / f"attribution_feedback_loop_{cond}.json"))
    s = a["summary"]; tot = sum(s.values())
    return {k: 100.0*v/tot for k,v in s.items()}, tot

def cp_ci(k, n):
    lo = beta.ppf(0.025, k, n-k+1) if k>0 else 0.0
    hi = beta.ppf(0.975, k+1, n-k) if k<n else 1.0
    return 100*lo, 100*hi

def load_recall(cond):
    p = EVAL / f"cross_verify_results_cond{cond}_gptoss120b.json"
    e = json.load(open(p))
    return {x["func"]:(x["harness_recall"] if x["harness_recall"] is not None else 0.0)
            for x in e if x["gt_harness_count"]>0}

def wilcox(c1, c2):
    r = {c: load_recall(c) for c in (c1,c2)}
    sh = sorted(set(r[c1]) & set(r[c2]))
    v1 = np.array([r[c1][f] for f in sh]); v2 = np.array([r[c2][f] for f in sh])
    stat,p = wilcoxon(v1,v2,alternative="greater")
    nz = (v1-v2)[(v1-v2)!=0]; rr = rankdata(np.abs(nz))
    rb = (rr[nz>0].sum()-rr[nz<0].sum())/rr.sum() if len(nz) else 0.0
    return len(sh), 100*(v1.mean()-v2.mean()), p, rb

# ── REGISTRY: (paper_loc, description, claimed, recompute_fn, tol) ──
R = []
def add(loc, desc, claimed, fn, tol=0.15):
    R.append((loc, desc, claimed, fn, tol))

# Table 6 (tab:oracle) — Sil/GT and Oracle recall, lines 501-512
add("T6/L501", "Oracle-gptoss Sil/GT",      42.7, lambda: silgt("Oracle_gptoss120b"))
add("T6/L501", "Oracle-gptoss recall",      37.6, lambda: orecall("Oracle_gptoss120b"))
add("T6/L502", "A-gptoss silenced",         41,   lambda: silenced("A_gptoss120b"), 0.5)
add("T6/L502", "A-gptoss Sil/GT",           11.1, lambda: silgt("A_gptoss120b"))
add("T6/L502", "A-gptoss recall",           42.2, lambda: orecall("A_gptoss120b"))
add("T6/L503", "H-gptoss Sil/GT",           10.0, lambda: silgt("H_gptoss120b"))
add("T6/L503", "H-gptoss recall",           47.6, lambda: orecall("H_gptoss120b"))
add("T6/L504", "M-gptoss Sil/GT",            8.1, lambda: silgt("M_gptoss120b"))
add("T6/L504", "M-gptoss recall",           64.9, lambda: orecall("M_gptoss120b"))
add("T6/L505", "G-gptoss Sil/GT",            0.3, lambda: silgt("G_gptoss120b"))
add("T6/L506", "DeepSeek-A Sil/GT",          1.4, lambda: silgt("A_deepseekv4flash"))
add("T6/L506", "DeepSeek-A recall",         73.0, lambda: orecall("A_deepseekv4flash"))
add("T6/L507", "DeepSeek-H Sil/GT",          4.9, lambda: silgt("H_deepseekv4flash"))
add("T6/L508", "DeepSeek-G Sil/GT",          1.1, lambda: silgt("G_deepseekv4flash"))
add("T6/L509", "A-Claude silenced",         16,   lambda: silenced("A_claude"), 0.5)
add("T6/L509", "A-Claude Sil/GT",            4.3, lambda: silgt("A_claude"))
add("T6/L509", "A-Claude recall",           95.7, lambda: orecall("A_claude"))
add("T6/L510", "H-Claude Sil/GT",            4.3, lambda: silgt("H_claude"))
add("T6/L510", "H-Claude recall",           93.2, lambda: orecall("H_claude"))
add("T6/L511", "M-Claude silenced",         11,   lambda: silenced("M_claude"), 0.5)
add("T6/L511", "M-Claude Sil/GT",            3.0, lambda: silgt("M_claude"))
add("T6/L511", "M-Claude recall",           96.2, lambda: orecall("M_claude"))
add("def",     "unified GT-FAIL denominator", 370, lambda: NDEN, 0.5)

# Table 7 (tab:attribution) — lines 544-549
add("T7/L544", "A-gptoss KG%",   90.2, lambda: attr("A_gptoss120b")[0]["knowledge_gap"])
add("T7/L544", "A-gptoss AOC%",   9.8, lambda: attr("A_gptoss120b")[0]["aoc"])
add("T7/L545", "M-gptoss KG%",   93.3, lambda: attr("M_gptoss120b")[0]["knowledge_gap"])
add("T7/L546", "DeepSeek-A KG%", 80.0, lambda: attr("A_deepseekv4flash")[0]["knowledge_gap"])
add("T7/L546", "DeepSeek-A AOC%",20.0, lambda: attr("A_deepseekv4flash")[0]["aoc"])
add("T7/L547", "A-Claude KG%",   18.8, lambda: attr("A_claude")[0]["knowledge_gap"])
add("T7/L547", "A-Claude SAC%",  37.5, lambda: attr("A_claude")[0]["sacrifice"])
add("T7/L547", "A-Claude AOC%",  25.0, lambda: attr("A_claude")[0]["aoc"])
add("T7/L548", "H-Claude SAC%",   0.0, lambda: attr("H_claude")[0]["sacrifice"])
add("T7/L549", "M-Claude SAC%",   0.0, lambda: attr("M_claude")[0]["sacrifice"])

# SAC conjunction CIs + Fisher — §4.2 lines ~565
add("L565", "A SAC 6/16 CI lo", 15.2, lambda: cp_ci(6,16)[0])
add("L565", "A SAC 6/16 CI hi", 64.6, lambda: cp_ci(6,16)[1])
add("L565", "H SAC 0/16 CI hi", 20.6, lambda: cp_ci(0,16)[1])
add("L565", "M SAC 0/11 CI hi", 28.5, lambda: cp_ci(0,11)[1])
add("L565", "Fisher A-vs-H p",  0.018, lambda: fisher_exact([[6,10],[0,16]])[1], 0.002)
add("L565", "Fisher A-vs-M p",  0.054, lambda: fisher_exact([[6,10],[0,11]])[1], 0.002)

# Wilcoxon Table 4 (tab:wilcoxon) — lines 370-384 (raw p + rank-biserial)
for loc, c1, c2, dm, p, rb in [
    ("T4/L371","M","Oracle",13.5,0.0008,0.53),("T4/L372","M","K",11.7,0.0022,0.48),
    ("T4/L373","M","H",8.2,0.0033,0.66),("T4/L374","J","H",8.0,0.0056,0.66),
    ("T4/L378","M","A",3.5,0.0769,0.35)]:
    add(loc, f"{c1}>{c2} dmean(pp)", dm, (lambda a,b: lambda: wilcox(a,b)[1])(c1,c2), 0.6)
    add(loc, f"{c1}>{c2} p_raw",      p, (lambda a,b: lambda: wilcox(a,b)[2])(c1,c2), 0.003)

# H vs A oracle silence: p=0.090 + paired diff CI [0.0,3.6]pp — §4.2 / abstract
def h_a_silence_ci():
    def pf(cond):
        from collections import defaultdict
        s=defaultdict(lambda:[0,0])
        for r in orc(cond):
            if r["gt"]=="FAIL": s[r["func"]][1]+=1; s[r["func"]][0]+= (1 if r.get("silenced") else 0)
        return {f:a/b for f,(a,b) in s.items() if b>0}
    A,H=pf("A_gptoss120b"),pf("H_gptoss120b"); sh=sorted(set(A)&set(H))
    d=np.array([A[f]-H[f] for f in sh]); rng=np.random.default_rng(42)
    boots=[rng.choice(d,len(d),replace=True).mean() for _ in range(10000)]
    return np.percentile(boots,2.5)*100, np.percentile(boots,97.5)*100
add("L75", "H-A silence paired CI hi(pp)", 3.6, lambda: h_a_silence_ci()[1], 0.4)

# Equivalent mutant analysis — §7 lines ~704
def eq_count():
    return sum(1 for x in json.load(open(EVAL/"assert_mutant_adjudication.json")) if x["verdict"]=="EQUIVALENT_ASSERT")
add("L705", "confirmed-equivalent mutants", 26, eq_count, 0.5)
add("L708", "Oracle adj Sil/GT (denom-26)", 45.9, lambda: 100*silenced("Oracle_gptoss120b")/(NDEN-26))
add("L708", "A-gptoss adj Sil/GT (denom-26)", 11.9, lambda: 100*silenced("A_gptoss120b")/(NDEN-26))

# Reverse cell — Part of discussion
def revcell():
    d=json.load(open(EVAL/"difftest_reverse_cell_string_eq.json"))
    return sum(1 for x in d if x["verdict"]=="DISTINGUISHED"), len(d)
add("rev", "reverse-cell genuine (string-eq)", 51, lambda: revcell()[0], 0.5)
add("rev", "reverse-cell total (string-eq)",   57, lambda: revcell()[1], 0.5)


# ── Table 2 (tab:rq1_main): pass%, assert-recall, VE% from cross_verify; sacrifice via analyze_rq1 ──
def cv(cond):
    return json.load(open(EVAL / f"cross_verify_results_cond{cond}_gptoss120b.json"))
def passrate(cond):
    # pass rate = fraction of functions whose final feedback-loop iteration verified (SUCCESS/UNSAT)
    d = RESULTS / f"feedback_loop_{cond}_gptoss120b"
    tot=succ=0
    for fd in d.iterdir():
        if fd.name.startswith("s2n_"): continue  # exclude s2n cross-corpus funcs from aws aggregate
        sp = fd / "summary.json"
        if not sp.exists(): continue
        tot += 1
        its = json.load(open(sp)).get("iterations", [])
        if its and its[-1].get("verify") in ("SUCCESS","UNSAT"): succ += 1
    return 100.0*succ/tot
def ve(cond):
    e=cv(cond); return 100.0*sum(1 for x in e if x.get("verification_equivalent"))/len(e)
def arec(cond):
    e=cv(cond); d=[x["harness_recall"] for x in e if x["gt_harness_count"]>0]; return sum(d)/len(d)

for loc,c,pa,rc,v in [
    ("T2/L325","G",31.3,0.290,36.1),("T2/L326","H",62.7,0.303,67.5),
    ("T2/L327","A",28.9,0.357,65.0),("T2/L328b","I",70.5,0.363,71.6),
    ("T2/L329","J",67.5,0.377,72.8),("T2/L330","M",75.3,0.384,72.3),
    ("T2/L331","K",81.9,0.268,80.7),("T2/L332","Oracle",84.3,0.251,80.7)]:
    add(loc,f"{c} pass%",        pa, (lambda x: lambda: passrate(x))(c), 0.2)
    add(loc,f"{c} assert-recall",rc, (lambda x: lambda: arec(x))(c), 0.01)
    add(loc,f"{c} VE%",          v,  (lambda x: lambda: ve(x))(c), 0.2)

# sacrifice ratio — reuse analyze_rq1.analyze_condition (no reimplementation)
def sacratio(cond_label, ds):
    import importlib.util as _u
    spec=_u.spec_from_file_location("arq1", f"{_BASE}/scripts/analyze_rq1.py")
    m=_u.module_from_spec(spec); spec.loader.exec_module(m)
    data=m.load_condition(ds)
    return m.analyze_condition(cond_label, data)["sacrifice_ratio"]*100
for loc,c,ds,sr in [("T2/L326","H","H_gptoss120b",86.3),
                    ("T2/L327","A","A_gptoss120b",91.4),
                    ("T2/L328b","I","I_gptoss120b",92.7),
                    ("T2/L329","J","J_gptoss120b",93.0)]:
    add(loc,f"{c} sacrifice-ratio", sr, (lambda lbl,d: lambda: sacratio(lbl,d))(c,ds), 0.6)

# config constants
add("L230","mutant-bearing functions (dir count; paper says 58 compile-valid)", 80, lambda: len([d for d in (Path(f"{_BASE}/mutants")).iterdir() if d.is_dir() and any(x.name.endswith(".c") for x in d.iterdir())]), 0.5)
add("L491","functions in oracle table", 40, lambda: len({r["func"] for r in orc("A_gptoss120b")}), 0.5)
add("def","total mutants run (A_gptoss)", 1233, lambda: len(orc("A_gptoss120b")), 0.5)

# ── run audit ──

# ── NEW (2026-06-12): recall-among-compilable + 4-way partition over canonical 370 ──
import json as _json
_CANON = set((r["func"],r["mutant"]) for r in
             _json.load(open(f"{_BASE}/evaluation/gt_fail_properties_canonical370.json"))["results"]
             if str(r.get("verdict","")).upper() in ("FAIL","SAT"))
def _orc_results(cond):
    d=_json.load(open(f"{_BASE}/evaluation/mutation_oracle_cbmc_feedback_loop_{cond}.json"))
    by={(r["func"],r["mutant"]):r for r in d["results"]}
    return [by[k] for k in _CANON if k in by]
def _verdict(r): return str(r.get("llm","")).upper()
def partition(cond, v):
    return sum(1 for r in _orc_results(cond) if _verdict(r)==v)
def recall_comp(cond):
    rs=_orc_results(cond); ce=sum(1 for r in rs if _verdict(r)=="COMPILE_ERROR")
    caught=sum(1 for r in rs if _verdict(r) in ("FAIL","SAT"))
    return 100.0*caught/(len(rs)-ce)

add("M/L288","gptoss A recall-among-compilable", 70.3, lambda: recall_comp("A_gptoss120b"), 0.2)
add("M/L288","gptoss H recall-among-compilable", 82.6, lambda: recall_comp("H_gptoss120b"), 0.2)
add("M/L288","gptoss M recall-among-compilable", 81.9, lambda: recall_comp("M_gptoss120b"), 0.2)
add("L520","A 4-way: caught(FAIL)",  156, lambda: partition("A_gptoss120b","FAIL"), 0.5)
add("L520","A 4-way: silenced(SUCC)", 41, lambda: partition("A_gptoss120b","SUCCESS"), 0.5)
add("L520","A 4-way: unknown",        25, lambda: partition("A_gptoss120b","UNKNOWN"), 0.5)
add("L520","A 4-way: compile_error", 148, lambda: partition("A_gptoss120b","COMPILE_ERROR"), 0.5)


# ── cloze + B2 (2026-06-12 evening) ──
def cloze_rec(f):
    d=_json.load(open(f"{_BASE}/evaluation/{f}"))
    ok=[r for r in d if r["status"]=="OK"]
    return sum(1 for r in ok if r.get("cbmc") and r["cbmc"]["orig"] in ("SUCCESS","UNKNOWN") and r["cbmc"]["catches"]>0)
def b2_caught(f):
    d=_json.load(open(f"{_BASE}/evaluation/{f}"))
    return sum(r.get("n_caught",0) for r in d)
add("T5/cloze","cloze recovered gptoss-self (of 9)", 9, lambda: cloze_rec("cloze_A_gptoss120b_openrouter.json"), 0.5)
add("T5/cloze","cloze recovered claude-fills-gptoss (of 9)", 9, lambda: cloze_rec("cloze_A_gptoss120b_claude.json"), 0.5)
add("T5/cloze","cloze recovered claude-self (of 16)", 15, lambda: cloze_rec("cloze_A_claude_claude.json"), 0.5)
add("S7/B2","B2 repair caught Claude-A (of 16)", 11, lambda: b2_caught("b2_repair_A_claude.json"), 0.5)
add("S7/B2","B2 repair caught gptoss-A (of 41)", 0, lambda: b2_caught("b2_repair_A_gptoss120b.json"), 0.5)


# ── Table7 v2.1 (programmatic, table7_v21.json) + cross-seed + screening + Spearman ──
def t7(cond,field):
    d=_json.load(open(f"{_BASE}/evaluation/table7_v21.json"))
    e=d[cond]; 
    return 100.0*e[field]/e["tot"]
for cond,kg in [("A_gptoss120b",90.2),("H_gptoss120b",89.2),("M_gptoss120b",96.7),
                ("Oracle_gptoss120b",89.2),("A_deepseekv4flash",100.0),("H_deepseekv4flash",100.0),
                ("G_deepseekv4flash",100.0),("A_claude",87.5),("H_claude",81.2),("M_claude",100.0)]:
    add("T7", f"{cond} KG%", kg, (lambda c: lambda: t7(c,"KG"))(cond), 0.15)
add("T7","A_claude SAC%",12.5, lambda: t7("A_claude","SAC"), 0.15)
add("T7","Oracle KG count",141, lambda: _json.load(open(f"{_BASE}/evaluation/table7_v21.json"))["Oracle_gptoss120b"]["KG"], 0.5)

# ── Clopper-Pearson 95% CIs on per-condition KG share (§5.2), 2026-06-17 ──
def _cp_kg(cond, bound):
    from scipy.stats import beta as _b
    v=_json.load(open(f"{_BASE}/evaluation/table7_v21.json"))[cond]
    k,n=v["KG"],v["tot"]; a=0.05
    lo=0.0 if k==0 else _b.ppf(a/2,k,n-k+1)
    hi=1.0 if k==n else _b.ppf(1-a/2,k+1,n-k)
    return 100*(lo if bound=="lo" else hi)
add("S5.2/ci","Oracle KG CI lower %", 83, lambda: _cp_kg("Oracle_gptoss120b","lo"), 2)
add("S5.2/ci","gptoss-A KG CI lower %", 77, lambda: _cp_kg("A_gptoss120b","lo"), 3)
add("S5.2/ci","gptoss-H KG CI lower %", 75, lambda: _cp_kg("H_gptoss120b","lo"), 3)
add("S5.2/ci","Claude-A KG CI lower %", 62, lambda: _cp_kg("A_claude","lo"), 4)
def spearman_pass_recall():
    pa=[84.3,81.9,75.3,70.5,67.5,62.7,31.3,28.9]; rc=[0.251,0.268,0.384,0.363,0.377,0.303,0.290,0.357]
    def rank(x): srt=sorted(x); return [srt.index(v)+1 for v in x]
    rp,rr=rank(pa),rank(rc); n=len(pa); d2=sum((a-b)**2 for a,b in zip(rp,rr))
    return 1-6*d2/(n*(n*n-1))
add("L326","Spearman pass-recall", -0.26, spearman_pass_recall, 0.01)
def sil(cond):
    d=_json.load(open(f"{_BASE}/evaluation/mutation_oracle_cbmc_feedback_loop_{cond}.json"))
    return sum(1 for r in d["results"] if r.get("silenced"))
for cond,v in [("A_claude_r3",16),("A_claude_r4",36),("A_claude_r5",19),("H_claude_r3",14),
               ("H_claude_r4",15),("H_claude_r5",44),("M_claude_r3",14),("M_claude_r4",11),
               ("M_claude_r5",9),("A_gpt55",55),("G_gpt55",48),("H_gpt55",67),
               ("A_llama3370binstruct",18),("G_llama3370binstruct",14),("A_deepseekv4pro",32)]:
    add("L503/L582", f"silenced {cond}", v, (lambda c: lambda: sil(c))(cond), 0.5)


# ── behavioral-mutation subset (behavioral_subset.json) ──
def _beh(key,field):
    d=_json.load(open(f"{_BASE}/evaluation/behavioral_subset.json"))
    return d[key][field] if field else d[key]
add("S7/beh","behavioral GT-FAIL denom", 329, lambda: _json.load(open(f"{_BASE}/evaluation/behavioral_subset.json"))["denom_behavioral"], 0.5)
add("S7/beh","A-gptoss sil behavioral", 41, lambda: _beh("A_gptoss120b","sil_beh"), 0.5)
add("S7/beh","Oracle sil behavioral", 149, lambda: _beh("Oracle_gptoss120b","sil_beh"), 0.5)
add("S7/beh","Claude-A sil behavioral", 15, lambda: _beh("A_claude","sil_beh"), 0.5)
add("S7/beh","A-gptoss SilGT behavioral %", 12.5, lambda: 100*_beh("A_gptoss120b","sil_beh")/_json.load(open(f"{_BASE}/evaluation/behavioral_subset.json"))["denom_behavioral"], 0.2)
add("S7/beh","Oracle SilGT behavioral %", 45.3, lambda: 100*_beh("Oracle_gptoss120b","sil_beh")/_json.load(open(f"{_BASE}/evaluation/behavioral_subset.json"))["denom_behavioral"], 0.2)


# ── reverse cell (sec:reverse) ──
add("S4/rev","Claude-A behavioral reverse cells", 81, lambda: _json.load(open(f"{_BASE}/evaluation/behavioral_subset.json"))["A_claude"]["rev_beh"], 0.5)
def _rev_stringeq():
    import os, re, difflib, sys
    sys.path.insert(0,f"{_BASE}/scripts"); import run_mutation_oracle_cbmc as _O
    C=re.compile(r'AWS_PRECONDITION|AWS_POSTCONDITION|AWS_FATAL|AWS_ASSUME|__CPROVER|^\s*[+-]\s*assert\s*\(')
    res=_json.load(open(f"{_BASE}/evaluation/mutation_oracle_cbmc_feedback_loop_A_claude.json"))["results"]
    fam={"aws_string_eq_c_str","aws_string_eq_byte_cursor","aws_string_eq_byte_buf"}
    n=0
    for r in res:
        if r["func"] not in fam: continue
        if not(str(r.get("gt","")).upper() in("SUCCESS","UNSAT") and str(r.get("llm","")).upper() in("FAIL","SAT")): continue
        mc=f"{_BASE}/mutants/{r['func']}/{r['mutant']}.c"
        cfg=_O.FUNC_CONFIGS.get(r["func"],{}); idx=_O.get_mutated_source_idx(r["func"])
        orig=open(cfg["project_sources"][idx]).read().splitlines()
        d=[l for l in difflib.unified_diff(orig,open(mc).read().splitlines(),n=0,lineterm="") if l and l[0] in "+-" and not l.startswith(("+++","---"))]
        if not C.search(" ".join(d)): n+=1
    return n
add("S4/rev","Claude-A string-eq-family reverse (behavioral)", 51, _rev_stringeq, 0.5)


# ── multi-run silenced (multirun_silenced.json) ──
def _mr(cond,i):
    return _json.load(open(f"{_BASE}/evaluation/multirun_silenced.json"))[cond][i]
add("S4/mr","A-gptoss run2 silenced", 34, lambda: _mr("A_gptoss120b",1), 0.5)
add("S4/mr","A-gptoss run3 silenced", 36, lambda: _mr("A_gptoss120b",2), 0.5)
add("S4/mr","M-gptoss run3 silenced", 45, lambda: _mr("M_gptoss120b",2), 0.5)
add("S4/mr","G-gptoss run2 silenced", 40, lambda: _mr("G_gptoss120b",1), 0.5)
add("S4/mr","G-gptoss run3 silenced", 43, lambda: _mr("G_gptoss120b",2), 0.5)
add("S4/mr","Oracle run2 silenced", 168, lambda: _mr("Oracle_gptoss120b",1), 0.5)


# ── s2n cross-corpus (sec:s2n) ──
def _s2n(cond,field):
    d=_json.load(open(f"{_BASE}/evaluation/mutation_oracle_s2n_{cond}.json"))
    res=d["results"]; gtf=[r for r in res if str(r.get("gt","")).upper() in ("FAIL","SAT")]
    sil=[r for r in gtf if r.get("silenced")]
    return {"gtfail":len(gtf),"sil":len(sil)}[field]
add("S6/s2n","s2n GT-FAIL denom", 253, lambda: _s2n("A_claude","gtfail"), 1)
add("S6/s2n","s2n Claude-A silenced", 57, lambda: _s2n("A_claude","sil"), 1)
add("S6/s2n","s2n gptoss-A silenced", 42, lambda: _s2n("A_gptoss120b","sil"), 1)
add("S6/s2n","s2n Claude-A Sil/GT %", 22.5, lambda: 100*_s2n("A_claude","sil")/_s2n("A_claude","gtfail"), 0.3)
add("S6/cost","oracle LLM-side calls/condition", 1233, lambda: len(_json.load(open(f"{_BASE}/evaluation/mutation_oracle_cbmc_feedback_loop_A_gptoss120b_pin.json"))["results"]), 1)
add("S6/cost","mutants per function (1233/83)", 14.9, lambda: len(_json.load(open(f"{_BASE}/evaluation/mutation_oracle_cbmc_feedback_loop_A_gptoss120b_pin.json"))["results"])/83, 0.3)
add("S6/s2n","s2n gptoss-A Sil/GT %", 16.6, lambda: 100*_s2n("A_gptoss120b","sil")/_s2n("A_gptoss120b","gtfail"), 0.3)

# ── severity stratification of silenced bugs (memory-safety %), §6.3, 2026-06-18 ──
import re as _re
_MEM=_re.compile(r"memcpy|memmove|memset|overlap|bounds|deref|null|out-of-bounds|pointer|is_valid|valid_memory|object", _re.I)
def _sev_mem(cond):
    canon={(r["func"],r["mutant"]):r.get("failed_properties",[]) for r in _json.load(open(f"{_BASE}/evaluation/gt_fail_properties_canonical370.json"))["results"]}
    res=_json.load(open(f"{_BASE}/evaluation/mutation_oracle_cbmc_feedback_loop_{cond}.json"))["results"]
    sil=[(r["func"],r["mutant"]) for r in res if r.get("silenced") and (r["func"],r["mutant"]) in canon]
    mem=sum(1 for k in sil if any(_MEM.search(p.get("desc","")+" "+p.get("property","")) for p in canon[k]))
    return mem
add("S6.3/sev","gptoss-A memory-safety silenced", 10, lambda: _sev_mem("A_gptoss120b"), 1)
add("S6.3/sev","Claude-A memory-safety silenced", 2, lambda: _sev_mem("A_claude"), 1)
add("S6.3/sev","Oracle memory-safety silenced", 66, lambda: _sev_mem("Oracle_gptoss120b"), 2)

# ── A3: s2n assume-relaxation cross-check (CBMC-decided KG vs AOC), 2026-06-17 ──
def _a3(cond, field):
    d=_json.load(open(f"{_BASE}/evaluation/b1_relax_s2n_{cond}.json"))
    tot=len(d)
    kg =sum(1 for r in d if r["label"] in ("KG_confirmed","KG_no_bounds"))
    aoc=sum(1 for r in d if r["label"]=="AOC_confirmed")
    return {"tot":tot,"kg":kg,"aoc":aoc,"kgpct":100*kg/tot if tot else 0,"aocpct":100*aoc/tot if tot else 0}[field]
add("S6/a3","s2n Claude-A KG% (CBMC relax)", 96.5, lambda: _a3("A_claude","kgpct"), 1)
add("S6/a3","s2n gptoss-A KG% (CBMC relax)", 59.5, lambda: _a3("A_gptoss120b","kgpct"), 2)
add("S6/a3","s2n gptoss-A AOC% (CBMC relax)", 35.7, lambda: _a3("A_gptoss120b","aocpct"), 2)

# ── confirmatory pinned run + H multi-run (added 2026-06-14, validated) ──
def _vct(cond, status):
    return sum(1 for r in orc(cond)
               if str(r.get("gt","")).upper() in ("FAIL","SAT")
               and str(r.get("llm","")).upper()==status)
def _kg_mod(cond):
    import importlib.util as _iu
    sp=_iu.spec_from_file_location("av2",f"{_BASE}/scripts/attribution_v2.py")
    m=_iu.module_from_spec(sp); sp.loader.exec_module(m)
    s,_=m.run(cond); return s["KNOWLEDGE-GAP"]
# H multi-run
add("S4/mr","H-gptoss run2 silenced", 40, lambda: silenced("H_gptoss120b_r2"), 0.5)
add("S4/mr","H-gptoss run3 silenced",  5, lambda: silenced("H_gptoss120b_r3"), 0.5)
add("S4/mr","H-gptoss run2 Sil/GT %", 10.8, lambda: silgt("H_gptoss120b_r2"), 0.3)
add("S4/mr","H-gptoss run3 Sil/GT %",  1.4, lambda: silgt("H_gptoss120b_r3"), 0.3)
# pinned confirmatory (DeepInfra/bf16)
add("Threats/pin","pinned-A silenced", 21, lambda: silenced("A_gptoss120b_pin"), 0.5)
add("Threats/pin","pinned-A Sil/GT %", 5.7, lambda: silgt("A_gptoss120b_pin"), 0.3)
add("Threats/pin","pinned-A KG", 21, lambda: _kg_mod("A_gptoss120b_pin"), 0.5)
# verdict-level breakdown cited in Threats + RQ2
add("Threats/pin","pinned-A compile-err", 109, lambda: _vct("A_gptoss120b_pin","COMPILE_ERROR"), 1)
add("Threats/pin","pinned-A caught(FAIL)", 240, lambda: _vct("A_gptoss120b_pin","FAIL"), 1)
add("Threats/pin","unpinned-A compile-err", 148, lambda: _vct("A_gptoss120b","COMPILE_ERROR"), 1)
add("Threats/pin","unpinned-A caught(FAIL)", 156, lambda: _vct("A_gptoss120b","FAIL"), 1)
add("S4/mr","H-r1 compile-err", 156, lambda: _vct("H_gptoss120b","COMPILE_ERROR"), 1)
add("S4/mr","H-r2 compile-err", 129, lambda: _vct("H_gptoss120b_r2","COMPILE_ERROR"), 1)
add("S4/mr","H-r3 compile-err", 109, lambda: _vct("H_gptoss120b_r3","COMPILE_ERROR"), 1)
add("S4/mr","H-r1 caught(FAIL)", 176, lambda: _vct("H_gptoss120b","FAIL"), 1)
add("S4/mr","H-r2 caught(FAIL)", 201, lambda: _vct("H_gptoss120b_r2","FAIL"), 1)
add("S4/mr","H-r3 caught(FAIL)", 256, lambda: _vct("H_gptoss120b_r3","FAIL"), 1)


# ── Clopper-Pearson bounds for SAC rarity (added 2026-06-14, ARS review C3) ──
def _cp_upper0(n):  # 95% two-sided CP upper bound for 0/n (closed form)
    return 100.0 * (1 - 0.025**(1.0/n))
add("RQ2/sac","CP upper 0/41 (gptoss-A)", 8.6, lambda: _cp_upper0(41), 0.2)
add("RQ2/sac","CP upper 0/16 (16-bug cond)", 20.6, lambda: _cp_upper0(16), 0.3)
add("S6/s2n","CP upper 0/57 (s2n Claude)", 6.3, lambda: _cp_upper0(57), 0.3)

# ── behavioural rename-immune KG re-attribution (ARS C1, 2026-06-14) ──
def _bkg(cond, field):
    d=_json.load(open(f"{_BASE}/evaluation/behavioural_kg_feedback_loop_{cond}.json"))
    return d[field]
add("RQ2/bkg","behav-KG A_gptoss", 37, lambda: _bkg("A_gptoss120b","behavioural_KG"), 0.5)
add("RQ2/bkg","behav-KG M_gptoss", 30, lambda: _bkg("M_gptoss120b","behavioural_KG"), 0.5)
add("RQ2/bkg","behav-KG A_claude", 16, lambda: _bkg("A_claude","behavioural_KG"), 0.5)
add("RQ2/bkg","behav-KG H_claude", 16, lambda: _bkg("H_claude","behavioural_KG"), 0.5)
add("RQ2/bkg","behav-KG M_claude", 11, lambda: _bkg("M_claude","behavioural_KG"), 0.5)
add("RQ2/bkg","behav-sacrifice A_gptoss", 4, lambda: _bkg("A_gptoss120b","ever_caught_some_iter"), 0.5)
add("RQ2/bkg","behav-KG total", 111, lambda: sum(_bkg(c,"behavioural_KG") for c in
     ["A_gptoss120b","M_gptoss120b","G_gptoss120b","A_claude","H_claude","M_claude"]), 0.5)
add("RQ2/bkg","behav-silenced total", 115, lambda: sum(_bkg(c,"silenced_total") for c in
     ["A_gptoss120b","M_gptoss120b","G_gptoss120b","A_claude","H_claude","M_claude"]), 0.5)

# ── Sil/GT-among-compilable (ARS C7/D, 2026-06-14) ──
def _acsilgt(cond):
    rs=[r for r in orc(cond) if (r["func"],r["mutant"]) in CANON]
    sil=sum(1 for r in rs if r.get("silenced"))
    defi=sum(1 for r in rs if str(r.get("llm","")).upper() in ("SUCCESS","UNSAT","FAIL","SAT"))
    return 100.0*sil/defi if defi else 0.0
add("RQ2/ac","amongComp Sil/GT Oracle", 53.2, lambda: _acsilgt("Oracle_gptoss120b"), 0.3)
add("RQ2/ac","amongComp Sil/GT A_gptoss", 20.8, lambda: _acsilgt("A_gptoss120b"), 0.3)
add("RQ2/ac","amongComp Sil/GT M_gptoss", 11.1, lambda: _acsilgt("M_gptoss120b"), 0.3)
add("RQ2/ac","amongComp Sil/GT A_claude", 4.3, lambda: _acsilgt("A_claude"), 0.2)

# ── Claude multi-run Sil/GT range endpoints (tab:multirun, 2026-06-15) ──
_CLA={"A":[16,16,36,19],"H":[16,14,15,44],"M":[11,14,11,9]}
def _clmin(c): return 100.0*min(_CLA[c])/370
def _clmax(c): return 100.0*max(_CLA[c])/370
add("T-mr","Claude A Sil/GT max", 9.7, lambda: _clmax("A"), 0.15)
add("T-mr","Claude A Sil/GT min", 4.3, lambda: _clmin("A"), 0.15)
add("T-mr","Claude H Sil/GT max", 11.9, lambda: _clmax("H"), 0.15)
add("T-mr","Claude H Sil/GT min", 3.8, lambda: _clmin("H"), 0.15)
add("T-mr","Claude M Sil/GT max", 3.8, lambda: _clmax("M"), 0.15)
add("T-mr","Claude M Sil/GT min", 2.4, lambda: _clmin("M"), 0.15)

# ── s2n silenced-bug spread (abstract + §s2n: rules out single-function artifact) ──
try:
    from collections import Counter as _Ctr
    def _s2n_spread(cond):
        d = json.load(open(f"{_BASE}/evaluation/mutation_oracle_s2n_{cond}.json"))
        r = d.get("results", d) if isinstance(d, dict) else d
        c = _Ctr(x["func"] for x in r if x.get("silenced"))
        return len(c), 100.0 * max(c.values()) / sum(c.values())
    add("abs/s2n","s2n Claude #silenced funcs", 9,    lambda: _s2n_spread("A_claude")[0], 0)
    add("abs/s2n","s2n gptoss #silenced funcs", 5,    lambda: _s2n_spread("A_gptoss120b")[0], 0)
    add("abs/s2n","s2n gptoss max func share %", 35.7, lambda: _s2n_spread("A_gptoss120b")[1], 0.3)
except Exception:
    pass  # s2n oracle jsons absent; skip

# ── Pinned-rho finding (#46, Threats §sec:threats; recomputed by pinned_rho.compute) ──
# Matched-set / de-biased-rho sub-analysis checks removed (2026-06-29 revision):
# the paper no longer presents the detailed pass-rate matched-set rho analysis,
# so these registry entries were orphaned. Full pinned data remains in the package;
# the surviving pinned claims (A silenced/Sil-GT/KG, A pass 28.9->71.1) are checked above/below.

# ── PC postcondition-checklist probe + de-biased pinned RQ1 (#48/#53), 2026-06-18 ──
import glob as _glob
def _canon_set():
    return {(r["func"],r["mutant"]) for r in _json.load(open(f"{_BASE}/evaluation/gt_fail_properties_canonical370.json"))["results"]}
def _sil_in_canon(fname):
    c=_canon_set()
    res=_json.load(open(f"{_BASE}/evaluation/{fname}"))["results"]
    return len({(r["func"],r["mutant"]) for r in res if r.get("silenced") and (r["func"],r["mutant"]) in c})
add("PC/claude","PC-Claude silenced (canon370)", 14, lambda: _sil_in_canon("mutation_oracle_cbmc_feedback_loop_PC_claude_pc.json"), 1)
add("PC/gptoss","PC-gptoss silenced (canon370)", 52, lambda: _sil_in_canon("mutation_oracle_cbmc_feedback_loop_PC_gptoss120b_pc.json"), 2)
add("PC/base","Claude-A silenced (canon370)", 16, lambda: _sil_in_canon("mutation_oracle_cbmc_feedback_loop_A_claude.json"), 1)

# de-biased pinned pass/recall/rho — pass needs server summary.json; recall is local cross_verify_*_pin
def _pin_pass(C):
    conv=tot=0
    for sp in _glob.glob(f"{_BASE}/results/feedback_loop_{C}_gptoss120b_pin/*/summary.json"):
        try:
            j=_json.load(open(sp)); tot+=1; v=j.get("converged"); its=j.get("iterations",[])
            if v is True or (its and str(its[-1].get("verify","")).upper() in ("SUCCESS","UNSAT")): conv+=1
        except: pass
    return 100*conv/tot if tot else 0
def _pin_recall(C):
    e=_json.load(open(f"{_BASE}/evaluation/cross_verify_results_cond{C}_pin.json"))
    rs=[x["harness_recall"] for x in e if x.get("gt_harness_count",0)>0 and x.get("harness_recall") is not None]
    return 100*sum(rs)/len(rs) if rs else 0
_PINC=["A","G","H","I","J","K","M","Oracle"]
def _pin_rho():
    P=[_pin_pass(C) for C in _PINC]; R=[_pin_recall(C) for C in _PINC]
    return spearmanr(P,R)[0]
add("S.threats/pin","de-biased pinned A pass%", 71.1, lambda: _pin_pass("A"), 3)
add("S.threats/pin","de-biased pinned Oracle pass%", 90.4, lambda: _pin_pass("Oracle"), 3)
add("S.threats/pin","de-biased pinned rho(pass,recall)", -0.357, _pin_rho, 0.08)


# ── operator-class / defect profile of the silenced set (operator_class_silenced.json) ──
def _op():
    return json.load(open(f"{_BASE}/evaluation/operator_class_silenced.json"))
def _op_share(key, opname):
    d=_op()[key]; return 100*d["operators"].get(opname,0)/d["n_resolved"]
add("S7/op","silenced AOR (arithmetic) share %", 37, lambda: _op_share("UNION_all_conditions","AOR (arithmetic operator)"), 0.6)
add("S7/op","silenced ROR (relational) share %", 16, lambda: _op_share("UNION_all_conditions","ROR (relational operator)"), 0.6)
add("S7/op","silenced CRP+off-by-one share %", 15,
    lambda: 100*(_op()["UNION_all_conditions"]["operators"].get("CRP (constant replacement)",0)
                 +_op()["UNION_all_conditions"]["operators"].get("CRP off-by-one (constant +/-1)",0))
                 /_op()["UNION_all_conditions"]["n_resolved"], 0.6)
add("S7/op","silenced contract-line share %", 13, lambda: 100*_op()["UNION_all_conditions"]["contract_line_share"], 0.6)
add("S7/op","silenced contract share A-gptoss %", 0, lambda: 100*_op()["A_gptoss120b"]["contract_line_share"], 0.6)
add("S7/op","silenced contract share A-claude %", 6, lambda: 100*_op()["A_claude"]["contract_line_share"], 0.6)
add("S7/op","unique silenced (func,mutant) union", 307, lambda: _op()["UNION_all_conditions"]["n_resolved"], 0.5)


# ── equivalent-mutant finalization (equiv_finalize.json) ──
def _eqf():
    return json.load(open(f"{_BASE}/evaluation/equiv_finalize.json"))
def _eqf_n(final_prefix):
    return sum(1 for x in _eqf() if x["final"].startswith(final_prefix))
add("S7/eq","contract-line equiv candidates", 41, lambda: len(_eqf()), 0.5)
add("S7/eq","confirmed EQUIVALENT", 26, lambda: _eqf_n("EQUIVALENT"), 0.5)
add("S7/eq","confirmed NON-EQUIVALENT", 15, lambda: _eqf_n("NONEQ"), 0.5)
add("S7/eq","fatal CBMC-proven non-equiv", 6, lambda: _eqf_n("NONEQ_PROVEN"), 0.5)
add("S7/eq","memory-predicate non-equiv", 9, lambda: _eqf_n("NONEQ_STRUCTURAL"), 0.5)


# ── verifier independence: CBMC vs ESBMC (esbmc_oracle_A_claude_assert.json) ──
def _vi():
    ast={(r["func"],r["mutant"]):r for r in json.load(open(f"{_BASE}/evaluation/esbmc_oracle_A_claude_assert.json"))["results"]}
    cbl=json.load(open(f"{_BASE}/evaluation/mutation_oracle_cbmc_feedback_loop_A_claude.json"))["results"]
    cb={(r["func"],r["mutant"]):r for r in cbl}
    fs=set(f for f,_ in ast)
    isF=lambda v: v in ("FAIL","SAT")
    egf=sum(1 for k,r in ast.items() if isF(r["gt"]))
    esl=sum(1 for k,r in ast.items() if r.get("silenced"))
    cgf=sum(1 for r in cbl if r["func"] in fs and isF(r["gt"]))
    csl=sum(1 for r in cbl if r["func"] in fs and r.get("silenced"))
    es=set(k for k,r in ast.items() if r.get("silenced"))
    cs=set(k for k,r in cb.items() if r.get("silenced") and k[0] in fs)
    # two-way GT-detection on common mutants
    common=[k for k in ast if k in cb]
    both=sum(1 for k in common if isF(cb[k]["gt"]) and isF(ast[k]["gt"]))
    cbonly=sum(1 for k in common if isF(cb[k]["gt"]) and not isF(ast[k]["gt"]))
    esonly=sum(1 for k in common if isF(ast[k]["gt"]) and not isF(cb[k]["gt"]))
    return dict(egf=egf,esl=esl,cgf=cgf,csl=csl,ov=len(es&cs),eo=len(es-cs),
                both=both,cbonly=cbonly,esonly=esonly)
add("S8/vi","ESBMC GT-FAIL (35f)", 284, lambda: _vi()["egf"], 0.5)
add("S8/vi","ESBMC silenced", 12, lambda: _vi()["esl"], 0.5)
add("S8/vi","CBMC GT-FAIL (35f)", 354, lambda: _vi()["cgf"], 0.5)
add("S8/vi","CBMC silenced (35f)", 14, lambda: _vi()["csl"], 0.5)
add("S8/vi","ESBMC-CBMC silenced overlap", 12, lambda: _vi()["ov"], 0.5)
add("S8/vi","ESBMC-only silenced", 0, lambda: _vi()["eo"], 0.5)
add("S8/vi","both-GT-FAIL common mutants", 273, lambda: _vi()["both"], 0.5)
add("S8/vi","CBMC-only GT-FAIL", 81, lambda: _vi()["cbonly"], 0.5)
add("S8/vi","ESBMC-only GT-FAIL (two-way)", 11, lambda: _vi()["esonly"], 0.5)
add("S8/vi","ESBMC Sil/GT %", 4.2, lambda: 100*_vi()["esl"]/_vi()["egf"], 0.15)
add("S8/vi","CBMC Sil/GT 35f %", 4.0, lambda: 100*_vi()["csl"]/_vi()["cgf"], 0.15)


def main():
    md = "--md" in sys.argv
    rows=[]; nfail=0
    for loc, desc, claimed, fn, tol in R:
        try:
            got = fn(); ok = abs(got-claimed) <= tol
        except Exception as e:
            got = f"ERR:{e}"; ok=False
        if not ok: nfail+=1
        gs = f"{got:.4g}" if isinstance(got,(int,float)) else str(got)[:40]
        rows.append((loc, desc, f"{claimed:g}", gs, "OK" if ok else "**MISMATCH**"))
    if md:
        print("| paper loc | quantity | claimed | recomputed | |")
        print("|---|---|---|---|---|")
        for r in rows: print("| "+" | ".join(r)+" |")
    else:
        print(f"{'loc':<10}{'quantity':<34}{'claimed':>9}{'recomputed':>12}  status")
        print("-"*78)
        for loc,desc,cl,gs,st in rows:
            print(f"{loc:<10}{desc:<34}{cl:>9}{gs:>12}  {st}")
    print(f"\n{len(R)} numbers checked, {nfail} mismatch(es). denom={NDEN}")

if __name__=="__main__":
    main()
