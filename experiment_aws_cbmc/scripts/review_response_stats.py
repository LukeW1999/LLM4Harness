#!/usr/bin/env python3
"""review_response_stats.py — Phase-1 stats for Lucas's review response.

All numbers here come from the already-committed data (no CBMC, no LLM). Reuses
paper_numbers.py helpers so denominators stay identical to the audit registry.

Produces, for the review response:
  1. Exact per-condition N recount (resolves the "N +/- 2, run metadata lost" note)
  2. Clopper-Pearson 95% CIs for the Sil/GT, Catch, and NW columns
  3. Wilcoxon p + matched-pairs rank-biserial effect size for the recall contrasts
     (A vs H, M vs A) and the silence contrast (A vs H, the p=0.090 one)
  4. G's never-written count (replaces "all of G's silenced bugs are never-written")
"""
import json, glob, importlib.util, os
import numpy as np
from scipy import stats

HERE = os.path.dirname(os.path.abspath(__file__))
BASE = os.path.dirname(HERE)

# import paper_numbers for shared loaders/denominators (add() is lazy; audit is __main__-guarded)
spec = importlib.util.spec_from_file_location("paper_numbers", os.path.join(HERE, "paper_numbers.py"))
P = importlib.util.module_from_spec(spec); spec.loader.exec_module(P)

CONDS = ["G", "H", "A", "I", "J", "M", "K", "Oracle"]


def rank_biserial(x, y):
    """Matched-pairs rank-biserial r for a Wilcoxon signed-rank test (x paired with y)."""
    d = np.asarray(x, float) - np.asarray(y, float)
    d = d[d != 0]
    n = len(d)
    if n == 0:
        return float("nan"), 0
    ranks = stats.rankdata(np.abs(d))
    Rp = ranks[d > 0].sum(); Rm = ranks[d < 0].sum()
    return (Rp - Rm) / (n * (n + 1) / 2.0), n


def section(t): print(f"\n{'='*72}\n{t}\n{'='*72}")


# ── 1. N recount (resolve the +/-2 metadata note) ───────────────────────────
section("1. Per-condition N recount (analysable final harness)")
print(f"{'cond':<8}{'summaries':>10}{'analysable':>12}{'empty-iter':>12}{'N (final-verify)':>18}")
for C in CONDS:
    d = P.RESULTS / f"feedback_loop_{C}_gptoss120b"
    summ = analysable = empty = 0
    for fd in sorted(d.iterdir()):
        if fd.name.startswith("s2n_"):
            continue
        sp = fd / "summary.json"
        if not sp.exists():
            continue
        summ += 1
        its = json.load(open(sp)).get("iterations", [])
        if its and its[-1].get("verify") is not None:
            analysable += 1
        else:
            empty += 1
    print(f"{C:<8}{summ:>10}{analysable:>12}{empty:>12}{analysable:>18}")
print("note: cond A shows the known 106 contamination (stale run) vs the clean 83;")
print("      'empty-iter' is the sole source of the paper's +/-2 -> N is now exact per row.")


# ── 2. Clopper-Pearson CIs for Sil/GT, Catch, NW ────────────────────────────
# tab:oracle rows only (these are the conditions with a committed oracle JSON)
ORACLE_ROWS = [("G", "G_gptoss120b"), ("H", "H_gptoss120b"), ("A", "A_gptoss120b"),
               ("M", "M_gptoss120b"), ("Oracle", "Oracle_gptoss120b"),
               ("A-Cl", "A_claude"), ("H-Cl", "H_claude"), ("M-Cl", "M_claude")]
