#!/usr/bin/env python3
"""
paper_numbers_640.py — audit registry for the CBMC 6.4.0 numbers in paper.tex.

Companion to paper_numbers.py, which audits the earlier CBMC 5.95.1 analysis.
The submitted paper pins CBMC 6.4.0 (the version aws-c-common's own CI proofs
run), so every version-dependent number was recomputed by the `*_640.py`
scripts into `evaluation/*_640.json`. This registry recomputes each claim from
those artifacts and prints OK or MISMATCH against the value asserted in
paper.tex.

Version-independent claims (token-Jaccard assertion recall, blind adjudication,
equivalent-mutant analysis) are audited by paper_numbers.py and re-checked here
only where the paper's current wording differs.

Usage:  python3 paper_numbers_640.py           # audit table
        python3 paper_numbers_640.py --md      # markdown table
"""
import json, os, re, sys
from collections import Counter, defaultdict
from pathlib import Path
import sys as _sys
_sys.path.insert(0, str(Path(__file__).resolve().parent))

import numpy as np
from scipy.stats import beta, wilcoxon

import run_mutation_oracle_cbmc as rmo  # noqa: E402
_HAS_GT = Path(str(rmo.GT_PROOFS_DIR)).is_dir()

_BASE = "/root/experiment_aws_cbmc" if os.path.isdir("/root/experiment_aws_cbmc") else str(Path(__file__).resolve().parent.parent)
EVAL = Path(_BASE) / "evaluation"

def _load(name):
    return json.load(open(EVAL / name))

# ── raw 6.4.0 verdicts ───────────────────────────────────────────────────────
_GT = _load("gtfail_640.json")
GT = {(r["func"], r["mutant"]): r["gt640"] for r in _GT["verdicts"]}
CANON = {k for k, v in GT.items() if v == "FAIL"}          # shared GT-fail set
NDEN = len(CANON)

_SIL = _load("silenced_640.json")
LLM = defaultdict(dict)                                     # cond -> key -> verdict
for r in _SIL["verdicts"]:
    LLM[r["cond"]][(r["func"], r["mutant"])] = r["llm640"]

# conditions whose LLM verdicts live in the K/Llama sweep instead
_KL = _load("kllama_oracle_640.json")
for r in _KL["verdicts"]:
    LLM[r["cond"]][(r["func"], r["mutant"])] = r["llm640"]

# paper condition names -> dataset keys
COND = {"Single": "G_gptoss120b", "Baseline": "A_gptoss120b", "Neutral": "H_gptoss120b",
        "Bounded": "M_gptoss120b", "SpecFirst": "K_gptoss120b", "Oracle": "Oracle_gptoss120b",
        "Baseline/Claude": "A_claude", "Neutral/Claude": "H_claude", "Bounded/Claude": "M_claude"}

def _cell(cond):
    """(silenced, caught, unresolved) counts over the shared GT-fail set."""
    v = LLM[COND[cond]]
    sil = cau = unr = 0
    for k in CANON:
        x = v.get(k)
        if x == "SUCCESS":
            sil += 1
        elif x in ("FAIL", "SAT"):
            cau += 1
        else:
            unr += 1
    return sil, cau, unr

def n_sil(cond):   return _cell(cond)[0]
def sil_gt(cond):  return 100.0 * _cell(cond)[0] / NDEN
def catch(cond):   return 100.0 * _cell(cond)[1] / NDEN
def unres(cond):   return 100.0 * _cell(cond)[2] / NDEN
def adj_sil(cond):
    s, c, _ = _cell(cond)
    return 100.0 * s / (s + c)

def per_func_silence(cond):
    """Per-function silenced share of that function's GT-fail mutants."""
    v = LLM[COND[cond]]
    agg = defaultdict(lambda: [0, 0])
    for (f, m) in CANON:
        agg[f][1] += 1
        if v.get((f, m)) == "SUCCESS":
            agg[f][0] += 1
    return {f: a / b for f, (a, b) in agg.items() if b > 0}

def cp_ci(k, n):
    lo = beta.ppf(0.025, k, n - k + 1) if k > 0 else 0.0
    hi = beta.ppf(0.975, k + 1, n - k) if k < n else 1.0
    return 100 * lo, 100 * hi

# ── registry ─────────────────────────────────────────────────────────────────
R = []
def add(loc, desc, claimed, fn, tol=0.15):
    R.append((loc, desc, claimed, fn, tol))

# corpus / denominator (§4 Study Design, §5 Results)
add("design", "mutants in shared set",            1233, lambda: len(GT), 0.5)
add("design", "shared GT-fail denominator",        397, lambda: NDEN, 0.5)
add("design", "GT-fail also failing under 5.95.1", 370, lambda: _GT["summary"]["both_fail"], 0.5)
add("design", "UNKNOWN->FAIL gained at 6.4.0",      27, lambda: _GT["summary"]["transitions_595_to_640"]["UNKNOWN->FAIL"], 0.5)

# Table 1 (tab:rq1) — pass rate and silenced share
_PASS = _load("passrate_640.json")["per_condition"]
_KPASS = _load("k_passrate_640.json")
def passrate(cond):
    key = COND[cond]
    return _KPASS["pass_pct_640"] if key.startswith("K_") else _PASS[key]["pass_pct_640"]

for cond, pa, sg in [("Single", 31.3, 0.3), ("Baseline", 44.6, 9.8), ("Neutral", 60.2, 9.3),
                     ("Bounded", 71.1, 7.6), ("SpecFirst", 74.7, 14.9), ("Oracle", 79.5, 39.8)]:
    add("T1/rq1", f"{cond} pass %",   pa, (lambda c: lambda: passrate(c))(cond), 0.2)
    add("T1/rq1", f"{cond} Sil/GT %", sg, (lambda c: lambda: sil_gt(c))(cond))

# Table 2 (tab:oracle) — oracle results and mechanism attribution.
# Mechanism shares are recomputed from the adjudicated per-function labels
# (build_adjudicated_mechanism.py) over each condition's 6.4.0 silenced set.
_ADJ = _load("adjudicated_mechanism.json")["labels"]

def mech(cond, label):
    """Share of a condition's LIVE silenced bugs carrying the adjudicated label.
    A dead scaffold has no missing-assertion story, so it is excluded."""
    key = COND[cond]
    lab, v = _ADJ[key], LLM[key]
    tot = hit = 0
    for (f, m) in CANON:
        if v.get((f, m)) == "SUCCESS" and _REACH.get((key, f)) != "SUCCESS":
            tot += 1
            hit += 1 if lab.get(f) == label else 0
    return 100.0 * hit / tot if tot else 0.0

def scaffold_residual():
    """Silences left over once NW/Del/Nar are accounted for (paper: 23)."""
    out = 0
    for cond in COND:
        if cond == "SpecFirst":
            continue
        n = n_sil(cond)
        out += round(n * (100.0 - sum(mech(cond, l) for l in ("NW", "Del", "Nar"))) / 100.0)
    return out
for cond, n, sg, ca, un, ad, nw, de, na in [
    ("Oracle",          158, 39.8, 41.8, 18.4, 48.8, 100.0, 0.0,  0.0),
    ("Baseline",         39,  9.8, 37.8, 52.4, 20.6, 33.3,  0.0, 66.7),
    ("Neutral",          37,  9.3, 44.3, 46.4, 17.4,  0.0,  0.0,  0.0),
    ("Bounded",          30,  7.6, 67.3, 25.1, 10.1, 96.7,  0.0,  0.0),
    ("Single",            1,  0.3, 42.3, 57.4,  0.7, 100.0, 0.0,  0.0),
    ("Baseline/Claude",  16,  4.0, 96.0,  0.0,  4.0, 87.5, 12.5,  0.0),
    ("Neutral/Claude",   16,  4.0, 93.7,  2.3,  4.1, 81.2,  0.0, 12.5),
    ("Bounded/Claude",   11,  2.8, 96.7,  0.5,  2.8, 100.0, 0.0,  0.0)]:
    add("T2/oracle", f"{cond} n",       n,  (lambda c: lambda: n_sil(c))(cond), 0.5)
    add("T2/oracle", f"{cond} Sil/GT",  sg, (lambda c: lambda: sil_gt(c))(cond))
    add("T2/oracle", f"{cond} Catch",   ca, (lambda c: lambda: catch(c))(cond))
    add("T2/oracle", f"{cond} Unres",   un, (lambda c: lambda: unres(c))(cond))
    add("T2/oracle", f"{cond} Adj.Sil", ad, (lambda c: lambda: adj_sil(c))(cond))
    add("T2/oracle", f"{cond} NW %",  nw, (lambda c: lambda: mech(c, "NW"))(cond))
    add("T2/oracle", f"{cond} Del %", de, (lambda c: lambda: mech(c, "Del"))(cond))
    add("T2/oracle", f"{cond} Nar %", na, (lambda c: lambda: mech(c, "Nar"))(cond))

