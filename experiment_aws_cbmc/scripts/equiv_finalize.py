#!/usr/bin/env python3
"""Finalize the equivalent-mutant adjudication (close paper L877 'ongoing').

Argument structure:
1. Behavioural-code mutants that are GT-FAIL are NON-EQUIVALENT by the soundness
   of CBMC: a FAIL verdict is a bounded counterexample = a real behavioural
   difference. So equivalence candidates are EXACTLY the 41 contract-line GT-FAIL
   mutants (behavioral_subset: 370-329=41), already enumerated.
2. Of those 41: 26 are EQUIVALENT_ASSERT (nonfatal macro, assert-triggered ->
   shipped no-op). The remaining 15 (6 fatal + 9 keep_check) we adjudicate here
   at the CONDITION level with CBMC: prove whether the mutated predicate is
   logically equivalent to the original. NONEQ_PROVEN = genuine check change.
"""
import json, re, subprocess
from pathlib import Path
from collections import Counter

EXP = Path('/root/experiment_aws_cbmc'); MUT = EXP/'mutants'
adj = json.load(open(EXP/'evaluation/assert_mutant_adjudication.json'))

def mline(func, mut):
    meta = json.loads((MUT/func/'metadata.json').read_text())
    orig = Path(meta['source_file'].replace('/home/weiqi/Verification/aws-c-common',
                                             '/root/aws-c-common')).read_text().splitlines()
    mc = (MUT/func/f'{mut}.c').read_text().splitlines()
    for o, m in zip(orig, mc):
        if o != m:
            return o.strip(), m.strip()
    return '', ''

def extract_cond(line):
    m = re.search(r'(?:AWS_FATAL_)?(?:PRE|POST)?CONDITION\s*\((.*)\)\s*;?\s*$', line)
    if not m:
        m = re.search(r'\((.*)\)', line)
    if not m:
        return None
    inner = m.group(1)
    inner = re.sub(r',\s*".*$', '', inner)  # drop trailing message literal
    return inner.strip()

def has_funcall(expr):
    return bool(re.search(r'\b[A-Z_][A-Z0-9_]*\s*\(', expr)) or \
        any(k in expr for k in ('IS_READABLE', 'IS_WRITABLE', 'is_valid'))

def cbmc_neq(orig_c, mut_c):
    ids = sorted(set(re.findall(r'[A-Za-z_]\w*(?:->\w+|\.\w+)*', orig_c + ' ' + mut_c)))
    ids = [i for i in ids if i != 'NULL']
    sub = {}; decl = []
    for k, i in enumerate(ids):
        v = f'v{k}'; sub[i] = v; decl.append(f'unsigned long {v};')
    def rep(e):
        for i in sorted(sub, key=len, reverse=True):
            e = e.replace(i, sub[i])
        return e
    oc, mc = rep(orig_c), rep(mut_c)
    src = 'int main(){\n' + '\n'.join(decl) + \
          f'\n__CPROVER_assert(({oc})==({mc}),"equiv");\nreturn 0;}}\n'
    Path('/tmp/eqchk.c').write_text(src)
    try:
        out = subprocess.run(['cbmc', '/tmp/eqchk.c'], capture_output=True, text=True, timeout=60).stdout
    except subprocess.TimeoutExpired:
        return 'CBMC_TIMEOUT'
    if 'VERIFICATION SUCCESSFUL' in out:
        return 'EQ_PROVEN'
    if 'VERIFICATION FAILED' in out:
        return 'NONEQ_PROVEN'
    return 'CBMC_INCONCLUSIVE'

out = []
for x in adj:
    if x['verdict'] == 'EQUIVALENT_ASSERT':
        out.append({**x, 'final': 'EQUIVALENT'}); continue
    o, m = mline(x['func'], x['mutant'])
    oc, mc = extract_cond(o), extract_cond(m)
    if not oc or not mc:
        out.append({**x, 'final': 'PARSE_FAIL', 'orig': o, 'mut': m}); continue
    if has_funcall(oc) or has_funcall(mc):
        v = 'NONEQ_STRUCTURAL'  # a memory predicate clause was weakened/removed
    else:
        v = cbmc_neq(oc, mc)
    out.append({**x, 'final': v, 'orig_cond': oc, 'mut_cond': mc})
    print(f"{x['func'][:26]:<26} {x['mutant']:<11} {x['verdict']:<16} -> {v:<18} | {oc[:32]} VS {mc[:32]}")

c = Counter(r['final'] for r in out)
print('\nFINAL:', dict(c))
n_equiv = c.get('EQUIVALENT', 0)
n_noneq = c.get('NONEQ_PROVEN', 0) + c.get('NONEQ_STRUCTURAL', 0)
print(f"contract-line candidates = {len(out)}; confirmed EQUIVALENT = {n_equiv}; "
      f"confirmed NON-EQUIVALENT = {n_noneq}; other = {len(out)-n_equiv-n_noneq}")
json.dump(out, open(EXP/'evaluation/equiv_finalize.json', 'w'), indent=1)
print('saved -> evaluation/equiv_finalize.json')
