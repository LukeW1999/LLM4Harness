#!/usr/bin/env python3
"""Mutation-operator / defect-class analysis of the SILENCED bug set.

Answers the recurring reviewer concern "is a seeded mutant a real bug?":
for every silenced (func,mutant) we diff the mutant against the original
source line, classify the syntactic operator (objective, from the diff),
flag whether it touches a verification CONTRACT line (non-behavioral) vs
behavioral code, and give a heuristic CWE-class mapping.

Run on server: python3 operator_analysis.py
Writes evaluation/operator_class_silenced.json + prints tables.
"""
import json, glob, os, re, difflib
from collections import Counter, defaultdict

EXP = "/root/experiment_aws_cbmc"
SRC = "/root/aws-c-common/source"
MUT = os.path.join(EXP, "mutants")
EVAL = os.path.join(EXP, "evaluation")

# ---- 1. collect silenced (func,mutant) per condition + union ----
def cond_name(fp):
    return os.path.basename(fp).split("feedback_loop_")[1].replace(".json", "")

per_cond = {}
union = set()
for fp in sorted(glob.glob(os.path.join(EVAL, "mutation_oracle_cbmc_feedback_loop_*.json"))):
    if any(x in fp for x in [".bak", "_r2", "_r3", "_r4", "_r5"]):
        continue
    try:
        d = json.load(open(fp))
    except Exception:
        continue
    sil = {(r["func"], r["mutant"]) for r in d.get("results", []) if r.get("silenced")}
    if sil:
        per_cond[cond_name(fp)] = sil
        union |= sil

# ---- 2. map func -> original source basename via metadata.json ----
func_src = {}
for func in os.listdir(MUT):
    mj = os.path.join(MUT, func, "metadata.json")
    if os.path.isfile(mj):
        try:
            m = json.load(open(mj))
            func_src[func] = os.path.basename(m.get("source_file", ""))
        except Exception:
            pass

# ---- 3. diff one mutant against original, return changed (old,new) line pairs ----
CONTRACT_RE = re.compile(r"AWS_PRECONDITION|AWS_POSTCONDITION|AWS_FATAL_|AWS_ASSUME|__CPROVER|AWS_ASSERT")

def changed_lines(func, mutant):
    base = func_src.get(func)
    if not base:
        return None
    orig_p = os.path.join(SRC, base)
    mut_p = os.path.join(MUT, func, mutant + ".c")
    if not (os.path.isfile(orig_p) and os.path.isfile(mut_p)):
        return None
    a = open(orig_p, errors="ignore").read().splitlines()
    b = open(mut_p, errors="ignore").read().splitlines()
    sm = difflib.SequenceMatcher(a=a, b=b, autojunk=False)
    pairs = []
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == "equal":
            continue
        old = " ".join(a[i1:i2]).strip()
        new = " ".join(b[j1:j2]).strip()
        pairs.append((tag, old, new))
    return pairs

# ---- 4. operator classifier (objective, from the literal diff) ----
ROR = set("< <= > >= == !=".split())

def classify(tag, old, new):
    """Return (operator_class, is_contract)."""
    is_contract = bool(CONTRACT_RE.search(old) or CONTRACT_RE.search(new))
    if tag == "delete" or (tag == "replace" and not new.strip()):
        return "SDL (statement deletion)", is_contract
    if tag == "insert":
        return "SIN (statement insertion)", is_contract
    # token-level deltas on a replace
    def toks(s):
        return re.findall(r"<=|>=|==|!=|&&|\|\||[-+*/%<>=!]|\b\d+\b|\w+", s)
    ot, nt = toks(old), toks(new)
    so, sn = set(ot), set(nt)
    added, removed = sn - so, so - sn
    moved = added | removed
    # relational operator replacement
    if (moved & ROR) and not (moved - ROR - {"true", "false"}):
        return "ROR (relational operator)", is_contract
    if moved & {"&&", "||"}:
        return "LCR (logical connector)", is_contract
    if moved & {"+", "-", "*", "/", "%"}:
        return "AOR (arithmetic operator)", is_contract
    if "!" in moved:
        return "UOI (unary/negation)", is_contract
    # constant replacement / off-by-one
    onums = [t for t in ot if t.isdigit()]
    nnums = [t for t in nt if t.isdigit()]
    if onums != nnums:
        try:
            if len(onums) == len(nnums) and any(abs(int(x) - int(y)) == 1 for x, y in zip(onums, nnums)):
                return "CRP off-by-one (constant +/-1)", is_contract
        except Exception:
            pass
        return "CRP (constant replacement)", is_contract
    # identifier / variable swap
    if added and removed:
        return "VAR/other (identifier swap)", is_contract
    return "other", is_contract

