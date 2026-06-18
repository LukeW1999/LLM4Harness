#!/usr/bin/env python3
import json, subprocess, hashlib
from pathlib import Path

SRC = Path('/root/aws-c-common')
DT  = Path('/root/experiment_aws_cbmc/difftest')
MUT = Path('/root/experiment_aws_cbmc/mutants')
EVAL= Path('/root/experiment_aws_cbmc/evaluation')

LIB = [SRC/'source/byte_buf.c', SRC/'source/allocator.c', SRC/'source/error.c',
       SRC/'source/assert.c', SRC/'source/array_list.c']
VARIANT = {'aws_string_eq_c_str': 1, 'aws_string_eq_byte_cursor': 2, 'aws_string_eq_byte_buf': 3}

def build(variant, string_c, out):
    cmd = ['gcc', '-O1', '-I', str(DT/'shim'), '-I', str(SRC/'include'),
           f'-DFUNC_VARIANT={variant}', str(DT/'driver.c'), str(DT/'stubs.c'),
           str(string_c)] + [str(x) for x in LIB] + ['-o', str(out)]
    p = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
    return p.returncode == 0, p.stderr[:300]

def run(binary):
    p = subprocess.run([str(binary)], capture_output=True, text=True, timeout=30)
    return p.stdout

oracle = json.load(open(EVAL/'mutation_oracle_cbmc_feedback_loop_A_claude.json'))['results']
results = []
for fn, var in VARIANT.items():
    rev = sorted(r['mutant'] for r in oracle if r['func']==fn and r['gt']=='SUCCESS' and r['llm']=='FAIL')
    ok, err = build(var, SRC/'source/string.c', f'/tmp/orig_{var}')
    assert ok, err
    orig_out = run(f'/tmp/orig_{var}').splitlines()
    print(f'=== {fn}: {len(rev)} reverse-cell mutants ===')
    for m in rev:
        mc = MUT/fn/f'{m}.c'
        ok, err = build(var, mc, f'/tmp/mut_{var}')
        if not ok:
            results.append({'func': fn, 'mutant': m, 'verdict': 'NATIVE_CE'}); print(f'  {m}: NATIVE_CE'); continue
        try:
            mut_out = run(f'/tmp/mut_{var}').splitlines()
        except subprocess.TimeoutExpired:
            results.append({'func': fn, 'mutant': m, 'verdict': 'TIMEOUT/HANG'}); print(f'  {m}: TIMEOUT'); continue
        diff_at = next((i for i, (a, b) in enumerate(zip(orig_out, mut_out)) if a != b), None)
        if diff_at is None and len(orig_out) != len(mut_out):
            diff_at = min(len(orig_out), len(mut_out))
        if diff_at is not None:
            results.append({'func': fn, 'mutant': m, 'verdict': 'DISTINGUISHED',
                            'first_diff_trial': diff_at,
                            'orig': orig_out[diff_at] if diff_at < len(orig_out) else 'EOF',
                            'mut': mut_out[diff_at] if diff_at < len(mut_out) else 'EOF/CRASH'})
            print(f'  {m}: DISTINGUISHED at trial {diff_at} (orig={orig_out[diff_at] if diff_at < len(orig_out) else "EOF"} mut={mut_out[diff_at] if diff_at < len(mut_out) else "EOF"})')
        else:
            results.append({'func': fn, 'mutant': m, 'verdict': 'NO_DIFF_20K'})
            print(f'  {m}: NO_DIFF after 20000 trials')

from collections import Counter
c = Counter(r['verdict'] for r in results)
print()
print('SUMMARY:', dict(c), f'  total={len(results)}')
json.dump(results, open(EVAL/'difftest_reverse_cell_string_eq.json', 'w'), indent=1)
print('saved -> evaluation/difftest_reverse_cell_string_eq.json')
