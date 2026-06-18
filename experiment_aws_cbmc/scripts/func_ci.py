import sys, json, io, contextlib, collections
sys.path.insert(0,"scripts"); sys.argv=["x"]
from scipy.stats import beta
def cp(k,n,a=0.05):
    lo=0.0 if k==0 else beta.ppf(a/2,k,n-k+1)
    hi=1.0 if k==n else beta.ppf(1-a/2,k+1,n-k)
    return 100*lo,100*hi
buf=io.StringIO()
with contextlib.redirect_stdout(buf):
    import attribution_v2 as av
print("=== function-unit KG proportion (function = unit of analysis) ===")
for cond in ["A_claude","Oracle_gptoss120b","A_gptoss120b","M_gptoss120b"]:
    r=av.run(cond)
    if r is None: print(f"{cond}: no data"); continue
    s,fd=r
    nf=len(fd); kg=sum(1 for f,(lab,n) in fd.items() if lab=="KNOWLEDGE-GAP")
    lo,hi=cp(kg,nf); print(f"{cond:<20} KG-functions {kg}/{nf} = {100*kg/nf:.0f}%  95%CP[{lo:.0f},{hi:.0f}]")
print("--- s2n (b1_relax per-func, KG = majority KG_confirmed/KG_no_bounds) ---")
for cond in ["A_claude","A_gptoss120b"]:
    d=json.load(open(f"evaluation/b1_relax_s2n_{cond}.json"))
    byf=collections.defaultdict(list)
    for r in d: byf[r["func"]].append(r["label"])
    nf=len(byf); kg=sum(1 for f,labs in byf.items() if sum(l.startswith("KG") for l in labs)>len(labs)/2)
    lo,hi=cp(kg,nf); print(f"s2n_{cond:<14} KG-functions {kg}/{nf} = {100*kg/nf:.0f}%  95%CP[{lo:.0f},{hi:.0f}]")
