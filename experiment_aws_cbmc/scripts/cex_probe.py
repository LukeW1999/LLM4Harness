import json, subprocess, sys
from pathlib import Path
EXP=Path('/root/experiment_aws_cbmc'); SRC=Path('/root/aws-c-common'); PROOF=SRC/'verification/cbmc'
sys.path.insert(0, str(EXP/'scripts'))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
from run_mutation_oracle_cbmc import get_mutated_source_idx, get_final_harness

func, mut, cond = sys.argv[1], sys.argv[2], sys.argv[3]   # e.g. aws_string_eq_c_str mutant_0011 feedback_loop_A_claude
idx = get_mutated_source_idx(func)
cfg = FUNC_CONFIGS[func]
ps = [str(p) for p in cfg.get('proof_sources',[])]
proj = [str(p) for p in cfg.get('project_sources',[])]
proj[idx] = str(EXP/'mutants'/func/f'{mut}.c')
llm = get_final_harness(EXP/'results'/cond/func)
rm=[]
for fn in cfg.get('remove_function_body',[]): rm += ['--remove-function-body', fn]
cmd=(['cbmc']+list(COMMON_FLAGS)+list(cfg.get('defines',[]))+list(cfg.get('unwind',[]))+
     list(cfg.get('unwindset',[]) or [])+rm+['--trace','--json-ui',
     '--function',f'{func}_harness']+ps+[str(llm)]+proj)
out=subprocess.run(cmd,capture_output=True,text=True,timeout=120).stdout
try: j=json.loads(out)
except: print('JSON parse failed'); print(out[:500]); sys.exit(1)
for item in j:
    if isinstance(item,dict) and item.get('result'):
        for r in item['result']:
            if r.get('status')=='FAILURE':
                print('FAILED PROPERTY:', r.get('property'))
                print('  description:', r.get('description'))
                # last few trace steps with assignments
                assigns=[s for s in r.get('trace',[]) if s.get('stepType')=='assignment' and s.get('sourceLocation',{}).get('file','').endswith(('string.c','assert.h'))]
                print('  failing source line:', r['trace'][-1].get('sourceLocation',{}) if r.get('trace') else '?')