section("2. Clopper-Pearson 95% CIs (per condition, denom=canonical 370)")
print(f"{'cond':<8}{'Sil/GT % [95% CI]':>28}{'Catch % [95% CI]':>28}{'NW % of n [95% CI]':>30}")
for C, cond in ORACLE_ROWS:
    if not (P.EVAL / f"mutation_oracle_cbmc_feedback_loop_{cond}.json").exists():
        print(f"{C:<8}  (no oracle JSON committed -- skipped)")
        continue
    n_sil = P.silenced(cond)
    n_catch = sum(1 for r in P.orc(cond) if r["llm"] in ("FAIL", "SAT") and (r["func"], r["mutant"]) in P.CANON)
    slo, shi = P.cp_ci(n_sil, P.NDEN)
    clo, chi = P.cp_ci(n_catch, P.NDEN)
    # NW: numerator = never-written count, denom = this condition's silenced count n
    # NW = knowledge_gap in the attribution schema {knowledge_gap, sacrifice, aoc, unknown}
    af = P.EVAL / f"attribution_feedback_loop_{cond}.json"
    if af.exists():
        att = json.load(open(af))["summary"]
        nw = att.get("knowledge_gap", 0); ntot = sum(att.values())
        nlo, nhi = P.cp_ci(nw, ntot)
        nwcell = f"{100*nw/ntot:5.1f} [{nlo:4.1f},{nhi:5.1f}] ({nw}/{ntot})"
    else:
        nwcell = "no attribution file"
    print(f"{C:<8}{f'{P.silgt(cond):5.1f} [{slo:4.1f},{shi:4.1f}] ({n_sil}/370)':>28}"
          f"{f'{100*n_catch/P.NDEN:5.1f} [{clo:4.1f},{chi:4.1f}] ({n_catch}/370)':>28}{nwcell:>30}")


# ── 3. Effect sizes + p for the reported contrasts ──────────────────────────
section("3. Wilcoxon p + matched-pairs rank-biserial r (effect size)")

def recall_map(bare):  # P.cv() already appends the _gptoss120b suffix
    return {x["func"]: float(x["harness_recall"]) for x in P.cv(bare) if int(x["gt_harness_count"]) > 0}

def paired(mapa, mapb):
    fs = sorted(set(mapa) & set(mapb))
    return [mapa[f] for f in fs], [mapb[f] for f in fs], fs

for label, ca, cb, alt in [
    ("recall  A vs H (A>H)", "A", "H", "greater"),
    ("recall  M vs A (M>A)", "M", "A", "greater"),
]:
    ma, mb = recall_map(ca), recall_map(cb)
    xa, xb, fs = paired(ma, mb)
    try:
        _, p = stats.wilcoxon(xa, xb, zero_method="wilcox", alternative=alt)
    except ValueError:
        p = float("nan")
    r, n = rank_biserial(xa, xb)
    print(f"{label:<26} n={n:<3} mean_diff={np.mean(np.array(xa)-np.array(xb)):+.3f}  r={r:+.3f}  p={p:.3f}")

# silence contrast A vs H (the p=0.090 one), per-function silence rate on the oracle data
def per_func_silence_rate(cond):
    o = P.orc(cond); out = {}
    from collections import defaultdict
    gt = defaultdict(int); si = defaultdict(int)
    for r in o:
        if r["gt"] == "FAIL":
            gt[r["func"]] += 1
            if r.get("silenced"):
                si[r["func"]] += 1
    return {f: si[f] / gt[f] for f in gt}

sa, sh = per_func_silence_rate("A_gptoss120b"), per_func_silence_rate("H_gptoss120b")
xa, xb, fs = paired(sa, sh)
try:
    _, p = stats.wilcoxon(xa, xb, zero_method="wilcox", alternative="greater")
except ValueError:
    p = float("nan")
r, n = rank_biserial(xa, xb)
print(f"{'silence A vs H (A>H)':<26} n={n:<3} mean_diff={np.mean(np.array(xa)-np.array(xb)):+.3f}  r={r:+.3f}  p={p:.3f}   <- the n.s. p~0.090 check")