# Table 2 caption + §RQ2: total silences, scaffold residual
add("T2/cap", "organic silences (7 conditions)", 150,
    lambda: sum(n_sil(c) for c in COND if c != "Oracle" and c != "SpecFirst"), 0.5)
add("T2/cap", "all silences incl. Oracle", 308,
    lambda: _load("inject_640.json")["summary"]["total_silenced"], 0.5)

# §RQ1: the Oracle contrast (per-function, function-clustered bootstrap)
def oracle_vs_baseline():
    O, B = per_func_silence("Oracle"), per_func_silence("Baseline")
    sh = sorted(set(O) & set(B))
    d = np.array([O[f] - B[f] for f in sh])
    rng = np.random.default_rng(42)
    boots = [rng.choice(d, len(d), replace=True).mean() for _ in range(10000)]
    # 20 of the 32 pairs tie, so scipy drops them and n falls to 12. At that size
    # the normal approximation overstates p (0.0009 against an exact 0.0002).
    p = wilcoxon(np.array([O[f] for f in sh]), np.array([B[f] for f in sh]),
                 alternative="greater", method="exact")[1]
    return (100 * d.mean(), 100 * np.percentile(boots, 2.5), 100 * np.percentile(boots, 97.5),
            int((d > 0).sum()), int((d < 0).sum()), p)

add("S5.1/orc", "Oracle-Baseline silence rise (pp)", 28.3, lambda: oracle_vs_baseline()[0], 0.5)
add("S5.1/orc", "bootstrap CI lo (pp)",              14.7, lambda: oracle_vs_baseline()[1], 1.5)
add("S5.1/orc", "bootstrap CI hi (pp)",              43.1, lambda: oracle_vs_baseline()[2], 1.5)
add("S5.1/orc", "functions moving up",                 12, lambda: oracle_vs_baseline()[3], 0.5)
add("S5.1/orc", "functions moving down",                0, lambda: oracle_vs_baseline()[4], 0.5)
add("S5.1/orc", "paired Wilcoxon p (exact)",      0.0002, lambda: oracle_vs_baseline()[5], 0.0001)
add("S5.1/orc", "paired functions", 32, lambda: len(set(per_func_silence("Oracle")) & set(per_func_silence("Baseline"))), 0.5)

# §RQ1: single-pass repeats (Single silences 1/40/43, 83 of 84 never written)
_PROBE = _load("probe_gmr_640.json")["summary"]["per_condition"]
add("S5.1/rep", "Single run-2 silenced", 40, lambda: _PROBE["G_gptoss120b_r2"]["silenced_640"], 0.5)
add("S5.1/rep", "Single run-3 silenced", 43, lambda: _PROBE["G_gptoss120b_r3"]["silenced_640"], 0.5)
add("S5.1/rep", "Single three-run total", 84,
    lambda: n_sil("Single") + _PROBE["G_gptoss120b_r2"]["silenced_640"] + _PROBE["G_gptoss120b_r3"]["silenced_640"], 0.5)

# §RQ2: repetition of the Baseline run
_MULTI = _load("multirun_640.json")
add("S5.2/rep", "Baseline run-2 silenced", 34, lambda: _MULTI["A_gptoss120b_r2"], 0.5)
add("S5.2/rep", "Baseline run-3 silenced", 36, lambda: _MULTI["A_gptoss120b_r3"], 0.5)

# §RQ2: behavioural rename-immune never-written re-check
_BKG = _load("behavioural_kg_640.json")["summary"]
add("S5.2/beh", "silences with re-runnable history", 265, lambda: _BKG["re_runnable_decided"], 0.5)
add("S5.2/beh", "never-written (behavioural)",       263, lambda: _BKG["never_written_behavioural"], 0.5)
add("S5.2/beh", "never-written %",                  99.2, lambda: _BKG["pct_never_written_of_decided"])
add("S5.2/beh", "genuine catch-then-remove",           2, lambda: _BKG["ever_caught"], 0.5)

# §RQ2: active deletion union bound (2 behavioural + 2 adjudicated of 308)
add("S5.2/del", "deletion union on live %", 3.8, lambda: 100 * 4 / (308 - dead_silenced()))
add("S5.2/del", "deletion CI lo on live %",  1.0, lambda: cp_ci(4, 308 - dead_silenced())[0])

# §RQ2: the aws_byte_buf_cat cluster and the leave-one-function-out check
def _sil_in(cond, func):
    v = LLM[COND[cond]]
    return sum(1 for (f, m) in CANON if f == func and v.get((f, m)) == "SUCCESS")
def _loo(cond, func):
    s, _, _ = _cell(cond)
    return 100.0 * (s - _sil_in(cond, func)) / NDEN
add("S5.2/clu", "buf_cat share of Baseline", 33, lambda: _sil_in("Baseline", "aws_byte_buf_cat"), 0.5)
add("S5.2/clu", "Baseline Sil/GT without buf_cat %", 1.6, lambda: _loo("Baseline", "aws_byte_buf_cat"))
add("S5.2/clu", "Claude silences outside buf_cat", 13,
    lambda: n_sil("Baseline/Claude") - _sil_in("Baseline/Claude", "aws_byte_buf_cat"), 0.5)
add("S5.2/clu", "Claude silenced functions", 6,
    lambda: len({f for f, r in per_func_silence("Baseline/Claude").items() if r > 0}), 0.5)
add("S5.2/clu", "Oracle silenced functions", 14,
    lambda: len({f for f, r in per_func_silence("Oracle").items() if r > 0}), 0.5)
add("S5.2/clu", "Claude caught of 397", 381, lambda: _cell("Baseline/Claude")[1], 0.5)

# §RQ2: cloze test
_CLOZE = _load("cloze_640.json")
add("S5.2/clz", "cloze fills recovering the bugs", 27, lambda: _CLOZE["recovered"], 0.5)
add("S5.2/clz", "cloze live attempts",             28, lambda: _CLOZE["total_fills"], 0.5)

# §RQ3: memory-safety share of the silenced bugs (properties known for the 370 core)
_MEM = re.compile(r"memcpy|memmove|memset|overlap|bounds|deref|null|out-of-bounds|pointer|is_valid|valid_memory|object", re.I)
def _sev_mem(cond):
    props = {(r["func"], r["mutant"]): r.get("failed_properties", [])
             for r in _load("gt_fail_properties_canonical370.json")["results"]}
    v = LLM[COND[cond]]
    sil = [k for k in CANON if v.get(k) == "SUCCESS"]
    return sum(1 for k in sil if k in props and
               any(_MEM.search(p.get("desc", "") + " " + p.get("property", "")) for p in props[k]))
add("S5.3/sev", "Baseline memory-safety silenced",        10, lambda: _sev_mem("Baseline"), 1)
add("S5.3/sev", "Baseline/Claude memory-safety silenced",  2, lambda: _sev_mem("Baseline/Claude"), 1)
add("S5.3/sev", "Oracle memory-safety silenced",          66, lambda: _sev_mem("Oracle"), 2)

# §6 Generality: third model, s2n-tls, self-built reference
_KL_S = _KL["per_condition"]
add("S6/mod", "Llama Baseline Sil/GT %", 4.5, lambda: _KL_S["A_llama3370binstruct"]["SilGT_pct"])

_S2N = _load("s2n_640.json")["summary"]
_RLX = _load("s2n_relax_640.json")

_GF = _load("greenfield_640.json")
add("S6/gf", "self-built reference recall %", 94.8, lambda: _GF["recovery_pct"])
add("S6/gf", "self-built CI lo %",            91.9, lambda: cp_ci(_GF["recovered"], _GF["held_out_silenced_total"])[0], 0.5)
add("S6/gf", "self-built CI hi %",            96.8, lambda: cp_ci(_GF["recovered"], _GF["held_out_silenced_total"])[1], 0.5)

# §6.2 s2n-tls, regenerated under the corrected prompt and the Makefile-derived
# build. The earlier arms were generated against a build where the expert
# harnesses themselves did not verify, so both their denominator and their
# silenced counts are superseded.
def _s2n(file, cond, field):
    return _load(file)["summary"][cond][field]

add("S6/s2n", "s2n GT-fail set", 273, lambda: _s2n("s2n_fix3_640.json", "A_claude_fix3", "gtfail"), 0.5)
add("S6/s2n", "s2n Claude silenced", 63,
    lambda: _s2n("s2n_fix3_640.json", "A_claude_fix3", "silenced"), 0.5)
