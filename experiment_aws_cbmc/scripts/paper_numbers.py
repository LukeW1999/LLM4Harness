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
import json, sys
from pathlib import Path
import numpy as np
from scipy.stats import wilcoxon, fisher_exact, beta, rankdata

EVAL = Path("/root/experiment_aws_cbmc/evaluation")
RESULTS = Path("/root/experiment_aws_cbmc/results")

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
    spec=_u.spec_from_file_location("arq1", "/root/experiment_aws_cbmc/scripts/analyze_rq1.py")
    m=_u.module_from_spec(spec); spec.loader.exec_module(m)
    data=m.load_condition(ds)
    return m.analyze_condition(cond_label, data)["sacrifice_ratio"]*100
for loc,c,ds,sr in [("T2/L326","H","H_gptoss120b",86.3),
                    ("T2/L327","A","A_gptoss120b",91.4),
                    ("T2/L328b","I","I_gptoss120b",92.7),
                    ("T2/L329","J","J_gptoss120b",93.0)]:
    add(loc,f"{c} sacrifice-ratio", sr, (lambda lbl,d: lambda: sacratio(lbl,d))(c,ds), 0.6)

# config constants
add("L230","mutant-bearing functions (dir count; paper says 58 compile-valid)", 80, lambda: len([d for d in (Path("/root/experiment_aws_cbmc/mutants")).iterdir() if d.is_dir() and any(x.name.endswith(".c") for x in d.iterdir())]), 0.5)
add("L491","functions in oracle table", 40, lambda: len({r["func"] for r in orc("A_gptoss120b")}), 0.5)
add("def","total mutants run (A_gptoss)", 1233, lambda: len(orc("A_gptoss120b")), 0.5)

# ── run audit ──

# ── NEW (2026-06-12): recall-among-compilable + 4-way partition over canonical 370 ──
import json as _json
_CANON = set((r["func"],r["mutant"]) for r in
             _json.load(open("/root/experiment_aws_cbmc/evaluation/gt_fail_properties_canonical370.json"))["results"]
             if str(r.get("verdict","")).upper() in ("FAIL","SAT"))
def _orc_results(cond):
    d=_json.load(open(f"/root/experiment_aws_cbmc/evaluation/mutation_oracle_cbmc_feedback_loop_{cond}.json"))
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
    d=_json.load(open(f"/root/experiment_aws_cbmc/evaluation/{f}"))
    ok=[r for r in d if r["status"]=="OK"]
    return sum(1 for r in ok if r.get("cbmc") and r["cbmc"]["orig"] in ("SUCCESS","UNKNOWN") and r["cbmc"]["catches"]>0)
def b2_caught(f):
    d=_json.load(open(f"/root/experiment_aws_cbmc/evaluation/{f}"))
    return sum(r.get("n_caught",0) for r in d)
add("T5/cloze","cloze recovered gptoss-self (of 9)", 9, lambda: cloze_rec("cloze_A_gptoss120b_openrouter.json"), 0.5)
add("T5/cloze","cloze recovered claude-fills-gptoss (of 9)", 9, lambda: cloze_rec("cloze_A_gptoss120b_claude.json"), 0.5)
add("T5/cloze","cloze recovered claude-self (of 16)", 15, lambda: cloze_rec("cloze_A_claude_claude.json"), 0.5)
add("S7/B2","B2 repair caught Claude-A (of 16)", 11, lambda: b2_caught("b2_repair_A_claude.json"), 0.5)
add("S7/B2","B2 repair caught gptoss-A (of 41)", 0, lambda: b2_caught("b2_repair_A_gptoss120b.json"), 0.5)


# ── Table7 v2.1 (programmatic, table7_v21.json) + cross-seed + screening + Spearman ──
def t7(cond,field):
    d=_json.load(open("/root/experiment_aws_cbmc/evaluation/table7_v21.json"))
    e=d[cond]; 
    return 100.0*e[field]/e["tot"]
for cond,kg in [("A_gptoss120b",90.2),("H_gptoss120b",89.2),("M_gptoss120b",96.7),
                ("Oracle_gptoss120b",89.2),("A_deepseekv4flash",100.0),("H_deepseekv4flash",100.0),
                ("G_deepseekv4flash",100.0),("A_claude",87.5),("H_claude",81.2),("M_claude",100.0)]:
    add("T7", f"{cond} KG%", kg, (lambda c: lambda: t7(c,"KG"))(cond), 0.15)
add("T7","A_claude SAC%",12.5, lambda: t7("A_claude","SAC"), 0.15)
add("T7","Oracle KG count",141, lambda: _json.load(open("/root/experiment_aws_cbmc/evaluation/table7_v21.json"))["Oracle_gptoss120b"]["KG"], 0.5)
def spearman_pass_recall():
    pa=[84.3,81.9,75.3,70.5,67.5,62.7,31.3,28.9]; rc=[0.251,0.268,0.384,0.363,0.377,0.303,0.290,0.357]
    def rank(x): srt=sorted(x); return [srt.index(v)+1 for v in x]
    rp,rr=rank(pa),rank(rc); n=len(pa); d2=sum((a-b)**2 for a,b in zip(rp,rr))
    return 1-6*d2/(n*(n*n-1))
add("L326","Spearman pass-recall", -0.26, spearman_pass_recall, 0.01)
def sil(cond):
    d=_json.load(open(f"/root/experiment_aws_cbmc/evaluation/mutation_oracle_cbmc_feedback_loop_{cond}.json"))
    return sum(1 for r in d["results"] if r.get("silenced"))
for cond,v in [("A_claude_r3",16),("A_claude_r4",36),("A_claude_r5",19),("H_claude_r3",14),
               ("H_claude_r4",15),("H_claude_r5",44),("M_claude_r3",14),("M_claude_r4",11),
               ("M_claude_r5",9),("A_gpt55",55),("G_gpt55",48),("H_gpt55",67),
               ("A_llama3370binstruct",18),("G_llama3370binstruct",14),("A_deepseekv4pro",32)]:
    add("L503/L582", f"silenced {cond}", v, (lambda c: lambda: sil(c))(cond), 0.5)

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
