import json, glob
from scipy.stats import spearmanr
def passrate(C):
    conv=tot=0
    for s in glob.glob(f"results/feedback_loop_{C}_gptoss120b_pin/*/summary.json"):
        try:
            j=json.load(open(s)); tot+=1
            v=j.get("converged")
            if v is True: conv+=1
            elif v is None:
                its=j.get("iterations",[])
                if its and str(its[-1].get("verify","")).upper() in ("SUCCESS","UNSAT"): conv+=1
        except: pass
    return 100*conv/tot if tot else 0, tot
def recall(C):
    f=f"evaluation/cross_verify_results_cond{C}_pin.json"
    e=json.load(open(f))
    rs=[x["harness_recall"] for x in e if x.get("gt_harness_count",0)>0 and x.get("harness_recall") is not None]
    return 100*sum(rs)/len(rs) if rs else 0, len(rs)
conds=["A","G","H","I","J","K","M","Oracle"]
print(f"{'cond':<8}{'pass%':>7}{'recall%':>9}{'n_pass':>7}{'n_rec':>6}")
P=[];R=[]
for C in conds:
    p,np_=passrate(C); r,nr=recall(C)
    P.append(p); R.append(r)
    print(f"{C:<8}{p:>6.1f}{r:>9.1f}{np_:>7}{nr:>6}")
rho,pval=spearmanr(P,R)
print(f"\nPINNED full-corpus Spearman rho(pass, recall) over 8 conds = {rho:.3f} (p={pval:.3f})")
# ordering check
ordp=sorted(conds, key=lambda c: -P[conds.index(c)])
ordr=sorted(conds, key=lambda c: -R[conds.index(c)])
print("pass order (hi->lo):", ordp)
print("recall order(hi->lo):", ordr)
print("highest-pass 2:", ordp[:2], "| their recall rank:", [ordr.index(c)+1 for c in ordp[:2]])