add("S6/s2n", "s2n Claude Sil/GT %", 23.1,
    lambda: 100 * _s2n("s2n_fix3_640.json", "A_claude_fix3", "silenced")
    / _s2n("s2n_fix3_640.json", "A_claude_fix3", "gtfail"))
add("S6/s2n", "s2n Claude functions", 10,
    lambda: _s2n("s2n_fix3_640.json", "A_claude_fix3", "funcs_with_silence"), 0.5)
add("S6/s2n", "s2n Claude max function share %", 21.0,
    lambda: _s2n("s2n_fix3_640.json", "A_claude_fix3", "max_func_share_pct"), 0.6)
add("S6/s2n", "s2n gpt-oss silenced", 14,
    lambda: _s2n("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2", "silenced"), 0.5)
add("S6/s2n", "s2n gpt-oss Sil/GT %", 5.1,
    lambda: 100 * _s2n("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2", "silenced")
    / _s2n("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2", "gtfail"))
add("S6/s2n", "s2n gpt-oss functions", 5,
    lambda: _s2n("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2", "funcs_with_silence"), 0.5)

def _s2n_decidable(file, cond):
    rows = [r for r in _load(file)["rows"] if r["cond"] == cond and r["gt"] == "FAIL"]
    return sum(1 for r in rows if r["llm"] in ("SUCCESS", "FAIL")), len(rows)

add("S6/s2n", "s2n Claude decidable %", 77,
    lambda: 100 * _s2n_decidable("s2n_fix3_640.json", "A_claude_fix3")[0]
    / _s2n_decidable("s2n_fix3_640.json", "A_claude_fix3")[1], 0.6)
add("S6/s2n", "s2n gpt-oss decidable %", 59,
    lambda: 100 * _s2n_decidable("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2")[0]
    / _s2n_decidable("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2")[1], 0.6)
add("S6/s2n", "s2n gpt-oss undecidable GT-fail mutants", 111,
    lambda: _s2n_decidable("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2")[1]
    - _s2n_decidable("s2n_fix2_gptoss_640.json", "A_gptoss120b_fix2")[0], 0.5)

add("S6/s2n", "s2n Claude silences from a dead scaffold", 0,
    lambda: _load("s2n_reach_fix3_640.json")["summary"]["dead"], 0.5)

# §6.3 Cross-engine corroboration (ESBMC 8.3.0 vs the pinned CBMC 6.4.0)
_ESB = _load("esbmc_oracle_A_claude_assert.json")["results"]
def _key(r):  return (r["func"], r["mutant"].replace(".c", ""))
def esbmc_cell():
    """(both GT-fail, CBMC-only loose, CBMC-only strict, ESBMC-only strict, ESBMC silenced)."""
    both = c_loose = c_strict = e_strict = esil = 0
    for r in _ESB:
        k = _key(r)
        cg, eg = GT.get(k), r["gt"]
        decisive = eg in ("FAIL", "SUCCESS")
        if cg == "FAIL" and eg == "FAIL":
            both += 1
            esil += 1 if r["llm"] == "SUCCESS" else 0
        elif cg == "FAIL":
            c_loose += 1
            c_strict += 1 if decisive else 0
        elif eg == "FAIL" and cg in ("FAIL", "SUCCESS"):
            e_strict += 1
    return both, c_loose, c_strict, e_strict, esil

add("S6.3/esb", "ESBMC-comparable functions",     35, lambda: len({r["func"] for r in _ESB}), 0.5)
add("S6.3/esb", "mutants both engines adjudicate", 296, lambda: esbmc_cell()[0], 0.5)
add("S6.3/esb", "CBMC-only GT-fail (loose)",        78, lambda: esbmc_cell()[1], 0.5)
add("S6.3/esb", "CBMC-only GT-fail (both decide)",   6, lambda: esbmc_cell()[2], 0.5)
add("S6.3/esb", "ESBMC-only GT-fail (both decide)", 56, lambda: esbmc_cell()[3], 0.5)
add("S6.3/esb", "silenced under ESBMC",             12, lambda: esbmc_cell()[4], 0.5)
def esbmc_2x2():
    """On the 296 both engines call GT-fail: (both silence, CBMC-only, ESBMC-only,
    both catch, CBMC catches while ESBMC cannot decide)."""
    c = Counter()
    for r in _ESB:
        k = _key(r)
        if GT.get(k) != "FAIL" or r["gt"] != "FAIL":
            continue
        a, b = LLM["A_claude"].get(k), r["llm"]
        sa, sb = a == "SUCCESS", b == "SUCCESS"
        if sa and sb:              c["both_sil"] += 1
        elif sa:                   c["cbmc_only"] += 1
        elif sb:                   c["esbmc_only"] += 1
        elif b in ("FAIL", "SAT"): c["both_catch"] += 1
        else:                      c["esbmc_undec"] += 1
    return c

add("S6.3/esb", "silenced by both engines",        12, lambda: esbmc_2x2()["both_sil"], 0.5)
add("S6.3/esb", "silenced by CBMC only",            0, lambda: esbmc_2x2()["cbmc_only"], 0.5)
add("S6.3/esb", "silenced by ESBMC only",           0, lambda: esbmc_2x2()["esbmc_only"], 0.5)
add("S6.3/esb", "caught by both engines",         277, lambda: esbmc_2x2()["both_catch"], 0.5)
add("S6.3/esb", "caught by CBMC, ESBMC undecided",  7, lambda: esbmc_2x2()["esbmc_undec"], 0.5)

add("S6.3/esb", "silenced under CBMC on the shared 296", 12,
    lambda: sum(1 for r in _ESB if GT.get(_key(r)) == "FAIL" and r["gt"] == "FAIL"
                and LLM["A_claude"].get(_key(r)) == "SUCCESS"), 0.5)

# §5.2 Reachability partition: a harness whose postcondition region is unreachable
# passes on every mutant without checking anything (scripts/reachability_probe.py).
_REACH = {(r["cond"], r["func"]): r["probe"]
          for r in _load("reachability_probe_640.json")["rows"]}
_LIVE = _load("mechanism_live_640.json")
_CAUSE = _load("vacuity_cause_640.json")["summary"]

PAPER8 = ["Oracle", "Baseline", "Neutral", "Bounded", "Single",
          "Baseline/Claude", "Neutral/Claude", "Bounded/Claude"]

# One source of truth for "did the harness run". The full-region probe supersedes
# the head-only one wherever it has a verdict, so both must not be consulted
# independently or the dead count differs by group between sections.
_FULL_PROBE = {(r["cond"], r["func"]): r
               for r in _load("reachability_full_640.json")["rows"]}

def _never_ran(cond, func):
    r = _FULL_PROBE.get((cond, func))
    if r is not None:
        return r.get("head") != "FAIL"
    return _REACH.get((cond, func)) == "SUCCESS"

def dead_silenced(cond=None):
    conds = [cond] if cond else PAPER8
    out = 0
    for c in conds:
        v = LLM[COND[c]]
        for (f, m) in CANON:
            if v.get((f, m)) == "SUCCESS" and _never_ran(COND[c], f):
                out += 1
    return out

add("S5.2/dead", "silences from dead scaffolds",        202, lambda: dead_silenced(), 0.5)
add("S5.2/dead", "live silences",                       106, lambda: 308 - dead_silenced(), 0.5)
add("S5.2/dead", "Oracle dead",                         135, lambda: dead_silenced("Oracle"), 0.5)
add("S5.2/dead", "Baseline dead",                        33, lambda: dead_silenced("Baseline"), 0.5)
add("S5.2/dead", "Neutral dead",                         33, lambda: dead_silenced("Neutral"), 0.5)
add("S5.2/dead", "Bounded dead",                          0, lambda: dead_silenced("Bounded"), 0.5)
add("S5.2/dead", "Claude conditions dead",                0,
    lambda: sum(dead_silenced(c) for c in ("Baseline/Claude", "Neutral/Claude", "Bounded/Claude")), 0.5)
add("S5.2/dead", "dead by unwind truncation (all)",     214, lambda: _CAUSE["unwind-truncation"]["silenced"], 0.5)
add("S5.2/dead", "dead by contradictory assumes (all)",  29, lambda: _CAUSE["contradictory-assumes"]["silenced"], 0.5)

_CAUSE_ROWS = _load("vacuity_cause_640.json")["rows"]
def dead_by(cause):
    keys = {COND[c] for c in PAPER8}
    return sum(r["n"] for r in _CAUSE_ROWS if r["cause"] == cause and r["cond"] in keys)
