#!/usr/bin/env python3
"""
mechanism_revised_640.py — the mechanism split, re-derived at the unit we actually
sample and with the residual category held to account.

Three things the earlier analysis did not do:

  1. It called a harness live on one probe, at the first postcondition. A harness
     can reach its first assertion and still have its tail cut off, and the
     never-written label presumes the missing assertion had somewhere to go, so we
     require the LAST assertion to be reachable.
  2. It counted silences, which are mutants, while the labels are assigned per
     (condition, function) group. Mutant counts weight a group by how many mutants
     its function happens to carry, so we report both and bootstrap over functions.
  3. It reported never-written pooled over conditions, which hides that the number
     rests on the Oracle control and one lucky Bounded run. We report the
     provenance.

Where the strengthening runs exist (`b2_repair_*.json`, which add assertions while
holding `__CPROVER_assume` fixed) they split the live silences further: a mutant
caught after strengthening proves the setup reaches the bad state, so the gap was
the assertion; one still missed does not, and cannot be called never-written.

Usage:  python3 scripts/mechanism_revised_640.py
"""
import collections, json, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "figures"))
import style as S  # noqa: E402
import numpy as np  # noqa: E402

EXP = HERE.parent
PAPER8 = ["Oracle_gptoss120b", "A_gptoss120b", "H_gptoss120b", "M_gptoss120b",
          "G_gptoss120b", "A_claude", "H_claude", "M_claude"]
ORGANIC = ["A_gptoss120b", "H_gptoss120b", "G_gptoss120b"]

def load_full():
    p = EXP / "evaluation/reachability_full_640.json"
    if not p.exists():
        return {}
    return {(r["cond"], r["func"]): r for r in json.load(open(p))["rows"]}

def reach_class(full, head_dead, key):
    """dead | tail_dead | live, from the three probes with the old one as fallback."""
    r = full.get(key)
    if r is None:
        return "dead" if key in head_dead else "live"
    if r.get("head") != "FAIL":
        return "dead"
    if r.get("tail") not in ("FAIL", None):
        return "tail_dead"
    return "live"

def strengthening():
    """(cond, func) -> (caught, silenced), keeping only runs whose strengthened
    harness still verifies on the unmutated source. A harness that stopped
    verifying there catches every mutant for the wrong reason, so its count is
    evidence of nothing."""
    out, dropped = {}, 0
    for p in (EXP / "evaluation").glob("b2_repair_*.json"):
        cond = p.stem.replace("b2_repair_", "")
        for r in json.load(open(p)):
            if not r.get("valid"):
                dropped += 1
                continue
            out[(cond, r["func"])] = (r["n_caught"], r["n_silenced"])
    if dropped:
        print(f"(dropped {dropped} strengthening runs invalid on the original source)")
    return out

def cluster_bootstrap(units, stat, n=5000, seed=0):
    """Resample FUNCTIONS, not mutants: silences cluster inside a function, so a
    mutant-level interval is far too tight."""
    rng = np.random.default_rng(seed)
    by_func = collections.defaultdict(list)
    for func, item in units:
        by_func[func].append(item)
    funcs = list(by_func)
    vals = []
    for _ in range(n):
        pick = rng.choice(len(funcs), len(funcs), replace=True)
        pool = [x for i in pick for x in by_func[funcs[i]]]
        v = stat(pool)
        if v is not None:
            vals.append(v)
    return (float(np.percentile(vals, 2.5)), float(np.percentile(vals, 97.5))) if vals else (0.0, 0.0)

def main():
    canon = S.gt_fail_set()
    llm = S.llm_verdicts()
    head_dead = S.dead_groups()
    full = load_full()
    lab = S.load("adjudicated_mechanism.json")["labels"]
    strong = strengthening()

    rows = []          # one per silenced mutant
    for cond in PAPER8:
        v = llm.get(cond, {})
        for (f, m) in canon:
            if v.get((f, m)) != "SUCCESS":
                continue
            cls = reach_class(full, head_dead, (cond, f))
            rows.append({"cond": cond, "func": f, "mutant": m, "reach": cls,
                         "label": lab.get(cond, {}).get(f, "Unres")})

    def show(title, sel):
        sub = [r for r in sel]
        if not sub:
            return
        groups = {(r["cond"], r["func"]) for r in sub}
        c = collections.Counter(r["reach"] for r in sub)
        gc = collections.Counter()
        for cond, f in groups:
            gc[reach_class(full, head_dead, (cond, f))] += 1
        print(f"\n{title}: {len(sub)} silences in {len(groups)} groups")
        for k in ("dead", "tail_dead", "live"):
            print(f"   {k:10s} mutants {c[k]:4d} ({100*c[k]/len(sub):5.1f}%)   "
                  f"groups {gc[k]:3d} ({100*gc[k]/len(groups):5.1f}%)")

    show("ALL eight conditions", rows)
    show("without the Oracle control", [r for r in rows if not r["cond"].startswith("Oracle")])
    show("organic gpt-oss only (Baseline, Neutral, Single)",
         [r for r in rows if r["cond"] in ORGANIC])
    show("Claude only", [r for r in rows if r["cond"].endswith("_claude")])

    live = [r for r in rows if r["reach"] == "live"]
    print(f"\n--- the {len(live)} live silences, by adjudicated label")
    lc = collections.Counter(r["label"] for r in live)
    for k, n in lc.most_common():
        print(f"   {k:8s} {n:3d}  ({100*n/len(live):5.1f}% of live)")
    nw = [r for r in live if r["label"] == "NW"]
    ci = cluster_bootstrap([(r["func"], r) for r in live],
                           lambda p: 100 * sum(1 for x in p if x["label"] == "NW") / len(p) if p else None)
    print(f"   never-written {len(nw)}/{len(live)} = {100*len(nw)/len(live):.1f}% "
          f"(function-clustered 95% CI [{ci[0]:.1f}, {ci[1]:.1f}])")

    print("\n--- provenance of the never-written silences")
    for cond in PAPER8:
        n = sum(1 for r in nw if r["cond"] == cond)
        g = len({r["func"] for r in nw if r["cond"] == cond})
        if n:
            print(f"   {S.COND_LABEL[cond]+'/'+S.model_of(cond):22s} {n:3d} silences in {g} groups")

    print("\n--- strengthening test on the live silences (assumes held fixed)")
    tested = [r for r in live if (r["cond"], r["func"]) in strong]
    if tested:
        by = collections.defaultdict(lambda: [0, 0])
        for (cond, f), (c, n) in strong.items():
            if reach_class(full, head_dead, (cond, f)) == "live":
                by[cond][0] += c; by[cond][1] += n
        for cond, (c, n) in sorted(by.items()):
            verdict = "assertion was the gap" if c else "setup never reaches the bad state"
            print(f"   {cond:20s} {c:3d}/{n:3d} caught after strengthening   {verdict}")
        tot_c = sum(c for c, _ in by.values()); tot_n = sum(n for _, n in by.values())
        print(f"   tested {tot_n} of {len(live)} live silences; {tot_c} confirm the assertion gap")
    tested_n = sum(n for (cond, f), (c, n) in strong.items()
                   if reach_class(full, head_dead, (cond, f)) == "live")
    print(f"\n   live silences with no strengthening run: {len(live) - tested_n}")

if __name__ == "__main__":
    main()
