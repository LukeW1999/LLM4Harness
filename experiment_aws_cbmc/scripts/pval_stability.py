import json
from collections import defaultdict
from scipy import stats

def load(p):
    raw = json.load(open(p))
    return raw.get("results", raw) if isinstance(raw, dict) else raw

def per_func(data):
    gt = defaultdict(int); sil = defaultdict(int)
    for it in data:
        f = it.get("func", "")
        g = it.get("gt") or it.get("gt_result", "")
        l = it.get("llm") or it.get("llm_result", "")
        if g == "FAIL":
            gt[f] += 1
            if it.get("silenced") or l == "SUCCESS":
                sil[f] += 1
    return {f: (sil[f], gt[f], sil[f]/gt[f] if gt[f] else 0.0) for f in set(gt) | set(sil)}

base = "evaluation/mutation_oracle_cbmc_feedback_loop_{}_gptoss120b{}.json"
runs = [("run1(headline)", ""), ("run2", "_r2"), ("run3", "_r3")]
hdr = ("run", "n", "meanA%", "meanH%", "stat", "p_2sided", "p_1sided_A>H")
print("%-15s %3s %7s %7s %7s %9s %13s" % hdr)
for name, suf in runs:
    A = per_func(load(base.format("A", suf)))
    H = per_func(load(base.format("H", suf)))
    common = sorted(set(A) & set(H))
    av = [A[f][2] for f in common]; hv = [H[f][2] for f in common]
    nz = sum(1 for a, b in zip(av, hv) if a != b)
    s2, p2 = stats.wilcoxon(av, hv, zero_method="wilcox")
    try:
        s1, p1 = stats.wilcoxon(av, hv, zero_method="wilcox", alternative="greater")
    except Exception:
        p1 = float("nan")
    ma = 100*sum(av)/len(av); mh = 100*sum(hv)/len(hv)
    verdict = "n.s." if p2 >= 0.05 else ("*" if p2 >= 0.01 else "**")
    print("%-15s %3d %7.2f %7.2f %7.1f %9.4f %13.4f   nz=%d  %s" % (name, len(common), ma, mh, s2, p2, p1, nz, verdict))
