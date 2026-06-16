#!/usr/bin/env python3
"""#46 PINNED-A: does the headline inversion rho = Spearman(pass%, assertion-recall)
survive provider/precision pinning (OpenRouter DeepInfra, bf16)?

Honest handling of truncated generation (some funcs hit `timeout 600` and have no
summary.json -> survivorship would inflate pass%): compute on the COMMON-COMPLETE
intersection -- funcs with a final-iteration verdict in ALL 8 pinned conditions --
and compute the MATCHED unpinned rho on the SAME set, so pinned-vs-unpinned isolates
the pinning effect from the set-restriction effect.

  pass%            = #funcs with final verify in {SUCCESS,UNSAT} / |set|   (paper_numbers.passrate semantics)
  assertion-recall = mean harness_recall over set funcs with gt_harness_count>0 (paper_numbers.arec semantics)
  rho              = Spearman(pass%, assertion-recall) over the 8 conditions

Run from experiment root:  python3 scripts/pinned_rho.py
compute() returns the cited numbers dict (imported by paper_numbers.py for auditing).
NOTE: canonical on the LOCAL repo; the server's unpinned condition-A is contaminated.
"""
import json, glob, os
from pathlib import Path
from scipy.stats import spearmanr

BASE = Path(__file__).resolve().parent.parent
RES  = BASE / "results"
EVAL = BASE / "evaluation"
CONDS = ["A", "G", "H", "I", "J", "K", "M", "Oracle"]
PASS_OK = ("SUCCESS", "UNSAT")


def _unpinned_verdicts(cond):
    m = {}
    for sj in glob.glob(str(RES / f"feedback_loop_{cond}_gptoss120b" / "*" / "summary.json")):
        func = os.path.basename(os.path.dirname(sj))
        try:
            its = json.load(open(sj)).get("iterations", [])
            if its:
                m[func] = its[-1].get("verify")
        except Exception:
            pass
    return m


def _recall_map(path):
    if not Path(path).exists():
        return {}
    d = json.load(open(path))
    e = d if isinstance(d, list) else d.get("results", [])
    return {x["func"]: x["harness_recall"] for x in e if x.get("gt_harness_count", 0) > 0}


def compute():
    """Return the cited pinned-rho numbers dict + per-condition series (for printing)."""
    pinned_verd = json.load(open(EVAL / "pinned_passverdicts.json"))   # {cond: {func: verify}}
    unpin_verd = {c: _unpinned_verdicts(c) for c in CONDS}
    pin_rec   = {c: _recall_map(EVAL / f"cross_verify_results_cond{c}_pin.json") for c in CONDS}
    unpin_rec = {c: _recall_map(EVAL / f"cross_verify_results_cond{c}_gptoss120b.json") for c in CONDS}

    # common-complete set: verdict in ALL 8 pinned conds, gt>0 in all pinned recall maps
    common = set(pinned_verd[CONDS[0]])
    for c in CONDS[1:]:
        common &= set(pinned_verd[c])
    for c in CONDS:
        common &= set(pin_rec[c])
    # every common func must also be measurable unpinned
    mset = set(common)
    for c in CONDS:
        mset &= set(unpin_verd[c]) & set(unpin_rec[c])
    mset = sorted(mset)

    def series(verd, rec):
        pas = [100.0 * sum(verd[c].get(f) in PASS_OK for f in mset) / len(mset) for c in CONDS]
        arc = [sum(rec[c][f] for f in mset) / len(mset) for c in CONDS]
        return pas, arc

    pin_pas, pin_arc = series(pinned_verd, pin_rec)
    unp_pas, unp_arc = series(unpin_verd, unpin_rec)
    rho_pin, p_pin = spearmanr(pin_pas, pin_arc)
    rho_unp, p_unp = spearmanr(unp_pas, unp_arc)

    # full-corpus pass% vs UNKNOWN% over the 8 conditions (the robust driver)
    fpas, funk = [], []
    for c in CONDS:
        vs = list(unpin_verd[c].values()); n = len(vs)
        fpas.append(100.0 * sum(v in PASS_OK for v in vs) / n)
        funk.append(100.0 * sum(v == "UNKNOWN" for v in vs) / n)
    rho_pu, p_pu = spearmanr(fpas, funk)

    # condition A on the SAME matched funcs
    def a_on_matched(verd):
        vs = [verd["A"][f] for f in mset]
        return 100.0 * sum(v in PASS_OK for v in vs) / len(vs), sum(v == "UNKNOWN" for v in vs)
    a_up_pass, a_up_unk = a_on_matched(unpin_verd)
    a_pn_pass, a_pn_unk = a_on_matched(pinned_verd)

    nums = {
        "matched_n": len(mset),
        "rho_unpinned_matched": round(rho_unp, 4), "p_unpinned_matched": round(p_unp, 3),
        "rho_pinned_matched":   round(rho_pin, 4), "p_pinned_matched":   round(p_pin, 3),
        "rho_pass_unknown_n8":  round(rho_pu, 4),  "p_pass_unknown_n8":  round(p_pu, 3),
        "A_matched_unpinned_pass": round(a_up_pass, 1), "A_matched_unpinned_unknown": a_up_unk,
        "A_matched_pinned_pass":   round(a_pn_pass, 1), "A_matched_pinned_unknown":   a_pn_unk,
    }
    series_dump = {"pinned": (pin_pas, pin_arc), "unpinned": (unp_pas, unp_arc),
                   "full_pass": fpas, "full_unknown": funk}
    return nums, series_dump


if __name__ == "__main__":
    nums, ser = compute()
    print(f"common-complete matched set: n={nums['matched_n']}\n")
    for label in ("pinned", "unpinned"):
        pas, arc = ser[label]
        print(f"=== {label.upper()} (matched n={nums['matched_n']}) ===")
        for i, c in enumerate(CONDS):
            print(f"  {c:>7}: pass%={pas[i]:5.1f}  recall={arc[i]:.4f}")
        rk = "rho_pinned_matched" if label == "pinned" else "rho_unpinned_matched"
        pk = "p_pinned_matched" if label == "pinned" else "p_unpinned_matched"
        print(f"  rho = {nums[rk]:+.4f}  (p={nums[pk]:.3f})\n")
    print("=== full-corpus (n=8), UNPINNED: pass% vs UNKNOWN% ===")
    for i, c in enumerate(CONDS):
        print(f"  {c:>7}: pass%={ser['full_pass'][i]:5.1f}  UNKNOWN%={ser['full_unknown'][i]:5.1f}")
    print(f"  rho(pass%, UNKNOWN%) = {nums['rho_pass_unknown_n8']:+.4f}  (p={nums['p_pass_unknown_n8']:.3f})\n")
    print(f"=== condition A on the SAME {nums['matched_n']} funcs ===")
    print(f"  UNPINNED: pass={nums['A_matched_unpinned_pass']}%  UNKNOWN={nums['A_matched_unpinned_unknown']}")
    print(f"  PINNED:   pass={nums['A_matched_pinned_pass']}%  UNKNOWN={nums['A_matched_pinned_unknown']}")
    json.dump(nums, open(EVAL / "pinned_rho_numbers.json", "w"), indent=1)
    print("\nwrote", EVAL / "pinned_rho_numbers.json")
