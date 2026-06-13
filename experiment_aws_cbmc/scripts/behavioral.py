import json, os, sys, difflib, re
sys.path.insert(0,"/root/experiment_aws_cbmc/scripts")
import run_mutation_oracle_cbmc as O
EVAL="/root/experiment_aws_cbmc/evaluation"; MUT="/root/experiment_aws_cbmc/mutants"
CONTRACT=re.compile(r'AWS_PRECONDITION|AWS_POSTCONDITION|AWS_FATAL_PRECONDITION|AWS_FATAL_POSTCONDITION|AWS_FATAL_ASSERT|AWS_ASSUME|AWS_ASSERT|__CPROVER|^\s*[+-]\s*assert\s*\(')
_oc={}
def ol(func):
    if func not in _oc:
        cfg=O.FUNC_CONFIGS.get(func,{}); idx=O.get_mutated_source_idx(func)
        _oc[func]=open(cfg['project_sources'][idx]).read().splitlines() if idx is not None else []
    return _oc[func]
_mc={}
def is_contract(func,m):
    k=(func,m)
    if k in _mc: return _mc[k]
    p=f"{MUT}/{func}/{m}.c"
    if not os.path.exists(p): _mc[k]=None; return None
    d=[l for l in difflib.unified_diff(ol(func),open(p).read().splitlines(),n=0,lineterm="") if l and l[0] in '+-' and not l.startswith(('+++','---'))]
    _mc[k]=bool(CONTRACT.search(" ".join(d))); return _mc[k]

canon=[(r['func'],r['mutant']) for r in json.load(open(f"{EVAL}/gt_fail_properties_canonical370.json"))['results'] if str(r.get('verdict','')).upper() in('FAIL','SAT')]
canon_beh=set(k for k in canon if not is_contract(*k))
print(f"DENOM: full=370 behavioral={len(canon_beh)} contract={370-len(canon_beh)}")
out={"denom_full":370,"denom_behavioral":len(canon_beh)}
CONDS=[("A_gptoss120b","A-gptoss"),("H_gptoss120b","H-gptoss"),("M_gptoss120b","M-gptoss"),
       ("Oracle_gptoss120b","Oracle"),("A_deepseekv4flash","A-DeepSeek"),("H_deepseekv4flash","H-DeepSeek"),
       ("G_deepseekv4flash","G-DeepSeek"),("A_claude","A-Claude"),("H_claude","H-Claude"),("M_claude","M-Claude")]
print(f"\n{'cond':<12}{'sil_full':>9}{'sil_beh':>8}{'SilGT_full':>11}{'SilGT_beh':>10}{'rev_full':>9}{'rev_beh':>8}")
for cond,label in CONDS:
    f=f"{EVAL}/mutation_oracle_cbmc_feedback_loop_{cond}.json"
    if not os.path.exists(f): continue
    res={(r['func'],r['mutant']):r for r in json.load(open(f))['results']}
    # restrict to canonical for comparability
    sil=[k for k in canon if k in res and res[k].get('silenced')]
    sil_b=[k for k in sil if k in canon_beh]
    rev=[k for k in res if str(res[k].get('gt','')).upper() in('SUCCESS','UNSAT') and str(res[k].get('llm','')).upper() in('FAIL','SAT')]
    rev_b=[k for k in rev if not is_contract(*k)]
    print(f"{label:<12}{len(sil):>9}{len(sil_b):>8}{100*len(sil)/370:>10.1f}%{100*len(sil_b)/len(canon_beh):>9.1f}%{len(rev):>9}{len(rev_b):>8}")
    out[cond]={"sil_full":len(sil),"sil_beh":len(sil_b),"rev_full":len(rev),"rev_beh":len(rev_b)}
json.dump(out, open(f"{EVAL}/behavioral_subset.json","w"), indent=1)
print("\nsaved behavioral_subset.json")
