#!/usr/bin/env python3
"""
envelope_check_640.py — did the strengthening rewrite keep the input space fixed?

The GT-guided strengthening asks the model to add the expert's assertions and to
leave every `__CPROVER_assume` alone. Nothing enforced that. It matters
asymmetrically: a rewrite that *loosens* the envelope can reach a faulty state
the original harness could not, so a catch is then evidence about the rewrite
rather than about the original harness, and the "missing assertion" label is
unsafe. A rewrite that *tightens* it can only make catching harder, so those
verdicts stay conservative.

This compares the assume set of each strengthened harness against the final
iteration it was built from and reports the groups that lost assumes on net.

Usage:  python3 scripts/envelope_check_640.py
"""
import collections, glob, json, re, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE / "figures"))
import style as S  # noqa: E402

EXP = HERE.parent

def assumes(text):
    return collections.Counter(re.sub(r"\s+", "", l) for l in text.splitlines()
                               if "__CPROVER_assume" in l)

def main():
    canon, llm = S.gt_fail_set(), S.llm_verdicts()
    rows, loosened = [], []
    for path in sorted(glob.glob(str(EXP / "evaluation/b2_repair_gt_*.json"))):
        cond = Path(path).stem.replace("b2_repair_gt_", "")
        for r in json.load(open(path)):
            d = EXP / "results" / f"feedback_loop_{cond}" / r["func"]
            rep = d / "repaired_b2_harness.c"
            its = sorted(d.glob("iter_*_harness.c"),
                         key=lambda x: int(re.search(r"iter_(\d+)_", x.name).group(1)))
            if not rep.exists() or not its:
                continue
            a0 = assumes(its[-1].read_text(errors="replace"))
            a1 = assumes(rep.read_text(errors="replace"))
            dropped, added = sum((a0 - a1).values()), sum((a1 - a0).values())
            n = sum(1 for (f, m) in canon
                    if f == r["func"] and llm.get(cond, {}).get((f, m)) == "SUCCESS")
            rows.append({"cond": cond, "func": r["func"], "silences": n,
                         "assumes_dropped": dropped, "assumes_added": added,
                         "net_loosened": dropped > added})
            if dropped > added:
                loosened.append(rows[-1])

    kept = len(rows) - len(loosened)
    n_loose = sum(r["silences"] for r in loosened)
    print(f"{kept} of {len(rows)} strengthened harnesses keep the envelope at least as tight")
    if loosened:
        print(f"net-loosened: {len(loosened)} groups, {n_loose} silences")
        for r in loosened:
            print(f"   {r['cond']}/{r['func']}  -{r['assumes_dropped']} +{r['assumes_added']}"
                  f"  ({r['silences']} silences)")
    json.dump({"summary": {"groups": len(rows), "envelope_kept": kept,
                           "net_loosened_groups": len(loosened),
                           "net_loosened_silences": n_loose},
               "rows": rows}, open(EXP / "evaluation/envelope_check_640.json", "w"), indent=1)

if __name__ == "__main__":
    main()