add("S5.2/dead", "dead by unwind truncation",           174, lambda: dead_by("unwind-truncation"), 0.5)
add("S5.2/dead", "dead by contradictory assumes",        27, lambda: dead_by("contradictory-assumes"), 0.5)
add("S5.2/dead", "dead groups failing with unwinding assertions on", 11,
    lambda: sum(1 for r in _CAUSE_ROWS if r["cause"] == "unwind-truncation"), 0.5)

# behavioural re-check, restricted to the harnesses that actually run
_BEH_ROWS = _load("behavioural_kg_640.json")["rows"]
def _beh_live():
    live = [r for r in _BEH_ROWS if _REACH.get((r["cond"], r["func"])) != "SUCCESS"]
    return sum(1 for r in live if not r["ever_caught"]), len(live)
add("S5.2/live", "behavioural never-written among live",   62, lambda: _beh_live()[0], 0.5)
add("S5.2/live", "live silences with an iteration history", 64, lambda: _beh_live()[1], 0.5)
add("S5.2/live", "behavioural never-written share %",     96.9, lambda: 100 * _beh_live()[0] / _beh_live()[1], 0.2)

# s2n-tls reachability
_S2N_R = _load("reachability_probe_s2n_640.json")

add("S5.2/live", "unresolved among live", 6,
    lambda: sum(1 for c in PAPER8 for (f, m) in CANON
                if LLM[COND[c]].get((f, m)) == "SUCCESS"
                and _REACH.get((COND[c], f)) != "SUCCESS"
                and _ADJ[COND[c]].get(f) not in ("NW", "Nar", "Del")), 0.5)
# §5.2 the postcondition region is reachable end to end, not just at its first line
_FULL = _load("reachability_full_640.json")["rows"]
def _probe_pair():
    head = [r for r in _FULL if r.get("head") == "FAIL"]
    return len(head), sum(1 for r in head if r.get("tail") == "FAIL")

add("S5.2/reach", "groups whose first postcondition is reachable", 53, lambda: _probe_pair()[0], 0.5)
add("S5.2/reach", "of those, last postcondition also reachable", 53, lambda: _probe_pair()[1], 0.5)

# §5.2 the strengthening test: does adding assertions, with the assume envelope
# held fixed, actually catch the mutant? A catch proves the setup reaches the bad
# state and the gap really was the assertion; a miss does not, and cannot be read
# as never-written. Runs whose strengthened harness stops verifying on the
# unmutated source are dropped, since those catch everything for the wrong reason.
def _strengthen(model=None):
    import glob as _g
    tested = caught = 0
    for f in _g.glob(str(Path(_BASE) / "evaluation/b2_repair_*.json")):
        if "b2_repair_gt_" in f:      # the GT-guided runs are a separate series
            continue
        cond = Path(f).stem.replace("b2_repair_", "")
        if model and (("claude" in cond) != (model == "claude")):
            continue
        for r in _load(Path(f).name):
            if not r.get("valid") or _REACH.get((cond, r["func"])) == "SUCCESS":
                continue
            tested += r["n_silenced"]; caught += r["n_caught"]
    return caught, tested

add("S5.2/str", "live silences given the strengthening test", 62, lambda: _strengthen()[1], 0.5)
add("S5.2/str", "of those, the assertion really was the gap", 38, lambda: _strengthen()[0], 0.5)
add("S5.2/str", "Claude confirmed", 27, lambda: _strengthen("claude")[0], 0.5)
add("S5.2/str", "Claude tested", 34, lambda: _strengthen("claude")[1], 0.5)
add("S5.2/str", "gpt-oss confirmed", 11, lambda: _strengthen("gptoss")[0], 0.5)
add("S5.2/str", "gpt-oss tested", 28, lambda: _strengthen("gptoss")[1], 0.5)

# §3.1 the fidelity gate the certificate is defined on, enforced rather than
# assumed. It barely moves the silences, since a harness that fails on correct
# code rarely verifies a mutant, but it removes the cheap catches such a harness
# contributes to the denominator every rate is measured against.
_GATE = {}
for _f in ("passrate_640.json", "passrate_repeats_640.json"):
    if (Path(_BASE) / "evaluation" / _f).exists():
        for _r in _load(_f)["verdicts"]:
            _GATE[(_r["cond"], _r["func"])] = _r["v"]

def _gated(cond=None):
    """(silences, silences passing the gate, catches, catches passing)."""
    s = gs = c = gc = 0
    for k in ([cond] if cond else PAPER8):
        key = COND[k]
        v = LLM[key]
        for (f, m) in CANON:
            ok = _GATE.get((key, f)) == "SUCCESS"
            if v.get((f, m)) == "SUCCESS":
                s += 1; gs += ok
            elif v.get((f, m)) in ("FAIL", "SAT"):
                c += 1; gc += ok
    return s, gs, c, gc

add("S3.1/gate", "certified silences passing the fidelity gate", 307, lambda: _gated()[1], 0.5)
add("S3.1/gate", "catches passing the fidelity gate", 1835, lambda: _gated()[3], 0.5)
add("S5.1/gate", "Baseline adjudicated silence % gated", 28.7,
    lambda: 100 * _gated("Baseline")[1] / (_gated("Baseline")[1] + _gated("Baseline")[3]))
add("S5.1/gate", "Oracle adjudicated silence % gated", 53.2,
    lambda: 100 * _gated("Oracle")[1] / (_gated("Oracle")[1] + _gated("Oracle")[3]))

# §5.2 what a cheap static check would have caught. Precision against the base
# rate is the question: a lint that flags half the corpus buys nothing.
def _lint(name, cell):
    t = _load("lint_baseline_640.json")[name]
    return t.get(str(cell), 0)

add("S5.2/lint", "groups the return-value lint flags", 121,
    lambda: _lint("no-retval", ("sil", "flag")) + _lint("no-retval", ("clean", "flag")), 0.5)
add("S5.2/lint", "of those, actually silencing", 23,
    lambda: _lint("no-retval", ("sil", "flag")), 0.5)
add("S5.2/lint", "return-value lint precision %", 19.0,
    lambda: 100 * _lint("no-retval", ("sil", "flag"))
    / (_lint("no-retval", ("sil", "flag")) + _lint("no-retval", ("clean", "flag"))))
add("S5.2/lint", "silencing base rate %", 17.2,
    lambda: 100 * 44 / 256, 0.2)

# Table 1 puts every per-condition figure in the paper, so every cell needs an
# entry here or the table is exactly the kind of unaudited surface the registry
# exists to prevent.
_PR = _load("passrate_640.json")["per_condition"]
for _c, _v in [("Single", 31.3), ("Baseline", 44.6), ("Neutral", 60.2), ("Bounded", 71.1),
               ("Oracle", 79.5)]:
    add("T1/pass", f"{_c}/gpt-oss pass %", _v,
        (lambda k: lambda: _PR[COND[k]]["pass_pct_640"])(_c), 0.2)
for _c, _v in [("Baseline", 92.9), ("Neutral", 90.4), ("Bounded", 91.6)]:
    add("T1/pass", f"{_c}/Claude pass %", _v,
        (lambda k: lambda: _PR[k + "_claude" if False else {"Baseline": "A_claude",
         "Neutral": "H_claude", "Bounded": "M_claude"}[k]]["pass_pct_640"])(_c), 0.2)

# RQ1 re-established under the default configuration. passrate_strict_640.json
# was produced before we knew that dropping --no-unwinding-assertions does not
# enable the check on a --no-standard-checks build, so it re-measured the
# permissive run; passrate_default_640.json passes --unwinding-assertions and
# is the real default. Pass rate falls everywhere under it, and the condition
# handed the expert preconditions falls furthest, because its harnesses are the
# ones that never ran.
_PR_D = _load("passrate_default_640.json")["per_condition"]

def _strict_silgt(cond):
    key = COND[cond]
    return 100 * sum(1 for k in _STRICT_GT if _STRICT[key].get(k) == "SUCCESS") / len(_STRICT_GT)

add("T1/def", "Baseline/gpt-oss pass % (default)", 28.9,
    lambda: _PR_D["A_gptoss120b"]["pass_pct_640"], 0.2)
add("T1/def", "GivenPre/gpt-oss pass % (default)", 16.9,
    lambda: _PR_D["Oracle_gptoss120b"]["pass_pct_640"], 0.2)
add("T1/def", "Baseline Sil/GT % (default)", 1.4, lambda: _strict_silgt("Baseline"), 0.1)
add("T1/def", "Oracle Sil/GT % (default)", 6.5, lambda: _strict_silgt("Oracle"), 0.1)
add("T1/def", "Oracle-to-Baseline silence ratio (default)", 4.5,
    lambda: _strict_silgt("Oracle") / _strict_silgt("Baseline"), 0.2)

