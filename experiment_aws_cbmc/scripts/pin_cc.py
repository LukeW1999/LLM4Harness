import json, glob
from scipy.stats import spearmanr
conds=["A","G","H","I","J","K","M","Oracle"]
# funcs with summary.json (decided) per condition
def decided(C):
    out=set()
    for s in glob.glob(f"results/feedback_loop_{C}_gptoss120b_pin/*/summary.json"):
        f=s.split("/")[-2]; out.add(f)
    return out
sets={C:decided(C) for C in conds}
common=set.intersection(*sets.values())
print("common-complete (completed) set size:", len(common), "(was 37 truncated)")
def pr(C):
    conv=tot=0
    for f in common:
        try:
            j=json.load(open(f"results/feedback_loop_{C}_gptoss120b_pin/{f}/summary.json")); tot+=1
            v=j.get("converged"); its=j.get("iterations",[])
            if v is True or (its and str(its[-1].get("verify","")).upper() in ("SUCCESS","UNSAT")): conv+=1
        except: pass
    return 100*conv/tot if tot else 0
def rc(C):
    e=json.load(open(f"evaluation/cross_verify_results_cond{C}_pin.json"))
    rs=[x["harness_recall"] for x in e if x["func"] in common and x.get("gt_harness_count",0)>0 and x.get("harness_recall") is not None]
    return 100*sum(rs)/len(rs) if rs else 0
P=[pr(C) for C in conds]; R=[rc(C) for C in conds]
for C,p,r in zip(conds,P,R): print(f"{C:<8} pass {p:5.1f}  recall {r:5.1f}")
rho,pv=spearmanr(P,R)
print(f"\ncommon-complete pinned rho(pass,recall) = {rho:.3f} (p={pv:.3f}), n_conds=8, n_funcs={len(common)}")
ordp=sorted(conds,key=lambda c:-P[conds.index(c)]); ordr=sorted(conds,key=lambda c:-R[conds.index(c)])
print("highest-pass 2:",ordp[:2],"recall ranks:",[ordr.index(c)+1 for c in ordp[:2]])