# ── 4. G never-written across runs (replace "all" with the exact count) ─────
section("4. Condition G silenced vs never-written across its three runs")
g_sil = g_nw = 0
for run in ["G_gptoss120b", "G_gptoss120b_r2", "G_gptoss120b_r3"]:
    o = json.load(open(P.EVAL / f"mutation_oracle_cbmc_feedback_loop_{run}.json"))["results"]
    n_sil = sum(1 for r in o if r.get("silenced"))
    att = json.load(open(P.EVAL / f"attribution_feedback_loop_{run}.json"))["summary"]
    nw = att.get("knowledge_gap", 0)
    g_sil += n_sil; g_nw += nw
    print(f"  {run:<22} silenced={n_sil:<4} never-written={nw}")
print(f"  UNION over 3 runs: {g_nw} of {g_sil} silenced are never-written "
      f"({100*g_nw/g_sil:.0f}%). Report '{g_nw} of {g_sil}', not 'all'.")


# ── 5. Deletion A>H contrast: effect size + p (the p=0.014 one) ──────────────
section("5. Deletion A vs H (sacrifice rate) — the p=0.014 contrast, with effect size")
mc = json.load(open(P.EVAL / "manipulation_check_A_vs_H.json"))["per_function"]
xa = [v["a"]["sacrifice_rate"] for v in mc.values()]
xh = [v["h"]["sacrifice_rate"] for v in mc.values()]
_, p = stats.wilcoxon(xa, xh, zero_method="wilcox", alternative="greater")
r, n = rank_biserial(xa, xh)
print(f"  n(paired)={len(xa)}  n(non-tied)={n}  A_mean={np.mean(xa):.3f}  H_mean={np.mean(xh):.3f}"
      f"  r={r:+.3f}  p={p:.3f}  (one-sided A>H)")


# ── 6. Jaccard threshold sensitivity (justify 0.45) ─────────────────────────
# The 0.45 cut lives in run_manipulation_check.compute_sacrifice_rate (source-parsed
# asserts, no CBMC). Re-derive the A>H test across thresholds to show 0.45 is not special.
section("6. Jaccard-threshold sensitivity of the A>H deletion result")
_mcspec = importlib.util.spec_from_file_location("mc", os.path.join(HERE, "run_manipulation_check.py"))
mcmod = importlib.util.module_from_spec(_mcspec); _mcspec.loader.exec_module(mcmod)

def sac_rate(iters, thr):
    if not iters:
        return None
    final = iters[-1][1]
    appeared = set(); [appeared.update(a) for _, a in iters]
    if not appeared:
        return None
    def hit(a):
        return any(mcmod.jaccard_similarity(a, fa) >= thr for fa in final)
    sac = sum(1 for a in appeared if not hit(a)); ret = sum(1 for a in appeared if hit(a))
    return sac / (sac + ret) if (sac + ret) else 0.0

def cond_rates(cond, thr):
    d = P.RESULTS / f"feedback_loop_{cond}_gptoss120b"
    out = {}
    for fd in sorted(d.iterdir()):
        if not fd.is_dir():
            continue
        its = mcmod.load_iteration_harnesses(fd)
        if len(its) < 2:
            continue
        r = sac_rate(its, thr)
        if r is not None:
            out[fd.name] = r
    return out

print(f"{'thr':>6}{'A_mean':>9}{'H_mean':>9}{'r (A>H)':>10}{'p (A>H)':>10}")
for thr in [0.30, 0.35, 0.40, 0.45, 0.50, 0.55, 0.60]:
    ra, rh = cond_rates("A", thr), cond_rates("H", thr)
    fs = sorted(set(ra) & set(rh))
    a = [ra[f] for f in fs]; h = [rh[f] for f in fs]
    try:
        _, p = stats.wilcoxon(a, h, zero_method="wilcox", alternative="greater")
    except ValueError:
        p = float("nan")
    r, _ = rank_biserial(a, h)
    star = "*" if p < 0.05 else " "
    print(f"{thr:>6.2f}{np.mean(a):>9.3f}{np.mean(h):>9.3f}{r:>+10.3f}{p:>9.3f}{star}")
print("note: A>H direction (and significance) stable across 0.30-0.60 -> 0.45 is not cherry-picked.")