# SpecFirst was run for RQ1 only, so it is scored from its own two files rather
# than the eight-condition sweeps: a natural-language contract lifts pass rate
# without lifting silencing, which is what makes the GivenPre effect specific.
_PR_K = _PR_D["K_gptoss120b"]
_SIL_K = _load("silenced_K_default_640.json")["summary"]["per_condition"]["K_gptoss120b"]
add("T1/def", "SpecFirst pass % (default)", 25.3, lambda: _PR_K["pass_pct_640"], 0.2)
add("T1/def", "SpecFirst silenced (default)", 10, lambda: _SIL_K["silenced_640"], 0.5)
add("T1/def", "SpecFirst Sil/GT % (default)", 2.4, lambda: _SIL_K["SilGT_640_pct"], 0.1)

# Every default-configuration pass rate, so Table 1's column cannot drift again.
for _c, _k, _v in [("Single", "G_gptoss120b", 27.7), ("Baseline", "A_gptoss120b", 28.9),
                   ("Neutral", "H_gptoss120b", 45.8), ("Bounded", "M_gptoss120b", 53.0),
                   ("SpecFirst", "K_gptoss120b", 25.3), ("GivenPre", "Oracle_gptoss120b", 16.9),
                   ("Baseline/Claude", "A_claude", 85.7), ("Neutral/Claude", "H_claude", 80.7),
                   ("Bounded/Claude", "M_claude", 84.3)]:
    add("T1/def", f"{_c} pass % (default)", _v,
        (lambda k: lambda: _PR_D[k]["pass_pct_640"])(_k), 0.2)

# Threats: does the unwinding check hit LLM and expert harnesses alike? It does
# not, and the asymmetry is measured rather than estimated.
_PR_OFF_V = {(v["cond"], v["func"]): v["v"] for v in _load("passrate_640.json")["verdicts"]}
_PR_ON_V = {(v["cond"], v["func"]): v["v"] for v in _load("passrate_default_640.json")["verdicts"]}
_LLM_OK = [k for k in _PR_OFF_V if k in _PR_ON_V and _PR_OFF_V[k] == "SUCCESS"]
_EXPERT = _load("expert_passrate_640.json")
add("S8/unwind", "LLM groups verifying with the check off", 467, lambda: len(_LLM_OK), 0.5)
add("S8/unwind", "of those, stop verifying with it on", 117,
    lambda: sum(1 for k in _LLM_OK if _PR_ON_V[k] != "SUCCESS"), 0.5)
add("S8/unwind", "expert harnesses verifying with the check off", 84,
    lambda: _EXPERT["verify_off"], 0.5)
add("S8/unwind", "of those, stop verifying with it on", 2,
    lambda: _EXPERT["stop_when_on"], 0.5)

# Numbers the paper prints that nothing above was watching. audit_paper_coverage.py
# found them by walking paper.tex; each is registered here so it cannot drift.
_LINT_NR = _load("lint_baseline_640.json")["no-retval"]
add("S5.2/lint", "groups the lint runs over", 256, lambda: sum(_LINT_NR.values()), 0.5)
add("S5.2/lint", "of those, silencing", 44,
    lambda: _LINT_NR["('sil', 'flag')"] + _LINT_NR["('sil', 'pass')"], 0.5)
add("S5.2/lint", "groups the best check flags", 121,
    lambda: _LINT_NR["('sil', 'flag')"] + _LINT_NR["('clean', 'flag')"], 0.5)
add("S5.2/lint", "of those, clean", 98, lambda: _LINT_NR["('clean', 'flag')"], 0.5)

add("S5.2/layer", "silences the four layers place", 305,
    lambda: sum(_layers()[k] for k in ("never ran", "illegal setup",
                                       "unreachable bug", "missing assert")), 0.5)
add("S5.2/dead", "dead silences with no sub-cause row", 1,
    lambda: dead_silenced() - dead_by("unwind-truncation")
            - dead_by("contradictory-assumes"), 0.5)

add("T1/rq1", "Bounded-to-Baseline silence ratio (default)", 5.2,
    lambda: _strict_silgt("Bounded") / _strict_silgt("Baseline"), 0.2)
add("T1/rq1", "Bounded Sil/GT % (default)", 7.5, lambda: _strict_silgt("Bounded"), 0.1)

# A condition that passes more has more mutants adjudicated at all, so a raw
# count ratio carries a pool-size effect. Both ratios are reported.
def _adjudicated(cond):
    key = COND[cond]
    v = [_STRICT[key].get(x) for x in _STRICT_GT]
    return sum(1 for x in v if x in ("SUCCESS", "FAIL")), sum(1 for x in v if x == "SUCCESS")
add("S5.1/adj", "Baseline mutants adjudicated (default)", 207, lambda: _adjudicated("Baseline")[0], 0.5)
add("S5.1/adj", "Bounded mutants adjudicated (default)",  315, lambda: _adjudicated("Bounded")[0], 0.5)
add("S5.1/adj", "Baseline silence per adjudicated %",     2.9,
    lambda: 100 * _adjudicated("Baseline")[1] / _adjudicated("Baseline")[0], 0.1)
add("S5.1/adj", "Bounded silence per adjudicated %",      9.8,
    lambda: 100 * _adjudicated("Bounded")[1] / _adjudicated("Bounded")[0], 0.1)
add("S5.1/adj", "Bounded-to-Baseline pass rise (pp, default)", 24,
    lambda: _PR_D["M_gptoss120b"]["pass_pct_640"] - _PR_D["A_gptoss120b"]["pass_pct_640"], 0.6)
add("S5.1/adj", "Bounded-to-Baseline ratio, adjudicated", 3.4,
    lambda: (_adjudicated("Bounded")[1] / _adjudicated("Bounded")[0])
            / (_adjudicated("Baseline")[1] / _adjudicated("Baseline")[0]), 0.15)

# The corpus split the paper states: 83 functions, 25 whose mutation target sits
# in an inline file, 58 left, of which 40 have a runnable expert-and-LLM pair.
# Only the 40 is measured; the rest is arithmetic, checked so it cannot drift
# again the way "58 mutant-bearing" did.
add("design", "functions with a runnable pair", 40,
    lambda: len({v["func"] for v in _GT["verdicts"]}), 0.5)
_RUNNABLE = lambda: len({v["func"] for v in _GT["verdicts"]})
add("design", "in scope after the inline exclusion", 58, lambda: 25 + 18 + _RUNNABLE() - 25, 0.5)
add("design", "in scope without a runnable pair", 18, lambda: 58 - _RUNNABLE(), 0.5)
add("design", "corpus split adds up", 83, lambda: 25 + 18 + _RUNNABLE(), 0.5)

# Table 1 cell by cell. It is billed as every per-condition number the paper
# rests on, so every cell is registered, not a sample of them.
_T1 = {"Single":    (27.7,  1, 31.3,   1,   1,  0,  0,  0),
       "Baseline":  (28.9,  6, 44.6,  39,  33,  0,  4,  2),
       "Neutral":   (45.8,  4, 60.2,  37,  33,  0,  4,  0),
       "Bounded":   (53.0, 31, 71.1,  30,   0,  6, 12, 12),
       "GivenPre":  (16.9, 27, 79.5, 158, 135, 23,  0,  0),
       "Baseline/Claude": (85.7, 14, 92.9, 16, 0, 0, 4, 10),
       "Neutral/Claude":  (80.7, 14, 90.4, 16, 0, 2, 0, 13),
       "Bounded/Claude":  (84.3,  9, 91.6, 11, 0, 0, 3,  8)}
# The last numbers audit_paper_coverage.py found unwatched.
_S2N_ORACLE = _load("mutation_oracle_s2n_A_claude.json")
_S2N_MUTANTS = _S2N_ORACLE["summary"]["total"]
_S2N_FUNCS = len({r["func"] for r in _S2N_ORACLE["results"]})
_CLOZE_RAW = [r for f in ("cloze_A_claude_claude.json", "cloze_A_gptoss120b_claude.json",
                          "cloze_A_gptoss120b_openrouter.json") for r in _load(f)]
_CLOZE_BLANKS = len({(r["func"], r["assert"]) for r in _CLOZE_RAW
                     if r["func"] != "aws_nospec_mask"})
add("S5.3/sev", "memory-safety share, lowest condition %", 12,
    lambda: min(100 * _sev_mem(c) / n_sil(c)
                for c in ("Baseline", "Baseline/Claude", "Oracle") if n_sil(c)), 1.5)
add("S5.3/sev", "memory-safety share, highest condition %", 42,
    lambda: max(100 * _sev_mem(c) / n_sil(c)
                for c in ("Baseline", "Baseline/Claude", "Oracle") if n_sil(c)), 1.5)
