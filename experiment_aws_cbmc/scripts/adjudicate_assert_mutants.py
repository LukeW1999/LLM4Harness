import json, re, subprocess, sys
from pathlib import Path
EXP=Path('/root/experiment_aws_cbmc'); MUT=EXP/'mutants'; SRC=Path('/root/aws-c-common')
sys.path.insert(0,str(EXP/'scripts'))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
from run_mutation_oracle_cbmc import get_mutated_source_idx, get_final_harness

canon=json.load(open(EXP/'evaluation/mutation_oracle_cbmc_feedback_loop_A_gptoss120b.json'))['results']
gtfail=[(r['func'],r['mutant']) for r in canon if r['gt']=='FAIL']
NONFATAL=re.compile(r'AWS_PRECONDITION|AWS_POSTCONDITION(?!.*FATAL)')
FATAL=re.compile(r'AWS_FATAL_PRECONDITION|AWS_FATAL_POSTCONDITION|AWS_FATAL_ASSERT')
ANY=re.compile(r'AWS_PRECONDITION|AWS_POSTCONDITION|AWS_FATAL_PRECONDITION|AWS_FATAL_POSTCONDITION|AWS_ASSERT|AWS_FATAL_ASSERT')

def mline(func,mut):
    meta=json.loads((MUT/func/'metadata.json').read_text())
    orig=Path(meta['source_file'].replace('/home/weiqi/Verification/aws-c-common','/root/aws-c-common')).read_text().splitlines()
    mc=(MUT/func/f'{mut}.c').read_text().splitlines()
    for i,(o,m) in enumerate(zip(orig,mc)):
        if o!=m: return i+1,o.strip(),m.strip()
    return None,'',''

def cex_property(func,mut,cond='feedback_loop_A_gptoss120b'):
    # use GT harness to see which property the mutation makes fail (the GT-FAIL trigger)
    idx=get_mutated_source_idx(func); cfg=FUNC_CONFIGS[func]
    ps=[str(p) for p in cfg.get('proof_sources',[])]
    proj=[str(p) for p in cfg.get('project_sources',[])]; proj[idx]=str(MUT/func/f'{mut}.c')
    gt=SRC/f'verification/cbmc/proofs/{func}/{func}_harness.c'
    rm=[]
    for fn in cfg.get('remove_function_body',[]): rm+=['--remove-function-body',fn]
    cmd=['cbmc']+list(COMMON_FLAGS)+list(cfg.get('defines',[]))+list(cfg.get('unwind',[]))+list(cfg.get('unwindset',[]) or [])+rm+['--trace','--json-ui','--function',f'{func}_harness']+ps+[str(gt)]+proj
    try: out=subprocess.run(cmd,capture_output=True,text=True,timeout=120).stdout
    except subprocess.TimeoutExpired: return 'TIMEOUT'
    try: j=json.loads(out)
    except: return 'PARSE_FAIL'
    props=[]
    for it in j:
        if isinstance(it,dict) and it.get('result'):
            for r in it['result']:
                if r.get('status')=='FAILURE': props.append(r.get('property',''))
    return ';'.join(props) if props else 'NO_FAILURE'

results=[]
for func,mut in gtfail:
    ln,o,m=mline(func,mut)
    if not ANY.search(m): continue
    kind='fatal' if FATAL.search(m) else ('nonfatal' if NONFATAL.search(m) else 'other')
    prop=cex_property(func,mut)
    # verdict: GT-FAIL triggered by a precondition/assertion property on the mutated assert => equivalent (in-source assertion only)
    is_assert_prop = bool(re.search(r'precondition|postcondition|assertion', prop)) and 'assert_bytes_match' not in prop
    verdict = 'EQUIVALENT_ASSERT' if (kind=='nonfatal' and is_assert_prop) else               ('FATAL_REVIEW' if kind=='fatal' else 'KEEP_CHECK')
    results.append({'func':func,'mutant':mut,'line':ln,'kind':kind,'mutated':m[:70],'gt_fail_property':prop[:80],'verdict':verdict})
    print(f"{func[:24]:<24} {mut:<11} {kind:<8} {verdict:<17} {prop[:45]}")

from collections import Counter
print(); print('VERDICTS:',dict(Counter(r['verdict'] for r in results)),' total',len(results))
json.dump(results,open(EXP/'evaluation/assert_mutant_adjudication.json','w'),indent=1)
print('saved -> evaluation/assert_mutant_adjudication.json')