# heuristic CWE-class mapping from operator + line keywords
SIZE_RE = re.compile(r"\b(len|length|size|capacity|count|index|idx|num|bound|offset|n_|max|min)\b", re.I)

def cwe_class(opclass, old, new):
    line = (old + " " + new).lower()
    if opclass.startswith("AOR"):
        return "CWE-190 integer overflow/wraparound"
    if opclass.startswith("CRP off-by-one") or (opclass.startswith("ROR") and SIZE_RE.search(line)):
        return "CWE-125/787 out-of-bounds (off-by-one / bound check)"
    if opclass.startswith("ROR"):
        return "CWE-697 incorrect comparison"
    if opclass.startswith("LCR") or opclass.startswith("UOI"):
        return "CWE-571/617 wrong condition / broken check"
    if opclass.startswith("SDL"):
        return "CWE-252/690 missing check or unchecked operation"
    return "other / unmapped"

# ---- 5. run over the union + a few headline conditions ----
def analyze(label, pairset):
    ops = Counter()
    cwes = Counter()
    n_contract = 0
    n_total = 0
    n_unresolved = 0
    for func, mutant in sorted(pairset):
        pairs = changed_lines(func, mutant)
        if not pairs:
            n_unresolved += 1
            continue
        # take the most informative changed pair (longest old text)
        tag, old, new = max(pairs, key=lambda p: len(p[1]) + len(p[2]))
        opc, is_contract = classify(tag, old, new)
        n_total += 1
        if is_contract:
            n_contract += 1
        ops[opc] += 1
        cwes[cwe_class(opc, old, new)] += 1
    return {
        "label": label,
        "n_silenced": len(pairset),
        "n_resolved": n_total,
        "n_unresolved": n_unresolved,
        "contract_line_share": round(n_contract / n_total, 3) if n_total else None,
        "n_contract": n_contract,
        "operators": dict(ops.most_common()),
        "cwe_classes": dict(cwes.most_common()),
    }

out = {}
out["UNION_all_conditions"] = analyze("UNION (all conds)", union)
for c in ["A_claude", "A_gptoss120b", "Oracle_gptoss120b"]:
    if c in per_cond:
        out[c] = analyze(c, per_cond[c])

# ---- 6. print ----
def show(rep):
    print(f"\n=== {rep['label']}  (silenced={rep['n_silenced']}, resolved={rep['n_resolved']}, unresolved={rep['n_unresolved']}) ===")
    print(f"  CONTRACT-line (non-behavioral) share: {rep['contract_line_share']}  ({rep['n_contract']}/{rep['n_resolved']})")
    print("  operator classes:")
    for k, v in rep["operators"].items():
        print(f"    {v:4d}  {k}")
    print("  CWE-class (heuristic):")
    for k, v in rep["cwe_classes"].items():
        print(f"    {v:4d}  {k}")

for key in out:
    show(out[key])

with open(os.path.join(EVAL, "operator_class_silenced.json"), "w") as f:
    json.dump(out, f, indent=2)
print("\nwrote evaluation/operator_class_silenced.json")