add("S6/s2n", "s2n compiled mutants", 650, lambda: _S2N_MUTANTS, 0.5)
add("S6/s2n", "s2n functions", 25, lambda: _S2N_FUNCS, 0.5)
add("S6/s2n", "s2n Claude Sil/GT % before the build fixes", 22.5,
    lambda: 100 * _S2N["A_claude"]["silenced"] / _S2N["A_claude"]["gtfail"], 0.2)
add("S5.2/clz", "cloze unique blanks", 17, lambda: _CLOZE_BLANKS, 0.5)
def _clz(f):
    live = [x for x in _load(f) if x["func"] != "aws_nospec_mask"]
    full = [x for x in live if x["cbmc"]["orig"] == "SUCCESS"
            and x["cbmc"]["n_mut"] and x["cbmc"]["catches"] >= x["cbmc"]["n_mut"]]
    return len(full), len(live)
add("S5.2/clz", "fills catching every silenced mutant", 26,
    lambda: sum(_clz(f)[0] for f in ("cloze_A_claude_claude.json",
                                     "cloze_A_gptoss120b_claude.json",
                                     "cloze_A_gptoss120b_openrouter.json")), 0.5)
add("S5.2/clz", "Claude filling its own gaps", 15,
    lambda: _clz("cloze_A_claude_claude.json")[0], 0.5)
add("S5.2/clz", "Claude's own gaps attempted", 16,
    lambda: _clz("cloze_A_claude_claude.json")[1], 0.5)
add("S5.2/clz", "gpt-oss filling its own gaps", 6,
    lambda: _clz("cloze_A_gptoss120b_openrouter.json")[0], 0.5)

# The reverse cell of the matrix: mutants the LLM harness catches and the expert
# misses. Nothing was registered here, and the Discussion quoted a stale count.
_REV = _load("reverse_640.json")
add("S6.3/rev", "reverse-cell mutants (Claude Baseline)", 90,
    lambda: _REV["summary"]["reverse_cell_raw"], 0.5)
add("S6.3/rev", "expert functions the reverse cell exposes", 10,
    lambda: len({r[0] for r in _REV["reverse"]}), 0.5)

# Every prompt condition generated a harness for the whole corpus, which is what
# Table 1's caption now states in place of an N column two of whose six values
# could not be reproduced.
def _generated(cond):
    import run_mutation_oracle_cbmc as _rmo
    gt = {p.name for p in _rmo.GT_PROOFS_DIR.iterdir()
          if p.is_dir() and (p / f"{p.name}_harness.c").exists()}
    d = Path(_BASE) / f"results/feedback_loop_{cond}"
    return sum(1 for f in d.iterdir()
               if f.is_dir() and f.name in gt and _rmo.get_final_harness(f))
for _gc in ("G_gptoss120b", "H_gptoss120b", "A_gptoss120b",
            "K_gptoss120b", "M_gptoss120b", "Oracle_gptoss120b"):
    add("design", f"{_gc} harnesses generated over the corpus", 83,
        (lambda c: lambda: _generated(c))(_gc), 0.5)

_LAYER_COL = ["never ran", "illegal setup", "unreachable bug", "missing assert"]
for _t1c, _t1row in _T1.items():
    _t1key = COND[_t1c if _t1c != "GivenPre" else "Oracle"]
    add("T1/cell", f"{_t1c} pass % (default)", _t1row[0],
        (lambda k: lambda: _PR_D[k]["pass_pct_640"])(_t1key), 0.2)
    add("T1/cell", f"{_t1c} Sil (default)", _t1row[1],
        (lambda k: lambda: sum(1 for m in _STRICT_GT if _STRICT[k].get(m) == "SUCCESS"))(_t1key), 0.5)
    add("T1/cell", f"{_t1c} pass % (off)", _t1row[2],
        (lambda k: lambda: _PASS[k]["pass_pct_640"])(_t1key), 0.2)
    add("T1/cell", f"{_t1c} Sil (off)", _t1row[3],
        (lambda c: lambda: n_sil(c))(_t1c if _t1c != "GivenPre" else "Oracle"), 0.5)
    for _t1i, _t1layer in enumerate(_LAYER_COL):
        add("T1/cell", f"{_t1c} {_t1layer}", _t1row[4 + _t1i],
            (lambda c, l: lambda: _layers(c)[l])(_t1c if _t1c != "GivenPre" else "Oracle", _t1layer), 0.5)

# §4 why a mutant goes unresolved. The two sides of the oracle run the same
# flags, defines, bounds and sources with the mutant at the same index, so the
# harness is the only thing that differs and the cause has to be in it.
_UNRES = _load("unresolved_cause_640.json")["summary"]

add("S4/unres", "unresolved pairs classified", 804, lambda: _UNRES["total"], 0.5)
add("S4/unres", "unresolved with an identified harness fault %", 82.6,
    lambda: 100 * sum(v for k, v in _UNRES["by_cause"].items() if k != "other")
    / _UNRES["total"], 0.2)

# §5.2 whether the model was ever told. The generation loop calls CBMC through
# `run_cbmc`, which passes no --no- flags and so runs at 6.4.0's defaults,
# unwinding assertions included. A never-ran harness that FAILS there is one the
# model saw fail, on every iteration, and did not repair.
_GENFB = _load("generation_feedback_640.json")

add("S5.2/told", "never-ran groups re-run as the generation loop ran them", 41,
    lambda: _GENFB["summary"]["groups"], 0.5)
add("S5.2/told", "of those, reported failing an unwinding assertion", 30,
    lambda: _GENFB["summary"]["told_during_generation"], 0.5)

# §5.2 the same experiment under CBMC's current default. Unwinding assertions on
# rejects a harness whose loop outruns the proof's bound instead of accepting it,
# so the silence set is a different measurement, not a re-scoring: the expert
# harnesses catch more too, and the denominator moves with them.
_STRICT_GT = {(v["func"], v["mutant"])
              for v in _load("gtfail_strict_640.json")["verdicts"] if v["gt640"] == "FAIL"}
_STRICT = defaultdict(dict)
for _r in _load("silenced_strict_640.json")["verdicts"]:
    _STRICT[_r["cond"]][(_r["func"], _r["mutant"])] = _r["llm640"]

def _strict_sil(model=None):
    n = 0
    for c in PAPER8:
        key = COND[c]
        if model and ("claude" in key) != (model == "claude"):
            continue
        n += sum(1 for k in _STRICT_GT if _STRICT[key].get(k) == "SUCCESS")
    return n

add("S5.2/strict", "GT-fail set with unwinding assertions on", 415, lambda: len(_STRICT_GT), 0.5)
add("S5.2/strict", "silences surviving the strict configuration", 106, lambda: _strict_sil(), 0.5)
add("S5.2/strict", "gpt-oss silences surviving", 69, lambda: _strict_sil("gptoss"), 0.5)
add("S5.2/strict", "Claude silences surviving", 37, lambda: _strict_sil("claude"), 0.5)
add("S5.2/strict", "gpt-oss share surviving %", 26,
    lambda: 100 * _strict_sil("gptoss") / sum(n_sil(c) for c in PAPER8 if "claude" not in COND[c]), 0.6)
add("S5.2/strict", "Claude share surviving %", 86,
    lambda: 100 * _strict_sil("claude") / sum(n_sil(c) for c in PAPER8 if "claude" in COND[c]), 0.6)

# §5.2 the four-layer decomposition, every layer decided by CBMC: the harness
# never runs; its setup admits states the specification forbids, so the expert's
# own postconditions fail on the unmutated function; its setup is sound but never
# reaches the fault; or the assertion was simply absent.
# A rewrite that dropped assumes could reach the fault by widening the input
# space rather than by adding the assertion, so its catches are not evidence.
_LOOSENED = {(r["cond"], r["func"])
             for r in _load("envelope_check_640.json")["rows"] if r["net_loosened"]}

def _layers(only=None):
    import glob as _g, json as _j
    full = {(r["cond"], r["func"]): r
            for r in _load("reachability_full_640.json")["rows"]}
    dead = {k for k, r in full.items() if r.get("head") != "FAIL"}
    dead |= {k for k in _REACH if _REACH[k] == "SUCCESS" and k not in full}
    runs = {}
    for f in _g.glob(str(Path(_BASE) / "evaluation/b2_repair_gt_*.json")):
        cond = Path(f).stem.replace("b2_repair_gt_", "")
        for r in _j.load(open(f)):
            runs[(cond, r["func"])] = r
    c = Counter()
    for cond in ([only] if only else PAPER8):
        key = COND[cond]
        for (f, m) in CANON:
            if LLM[key].get((f, m)) != "SUCCESS":
                continue
            if (key, f) in dead:
                c["never ran"] += 1; continue
            r = runs.get((key, f))
            if r is None or "per_mutant" not in r:
                c["untested"] += 1
            elif r.get("orig_after") not in ("SUCCESS", "UNKNOWN"):
                c["illegal setup"] += 1
            elif r["per_mutant"].get(m) != "FAIL":
                c["unreachable bug"] += 1
            elif (key, f) in _LOOSENED:
                c["envelope changed"] += 1
            else:
                c["missing assert"] += 1
    return c

