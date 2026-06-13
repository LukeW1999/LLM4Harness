import json, os
EVAL="/root/experiment_aws_cbmc/evaluation"
def sil(c):
    f=f"{EVAL}/mutation_oracle_cbmc_feedback_loop_{c}.json"
    if not os.path.exists(f): return None
    d=json.load(open(f))['results']; g=[r for r in d if str(r.get('gt','')).upper() in('FAIL','SAT')]
    return sum(1 for r in g if r.get('silenced'))
import statistics as st
rows={}
for base,tags in [("A_gptoss120b",["","_r2","_r3"]),("M_gptoss120b",["","_r2","_r3"]),
                  ("G_gptoss120b",["","_r2","_r3"]),("Oracle_gptoss120b",["","_r2"]),
                  ("H_gptoss120b",["","_r2","_r3"])]:
    vals=[sil(base+t) for t in tags]; vals=[v for v in vals if v is not None]
    rows[base]=vals
    if vals:
        pct=[100*v/370 for v in vals]
        print(f"{base:<18} runs={vals}  Sil/GT={'/'.join(f'{p:.1f}' for p in pct)}%  mean={st.mean(pct):.1f}% range=[{min(pct):.1f},{max(pct):.1f}]")
json.dump(rows, open(f"{EVAL}/multirun_silenced.json","w"))
