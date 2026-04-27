#!/usr/bin/env python3
"""
run_esbmc_gt_batch.py - Run all GT harnesses through ESBMC and report results.
"""
import sys, json
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor, as_completed

sys.path.insert(0, str(Path(__file__).parent))
from esbmc_runner import run_gt, FUNC_CONFIGS

RESULTS_DIR = Path(__file__).parent.parent / "results"
RESULTS_DIR.mkdir(exist_ok=True)
OUT_JSON = RESULTS_DIR / "esbmc_gt_results.json"

TIMEOUT = 300   # seconds per function

def _run_one(fn):
    try:
        r = run_gt(fn)
        return fn, r.verification_result, r.compilation_ok, r.num_checks, r.num_failed, r.error_summary[:200]
    except Exception as e:
        return fn, "ERROR", False, 0, 0, str(e)[:200]

funcs = sorted(FUNC_CONFIGS.keys())
print(f"Running {len(funcs)} functions (timeout={TIMEOUT}s each) ...")

results = {}
counts = {"SUCCESS": 0, "FAIL": 0, "TIMEOUT": 0, "COMPILE_ERROR": 0, "UNKNOWN": 0, "ERROR": 0}

with ThreadPoolExecutor(max_workers=6) as pool:
    futures = {pool.submit(_run_one, fn): fn for fn in funcs}
    done = 0
    for fut in as_completed(futures):
        fn, verdict, comp_ok, n_checks, n_failed, err = fut.result()
        results[fn] = {
            "verdict": verdict,
            "compilation_ok": comp_ok,
            "num_checks": n_checks,
            "num_failed": n_failed,
            "error": err,
        }
        counts[verdict] = counts.get(verdict, 0) + 1
        done += 1
        status = "OK" if verdict == "SUCCESS" else verdict
        print(f"  [{done:3d}/{len(funcs)}] {fn:<50s} {status}")
        sys.stdout.flush()

print("\n── Summary ──────────────────────────────────────────")
for k, v in sorted(counts.items()):
    if v:
        print(f"  {k:<15s}: {v}")
print(f"  TOTAL         : {len(funcs)}")

with open(OUT_JSON, "w") as f:
    json.dump(results, f, indent=2)
print(f"\nSaved to {OUT_JSON}")