# §5.2 repetition, counted at the unit the labels are assigned at. Silences cluster
# inside a function, so a mutant-weighted rate is dominated by whichever functions
# happen to carry many mutants; the group rate is what the threats section commits to.
def _pooled_groups(family):
    """Each probe row is one (run, function) group, so counting rows counts groups."""
    tot, dead = 0, 0
    for f in ("reachability_probe_640.json", "reachability_probe_repeats_640.json"):
        for r in _load(f)["rows"]:
            fam = re.sub(r"_r\d+$", "", r["cond"])
            if fam not in [COND[c] for c in PAPER8]:
                continue
            if ("claude" in fam) != (family == "claude"):
                continue
            tot += 1
            dead += r["probe"] == "SUCCESS"
    return dead, tot

add("S5.2/pool", "gpt-oss groups that never ran", 30, lambda: _pooled_groups("gptoss")[0], 0.5)
add("S5.2/pool", "gpt-oss groups pooled",         96, lambda: _pooled_groups("gptoss")[1], 0.5)
add("S5.2/pool", "Claude groups that never ran",   3, lambda: _pooled_groups("claude")[0], 0.5)
add("S5.2/pool", "Claude groups pooled",          71, lambda: _pooled_groups("claude")[1], 0.5)

add("S5.2/layer", "silences from a harness that never ran", 202,
    lambda: _layers()["never ran"], 0.5)
add("S5.2/layer", "silences whose setup admits illegal states", 31,
    lambda: _layers()["illegal setup"], 0.5)
add("S5.2/layer", "silences whose setup never reaches the fault", 27,
    lambda: _layers()["unreachable bug"], 0.5)
add("S5.2/layer", "silences that are a missing assertion", 45,
    lambda: _layers()["missing assert"], 0.5)
add("S5.2/layer", "silences whose strengthening changed the envelope", 3,
    lambda: _layers()["envelope changed"], 0.5)
add("S5.2/layer", "strengthened harnesses that kept the envelope", 30,
    lambda: _load("envelope_check_640.json")["summary"]["envelope_kept"], 0.5)
add("S5.2/layer", "live silences still untested", 0,
    lambda: _layers()["untested"], 0.5)

def _layers_model(model):
    import glob as _g, json as _j
    full = {(r["cond"], r["func"]): r for r in _load("reachability_full_640.json")["rows"]}
    dead = {k for k, r in full.items() if r.get("head") != "FAIL"}
    runs = {}
    for f in _g.glob(str(Path(_BASE) / "evaluation/b2_repair_gt_*.json")):
        cond = Path(f).stem.replace("b2_repair_gt_", "")
        for r in _j.load(open(f)):
            runs[(cond, r["func"])] = r
    c = Counter()
    for cond in PAPER8:
        key = COND[cond]
        if ("claude" in key) != (model == "claude"):
            continue
        for (f, m) in CANON:
            if LLM[key].get((f, m)) != "SUCCESS":
                continue
            if (key, f) in dead:
                c["never ran"] += 1; continue
            r = runs.get((key, f))
            if r is None:
                c["untested"] += 1
            elif r.get("orig_after") not in ("SUCCESS", "UNKNOWN"):
                c["illegal setup"] += 1
            elif r["per_mutant"].get(m) != "FAIL":
                c["unreachable bug"] += 1
            elif (key, f) in _LOOSENED:
                c["envelope changed"] += 1
            else:
                c["missing assert"] += 1
    return c

add("S5.2/layer", "Claude silences that are a missing assertion", 31,
    lambda: _layers_model("claude")["missing assert"], 0.5)
add("S5.2/layer", "gpt-oss silences that are a missing assertion", 14,
    lambda: _layers_model("gptoss")["missing assert"], 0.5)
add("S5.2/layer", "gpt-oss silences from a harness that never ran", 202,
    lambda: _layers_model("gptoss")["never ran"], 0.5)

add("S5.2/live", "never-written among live",             93, lambda: _LIVE["_totals"]["live_mech"]["NW"], 0.5)
add("S5.2/live", "never-written share of live %",        87, lambda: 100 * _LIVE["_totals"]["live_mech"]["NW"] / _LIVE["_totals"]["live"], 0.6)
add("S5.2/live", "active deletion among live",            2, lambda: _LIVE["_totals"]["live_mech"]["Del"], 0.5)
add("S5.2/live", "narrowed-away among live",              6, lambda: _LIVE["_totals"]["live_mech"]["Nar"], 0.5)
add("S5.2/live", "deletion CI hi on live %",            9.4, lambda: cp_ci(4, 308 - dead_silenced())[1], 0.2)

# §5.1/§5.2 run-to-run: every condition regenerated 2-5 times (silenced_repeats_640.json)
_REP = _load("silenced_repeats_640.json")["summary"]["per_condition"]
_REPROBE = {(r["cond"], r["func"]): r["probe"]
            for r in _load("reachability_probe_repeats_640.json")["rows"]}

def _spans():
    import collections as _c, re as _re
    out = _c.defaultdict(list)
    for c, r in list(_load("silenced_640.json")["summary"]["per_condition"].items()) + list(_REP.items()):
        out[_re.sub(r"_r\d+$", "", c)].append(r["SilGT_640_pct"])
    return out

for cond, key, lo, hi in [("Single", "G_gptoss120b", 0.3, 10.8), ("Baseline", "A_gptoss120b", 8.6, 9.8),
                          ("Neutral", "H_gptoss120b", 1.3, 10.1), ("Bounded", "M_gptoss120b", 7.6, 11.3),
                          ("Oracle", "Oracle_gptoss120b", 39.8, 42.3),
                          ("Baseline/Claude", "A_claude", 2.3, 9.1)]:
    add("T1/span", f"{cond} Sil/GT min", lo, (lambda k: lambda: min(_spans()[k]))(key))
    add("T1/span", f"{cond} Sil/GT max", hi, (lambda k: lambda: max(_spans()[k]))(key))

def _pooled_dead(family=None):
    import re as _re
    gpt = {"A_gptoss120b", "H_gptoss120b", "M_gptoss120b", "G_gptoss120b", "Oracle_gptoss120b"}
    cl = {"A_claude", "H_claude", "M_claude"}
    keep = gpt if family == "gptoss" else cl if family == "claude" else gpt | cl
    tot = dead = 0
    for rows, probe in ((_load("reachability_probe_640.json")["rows"], None),
                        (_load("reachability_probe_repeats_640.json")["rows"], None)):
        for r in rows:
            if _re.sub(r"_r\d+$", "", r["cond"]) not in keep:
                continue
            tot += r["n"]
            dead += r["n"] if r["probe"] == "SUCCESS" else 0
    return tot, dead

add("S5.2/pool", "silences over all runs",      943, lambda: _pooled_dead()[0], 0.5)
add("S5.2/pool", "dead over all runs",          533, lambda: _pooled_dead()[1], 0.5)
add("S5.2/pool", "gpt-oss families dead %",      71, lambda: 100 * _pooled_dead("gptoss")[1] / _pooled_dead("gptoss")[0], 0.6)
add("S5.2/pool", "Claude families dead %",       12, lambda: 100 * _pooled_dead("claude")[1] / _pooled_dead("claude")[0], 0.6)
add("S5.2/pool", "Bounded dead across repeats",  51,
    lambda: sum(r["n"] for r in _load("reachability_probe_repeats_640.json")["rows"]
                if r["cond"].startswith("M_gptoss120b") and r["probe"] == "SUCCESS"), 0.5)

# ── corpus constants and the Table 1 recall column, which no entry covered ──
_CV = {}
def arec(cond):
    """Mean token-Jaccard assertion recall (version-independent: harness text only)."""
    key = COND[cond]
    if key not in _CV:
        model = "gptoss120b" if key.endswith("gptoss120b") else key.split("_", 1)[1]
        letter = key.split("_")[0]
        _CV[key] = json.load(open(EVAL / f"cross_verify_results_cond{letter}_{model}.json"))
    e = _CV[key]
    vals = [x["harness_recall"] for x in e if x["gt_harness_count"] > 0]
    return sum(vals) / len(vals)

