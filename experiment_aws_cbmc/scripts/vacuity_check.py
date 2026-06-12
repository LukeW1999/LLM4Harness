#!/usr/bin/env python3
"""
vacuity_check.py - Detect vacuous SUCCESS in LLM-generated harnesses.

For each function where the final harness reached SUCCESS, insert
__CPROVER_assert(false) immediately before the first function call.
If CBMC returns UNSAT, all assume clauses are mutually contradictory
(no path reaches the call) -> harness is vacuously true.

Usage:
    python3 scripts/vacuity_check.py --condition K_gptoss120b
    python3 scripts/vacuity_check.py --condition M_gptoss120b --save-json
    python3 scripts/vacuity_check.py --all-new-conditions --save-json
"""

import re
import sys
import json
import tempfile
import subprocess
import argparse
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS, SRCDIR, PROOFDIR

RESULTS_DIR = SCRIPT_DIR.parent / 'results'
EVAL_DIR = SCRIPT_DIR.parent / 'evaluation'

def inject_vacuity_assert(harness_text: str, func_name: str) -> str:
    """
    Insert __CPROVER_assert(false, "vacuity") immediately before
    the first call to func_name in the harness.
    Returns modified harness text, or None if call not found.
    """
    # Find the function call line
    pattern = rf'([ \t]*(?:[\w\* ]+\s+)?\w+\s*=\s*)?{re.escape(func_name)}\s*\('
    m = re.search(pattern, harness_text)
    if not m:
        # Try without return value capture
        pattern2 = rf'([ \t]*){re.escape(func_name)}\s*\('
        m = re.search(pattern2, harness_text)
    if not m:
        return None
    
    insert_pos = m.start()
    indent = re.match(r'[ \t]*', harness_text[insert_pos:]).group()
    inject = f'{indent}__CPROVER_assert(false, "vacuity_probe");\n'
    return harness_text[:insert_pos] + inject + harness_text[insert_pos:]


def check_vacuity(func_name: str, harness_path: Path) -> dict:
    """
    Returns dict with keys: func, harness, vacuous (bool), error (str or None)
    """
    cfg = FUNC_CONFIGS.get(func_name)
    if cfg is None:
        return {'func': func_name, 'vacuous': None, 'error': 'no config'}

    harness_text = harness_path.read_text()
    modified = inject_vacuity_assert(harness_text, func_name)
    if modified is None:
        return {'func': func_name, 'vacuous': None, 'error': f'call to {func_name} not found'}

    with tempfile.NamedTemporaryFile(suffix='.c', mode='w', delete=False) as f:
        f.write(modified)
        tmp_path = Path(f.name)

    try:
        src_files = [str(PROOFDIR / 'sources' / 'make_common_data_structures.c')]
        for stub in cfg.get('stubs', []):
            src_files.append(str(PROOFDIR / 'stubs' / stub))
        for util in cfg.get('utils', []):
            src_files.append(str(PROOFDIR / 'sources' / util))
        for src in cfg.get('sources', []):
            src_files.append(str(SRCDIR / src))
        src_files.append(str(tmp_path))

        cbmc_args = ['cbmc'] + COMMON_FLAGS
        for flag in cfg.get('extra_flags', []):
            cbmc_args.append(flag)
        cbmc_args += ['--function', f'{func_name}_harness'] + src_files

        result = subprocess.run(
            cbmc_args, capture_output=True, text=True, timeout=120
        )
        combined = result.stdout + result.stderr
        
        # If vacuity probe is UNSAT -> all paths pruned -> vacuous
        if 'VERIFICATION SUCCESSFUL' in combined:
            # vacuity_probe assert passed (UNSAT means no path reaches it)
            vacuous = True
        elif 'VERIFICATION FAILED' in combined:
            # vacuity_probe assert fired -> path IS reachable -> not vacuous
            vacuous = False
        else:
            vacuous = None
            
        return {'func': func_name, 'vacuous': vacuous, 'error': None,
                'cbmc_rc': result.returncode}
    except subprocess.TimeoutExpired:
        return {'func': func_name, 'vacuous': None, 'error': 'timeout'}
    except Exception as e:
        return {'func': func_name, 'vacuous': None, 'error': str(e)}
    finally:
        tmp_path.unlink(missing_ok=True)


def run_condition(condition: str, workers: int = 8) -> list:
    import concurrent.futures

    cond_dir = RESULTS_DIR / f'feedback_loop_{condition}'
    if not cond_dir.exists():
        print(f'ERROR: {cond_dir} not found')
        return []

    # Only check functions that reached final SUCCESS
    tasks = []
    for func in sorted(cond_dir.iterdir()):
        summary_p = func / 'summary.json'
        if not summary_p.exists(): continue
        d = json.loads(summary_p.read_text())
        iters = d.get('iterations', [])
        if not iters: continue
        last_verify = iters[-1].get('verify', '')
        if last_verify != 'SUCCESS': continue
        
        # Get last harness file
        last_iter = iters[-1]['iter']
        harness_p = func / f'iter_{last_iter}_harness.c'
        if not harness_p.exists(): continue
        tasks.append((func.name, harness_p))

    print(f'Condition {condition}: checking {len(tasks)} SUCCESS harnesses for vacuity...')

    results = []
    with concurrent.futures.ProcessPoolExecutor(max_workers=workers) as ex:
        futures = {ex.submit(check_vacuity, fn, hp): fn for fn, hp in tasks}
        for fut in concurrent.futures.as_completed(futures):
            r = fut.result()
            results.append(r)
            status = 'VACUOUS' if r['vacuous'] else ('reachable' if r['vacuous'] is False else f'unknown({r["error"]})')
            print(f'  {r["func"]}: {status}')

    vacuous_count = sum(1 for r in results if r['vacuous'] is True)
    reachable_count = sum(1 for r in results if r['vacuous'] is False)
    unknown_count = sum(1 for r in results if r['vacuous'] is None)
    total = len(results)
    print(f'\nSummary: {total} checked | VACUOUS={vacuous_count} ({vacuous_count/total*100:.1f}%) | reachable={reachable_count} | unknown={unknown_count}')
    return results


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--condition', help='e.g. K_gptoss120b')
    parser.add_argument('--all-new-conditions', action='store_true',
                        help='Run K, Oracle, M conditions')
    parser.add_argument('--save-json', action='store_true')
    parser.add_argument('--workers', type=int, default=8)
    args = parser.parse_args()

    conditions = []
    if args.all_new_conditions:
        conditions = ['K_gptoss120b', 'Oracle_gptoss120b', 'M_gptoss120b']
    elif args.condition:
        conditions = [args.condition]
    else:
        parser.print_help(); return

    all_results = {}
    for cond in conditions:
        results = run_condition(cond, args.workers)
        all_results[cond] = results
        if args.save_json and results:
            out = EVAL_DIR / f'vacuity_check_{cond}.json'
            out.write_text(json.dumps(results, indent=2))
            print(f'Saved: {out}')

if __name__ == '__main__':
    main()
