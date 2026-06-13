import sys, json
from pathlib import Path
sys.path.insert(0,"/root/experiment_aws_cbmc/scripts")
import importlib.util as u
spec=u.spec_from_file_location("av2","/root/experiment_aws_cbmc/scripts/attribution_v2.py")
av2=u.module_from_spec(spec); spec.loader.exec_module(av2)
A=av2.A
# point GT extraction at s2n proofs
A.GT_PROOFS_DIR=Path("/root/s2n-tls/tests/cbmc/proofs")
EVAL=Path("/root/experiment_aws_cbmc/evaluation")
from collections import Counter, defaultdict
for cond in ["A_claude","A_gptoss120b"]:
    d=json.load(open(EVAL/f"mutation_oracle_s2n_{cond}.json"))
    sil=[r for r in d["results"] if r.get("silenced")]
    byf=defaultdict(list)
    for r in sil: byf[r["func"]].append(r["mutant"])
    summ=Counter()
    perf={}
    for f,muts in byf.items():
        lab,_=av2.classify_func(f"feedback_loop_{cond}", f)
        summ[lab]+=len(muts); perf[f]=(lab,len(muts))
    tot=len(sil)
    print(f"\n=== s2n {cond}: {tot} silenced ===")
    for lab in ["KNOWLEDGE-GAP","SACRIFICE","AOC","UNRESOLVED"]:
        n=summ.get(lab,0); print(f"  {lab:<16} {n}  ({100*n/tot:.1f}%)")
    for f,(lab,n) in sorted(perf.items()): print(f"     {f:<34} {lab} x{n}")