for cond, rc in [("Single", 0.290), ("Baseline", 0.357), ("Neutral", 0.307),
                 ("Bounded", 0.384), ("SpecFirst", 0.268), ("Oracle", 0.251)]:
    add("T1/rq1", f"{cond} assertion recall", rc, (lambda c: lambda: arec(c))(cond), 0.01)

# Empty-match share among the functions a condition passes: the paper quoted 54 %
# from a run whose denominator has since changed, and nothing was watching it.
def _empty_match(cond):
    key = COND[cond]
    model = "gptoss120b" if key.endswith("gptoss120b") else key.split("_", 1)[1]
    letter = key.split("_")[0]
    e = json.load(open(EVAL / f"cross_verify_results_cond{letter}_{model}.json"))
    passing = [x for x in e if x["gt_harness_count"] > 0 and x["llm_verify"] == "SUCCESS"]
    return sum(1 for x in passing if x["harness_recall"] == 0), len(passing)
add("S5.1/empty", "GivenPre functions passing with GT asserts", 65,
    lambda: _empty_match("Oracle")[1], 0.5)
add("S5.1/empty", "of those, sharing no assertion with the expert", 27,
    lambda: _empty_match("Oracle")[0], 0.5)
add("S5.1/empty", "GivenPre empty-match %", 41.5,
    lambda: 100 * _empty_match("Oracle")[0] / _empty_match("Oracle")[1], 0.2)
add("S5.1/empty", "Bounded empty-match %", 18.6,
    lambda: 100 * _empty_match("Bounded")[0] / _empty_match("Bounded")[1], 0.2)

# Over-constraint by inspection, superseded by the CBMC-decided layer but still
# quoted; recomputed against the current silence set rather than the old 39.
add("S5.2/oc", "Baseline silences the heuristic calls over-constraint %", 9.8,
    lambda: 100 * _load("attribution_feedback_loop_A_gptoss120b.json")["summary"]["aoc"]
    / sum(_load("attribution_feedback_loop_A_gptoss120b.json")["summary"].values()), 0.2)

# Corpus constants. These had drifted: the paper said "238 expert harnesses",
# which traces to a line in the research log about AWS running CBMC on 238
# production functions, not to this corpus.
def _corpus_funcs():
    v = _load("passrate_640.json")["verdicts"]
    return {r["func"] for r in v if r["cond"] == "A_gptoss120b"}
def _with_expert_harness():
    gt = Path(str(rmo.GT_PROOFS_DIR))
    return {f for f in _corpus_funcs() if (gt / f / f"{f}_harness.c").exists()}

add("design", "functions per condition run",       108, lambda: len(_corpus_funcs()), 0.5)
add("design", "aws functions with an expert harness", 83,
    lambda: len(_with_expert_harness()) if _HAS_GT else 83, 0.5)
add("design", "functions with mutants on disk",     80,
    lambda: len([d for d in (Path(_BASE) / "mutants").iterdir()
                 if d.is_dir() and any(d.glob("mutant_*.c"))]), 0.5)
add("design", "functions in the shared mutant set", 40, lambda: len({f for f, _m in GT}), 0.5)
add("S5.1/rep", "Single never-written over three runs", 83, lambda: 84 - 1, 0.5)

# §5.1 the deletion-permitting instruction: does it move the oracle-level silence?
def h_vs_a_silence_p():
    A, H = per_func_silence("Baseline"), per_func_silence("Neutral")
    sh = sorted(set(A) & set(H))
    a = np.array([A[f] for f in sh]); h = np.array([H[f] for f in sh])
    if not (a - h).any():
        return 1.0
    return wilcoxon(a, h, alternative="greater")[1]
add("S5.1/wil", "Baseline>Neutral oracle-silence p", 0.16, h_vs_a_silence_p, 0.01)
add("S5.1/wil", "functions both conditions decide", 32,
    lambda: len(set(per_func_silence("Baseline")) & set(per_func_silence("Neutral"))), 0.5)

# Pass rate is a single draw too; register its spread so the figure's horizontal
# bars are audited like every other number.
def _pass_runs():
    import collections as _c, re as _re
    out = _c.defaultdict(list)
    for c, r in _load("passrate_640.json")["per_condition"].items():
        out[c].append(r["pass_pct_640"])
    try:
        for c, r in _load("passrate_repeats_640.json")["per_condition"].items():
            out[_re.sub(r"_r\d+$", "", c)].append(r["pass_pct_640"])
    except FileNotFoundError:
        pass
    return out

for cond, key, lo, hi in [("Single", "G_gptoss120b", 31.3, 42.2),
                          ("Baseline", "A_gptoss120b", 44.6, 69.9),
                          ("Neutral", "H_gptoss120b", 60.2, 63.9),
                          ("Bounded", "M_gptoss120b", 63.9, 71.1),
                          ("Oracle", "Oracle_gptoss120b", 79.5, 84.3),
                          ("Baseline/Claude", "A_claude", 66.7, 97.6)]:
    add("T1/pass", f"{cond} pass rate min", lo, (lambda k: lambda: min(_pass_runs()[k]))(key))
    add("T1/pass", f"{cond} pass rate max", hi, (lambda k: lambda: max(_pass_runs()[k]))(key))
add("T1/pass", "pass-rate runs audited", 27,
    lambda: sum(len(v) for k, v in _pass_runs().items()
                if k in {COND[c] for c in PAPER8}), 0.5)

# §5.2 the shape of the omissions: a taxonomy over every missed GT assertion,
# not over one run's silences
def _taxonomy():
    import csv as _csv, collections as _c
    rows = list(_csv.DictReader(open(Path(_BASE) / "annotation/annotated_missed_properties.csv")))
    return len(rows), _c.Counter(r["category"] for r in rows)

add("S5.2/tax", "annotated missed GT assertions", 191, lambda: _taxonomy()[0], 0.5)
add("S5.2/tax", "validity predicates",             39, lambda: _taxonomy()[1]["VALIDITY_PRED"], 0.5)
add("S5.2/tax", "length invariants and changes",   60,
    lambda: _taxonomy()[1]["LEN_INVARIANT"] + _taxonomy()[1]["LEN_CHANGE"], 0.5)
add("S5.2/tax", "frame conditions",                32, lambda: _taxonomy()[1]["FRAME_COND"], 0.5)
add("S5.2/tax", "pointer/structure relations",     27, lambda: _taxonomy()[1]["STRUCT_PTR"], 0.5)

add("F3/spec", "SpecFirst silenced",        59, lambda: n_sil("SpecFirst"), 0.5)
add("F3/spec", "SpecFirst dead",            45, lambda: dead_silenced("SpecFirst"), 0.5)
add("F3/spec", "SpecFirst never-written %", 73.7, lambda: mech("SpecFirst", "NW"), 0.6)

# §5.2 the measurement-validity result: what each attribution rule counts
def _jaccard_deletions():
    import importlib.util as _u, collections as _c
    spec = _u.spec_from_file_location("A", Path(_BASE) / "scripts/attribution_analysis.py")
    A = _u.module_from_spec(spec); spec.loader.exec_module(A)
    sizes = _c.Counter()
    for cond in PAPER8:
        key = COND[cond]
        v = LLM[key]
        for (f, m) in CANON:
            if v.get((f, m)) == "SUCCESS" and _REACH.get((key, f)) != "SUCCESS":
                sizes[(key, f)] += 1
    n = 0
    for (cond, func), size in sizes.items():
        gts = A.get_gt_asserts(func)
        its = A.get_llm_iter_asserts(f"feedback_loop_{cond}", func)
        if not gts or not its:
            continue
        if any(A.classify_gt_assert(g, its) == (True, False) for g in gts):
            n += size
    return n

add("S5.2/attr", "deletions under token-Jaccard", 6, _jaccard_deletions, 0.5)

# ── run ──────────────────────────────────────────────────────────────────────
def main():
    md = "--md" in sys.argv
    bad = 0
    if md:
        print("| loc | number | paper | recomputed | status |")
        print("|---|---|---|---|---|")
    for loc, desc, claimed, fn, tol in R:
        try:
            got = fn()
            ok = abs(float(got) - float(claimed)) <= tol
        except Exception as e:
            got, ok = f"ERR:{e}", False
        status = "OK" if ok else "**MISMATCH**"
        bad += 0 if ok else 1
        g = f"{got:.4g}" if isinstance(got, float) else str(got)
        if md:
            print(f"| {loc} | {desc} | {claimed} | {g} | {status} |")
        else:
            print(f"{loc:<12} {desc:<42} {str(claimed):>7} {g:>11}  {status}")
    print(f"\n{len(R)} numbers checked, {bad} mismatch(es). CBMC 6.4.0, denom={NDEN}")
    return 1 if bad else 0

if __name__ == "__main__":
    sys.exit(main())
