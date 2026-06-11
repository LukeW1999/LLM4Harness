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

# ── run audit ──
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
