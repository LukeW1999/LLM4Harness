#!/usr/bin/env python3
"""Re-run GT harness on the canonical 370 GT-FAIL mutants, capture CBMC stdout,
and record WHICH properties (assertions) failed for each mutant."""
import json, re, subprocess, sys
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

EXPERIMENT_DIR = Path('/root/experiment_aws_cbmc')
MUTANTS_DIR    = EXPERIMENT_DIR / 'mutants'
EVAL_DIR       = EXPERIMENT_DIR / 'evaluation'
GT_PROOFS_DIR  = Path('/root/aws-c-common/verification/cbmc/proofs')
SRCDIR         = Path('/root/aws-c-common')
PROOFDIR       = SRCDIR / 'verification/cbmc'
TIMEOUT = 120

sys.path.insert(0, str(EXPERIMENT_DIR / 'scripts'))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
from run_mutation_oracle_cbmc import get_mutated_source_idx

PROP_RE = re.compile(r'\[([^\]]+)\]\s+line\s+(\d+)\s+(.*?):\s+(SUCCESS|FAILURE)\s*$')

def run_and_parse(task):
    func, mutant_name, mutated_src_idx = task
    mutant_c = MUTANTS_DIR / func / f'{mutant_name}.c'
    harness_c = GT_PROOFS_DIR / func / f'{func}_harness.c'
    cfg = FUNC_CONFIGS.get(func)
    if cfg is None:
        return {'func': func, 'mutant': mutant_name, 'verdict': 'NO_CONFIG', 'failed_properties': []}
    proof_sources   = [str(p) for p in cfg.get('proof_sources', [])]
    project_sources = [str(p) for p in cfg.get('project_sources', [])]
    project_sources[mutated_src_idx] = str(mutant_c)
    obj_bits = cfg.get('object_bits', 8)
    if obj_bits != 8:
        base_flags = [f'-I{SRCDIR}/include', f'-I{PROOFDIR}/include', '-DCBMC',
                      f'-DCBMC_OBJECT_BITS={obj_bits}',
                      '-DCBMC_MAX_OBJECT_SIZE=(SIZE_MAX>>(CBMC_OBJECT_BITS+1))',
                      '--object-bits', str(obj_bits)]
        defines = [d for d in cfg.get('defines', []) if 'CBMC_OBJECT_BITS' not in d]
    else:
        base_flags = list(COMMON_FLAGS)
        defines = list(cfg.get('defines', []))
    remove_flags = []
    for fn in cfg.get('remove_function_body', []):
        remove_flags += ['--remove-function-body', fn]
    cmd = (['cbmc'] + base_flags + defines + list(cfg.get('unwind', [])) +
           list(cfg.get('unwindset', []) or []) + remove_flags +
           ['--function', f'{func}_harness'] + proof_sources + [str(harness_c)] + project_sources)
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=TIMEOUT)
        stdout = proc.stdout
    except subprocess.TimeoutExpired:
        return {'func': func, 'mutant': mutant_name, 'verdict': 'TIMEOUT', 'failed_properties': []}
    verdict = ('SUCCESS' if 'VERIFICATION SUCCESSFUL' in stdout
               else 'FAIL' if 'VERIFICATION FAILED' in stdout else 'UNKNOWN')
    failed = []
    for line in stdout.splitlines():
        m = PROP_RE.search(line.strip())
        if m and m.group(4) == 'FAILURE':
            pid, lineno, desc = m.group(1), int(m.group(2)), m.group(3)
            src = 'harness' if 'harness' in pid else 'library'
            failed.append({'property': pid, 'line': lineno, 'desc': desc, 'source': src})
    return {'func': func, 'mutant': mutant_name, 'verdict': verdict, 'failed_properties': failed}

def main():
    canonical = json.loads((EVAL_DIR / 'mutation_oracle_cbmc_feedback_loop_A_gptoss120b.json').read_text())
    gtfail = [(r['func'], r['mutant']) for r in canonical['results'] if r['gt'] == 'FAIL']
    print(f'Canonical GT-FAIL mutants: {len(gtfail)}')
    idx_cache = {}
    tasks = []
    for func, mutant in gtfail:
        if func not in idx_cache:
            idx_cache[func] = get_mutated_source_idx(func)
        if idx_cache[func] is None:
            continue
        tasks.append((func, mutant, idx_cache[func]))
    print(f'Tasks: {len(tasks)}')
    results, done = [], 0
    with ProcessPoolExecutor(max_workers=8) as ex:
        futs = {ex.submit(run_and_parse, t): t for t in tasks}
        for fut in as_completed(futs):
            results.append(fut.result()); done += 1
            if done % 50 == 0:
                print(f'  [{done}/{len(tasks)}]', flush=True)
    out = EVAL_DIR / 'gt_fail_properties_canonical370.json'
    out.write_text(json.dumps({'n': len(results), 'results': results}, indent=1))
    nfail = sum(1 for r in results if r['verdict'] == 'FAIL')
    print(f'Done. verdict=FAIL reproduced: {nfail}/{len(results)}. Saved -> {out}')

if __name__ == '__main__':
    main()
