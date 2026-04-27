#!/usr/bin/env python3
"""
run_gt_baseline.py
==================
Run all s2n_stuffer GT harnesses and record pass/fail.
Produces a JSON summary at ../results/gt_baseline.json.
"""

import json
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from cbmc_runner_s2n import run_gt, PROOFS_DIR

RESULTS_DIR = Path(__file__).parent.parent / "results"
RESULTS_DIR.mkdir(parents=True, exist_ok=True)

STUFFER_PROOFS = sorted(
    d.name for d in PROOFS_DIR.iterdir()
    if d.is_dir() and d.name.startswith("s2n_stuffer")
)


def main():
    results = {}
    total = len(STUFFER_PROOFS)
    passed = failed = compile_err = timeout = unknown = 0

    print(f"Running {total} GT harnesses...\n")
    for i, func in enumerate(STUFFER_PROOFS, 1):
        t0 = time.time()
        r = run_gt(func)
        elapsed = time.time() - t0
        status = r.verification_result

        sym = {"SUCCESS": "✓", "FAIL": "✗", "COMPILE_ERROR": "C", "TIMEOUT": "T"}.get(status, "?")
        print(f"[{i:2}/{total}] {sym} {func:45s}  {status:13s}  {r.num_failed}/{r.num_checks} failed  {elapsed:.0f}s")

        if status == "SUCCESS":   passed += 1
        elif status == "FAIL":    failed += 1; print(f"        {r.error_summary[:120]}")
        elif status == "COMPILE_ERROR": compile_err += 1; print(f"        {r.error_summary[:120]}")
        elif status == "TIMEOUT": timeout += 1
        else:                     unknown += 1

        results[func] = {
            "verification_result": status,
            "compilation_ok": r.compilation_ok,
            "num_checks": r.num_checks,
            "num_failed": r.num_failed,
            "elapsed_s": round(elapsed, 1),
            "error_summary": r.error_summary,
        }

    print(f"\n{'='*60}")
    print(f"Total:         {total}")
    print(f"  PASS:        {passed}")
    print(f"  FAIL:        {failed}")
    print(f"  COMPILE ERR: {compile_err}")
    print(f"  TIMEOUT:     {timeout}")
    print(f"  UNKNOWN:     {unknown}")

    out = RESULTS_DIR / "gt_baseline.json"
    out.write_text(json.dumps(results, indent=2))
    print(f"\nResults saved to {out}")


if __name__ == "__main__":
    main()
