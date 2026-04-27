#!/usr/bin/env python3
"""
precond_minimize.py
===================
Precondition minimization for LLM-generated CBMC harnesses.

For each __CPROVER_assume(...) in the harness, remove it and run CBMC.
  - If CBMC still passes  → the assume is UNNECESSARY (over-constraining)
  - If CBMC now fails     → the assume is NECESSARY (legitimate precondition)

Requires no ground truth.  Only needs: harness + CBMC.

Usage:
    python precond_minimize.py --func aws_byte_buf_init
    python precond_minimize.py --all
    python precond_minimize.py --func aws_byte_buf_init --variant feedback_loop_B_claude --iter 1
"""

import re
import sys
import json
import argparse
import tempfile
from pathlib import Path
from dataclasses import dataclass

SCRIPT_DIR     = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent
sys.path.insert(0, str(SCRIPT_DIR))

from cbmc_runner import run_cbmc, FUNC_CONFIGS
from feedback_loop import PILOT_FUNCTIONS, RESULTS_DIR

# ── Assume extraction ─────────────────────────────────────────────────────────

# Matches  __CPROVER_assume(<balanced-parens-expr>);
_ASSUME_RE = re.compile(
    r'__CPROVER_assume\s*\(([^()]*(?:\([^()]*\)[^()]*)*)\)\s*;'
)

def extract_assumes(code: str) -> list[dict]:
    """
    Return list of assumes found in the harness code.
    Each entry: {"expr": "...", "full": "__CPROVER_assume(...);", "start": int, "end": int}
    """
    assumes = []
    for m in _ASSUME_RE.finditer(code):
        assumes.append({
            "expr":  m.group(1).strip(),
            "full":  m.group(0),
            "start": m.start(),
            "end":   m.end(),
        })
    return assumes


def remove_assume(code: str, assume: dict) -> str:
    """Remove a single assume from the code (replace with blank line)."""
    return code[:assume["start"]] + "/* removed */" + code[assume["end"]:]


# ── Minimization ─────────────────────────────────────────────────────────────

@dataclass
class AssumeResult:
    expr:        str
    necessary:   bool    # False = unnecessary (CBMC passes without it)
    cbmc_verify: str     # CBMC result after removal


@dataclass
class MinimizationResult:
    func_name:    str
    total_assumes: int
    necessary:    list   # AssumeResult where necessary=True
    unnecessary:  list   # AssumeResult where necessary=False
    minimal_code: str    # harness with all unnecessary assumes removed

    def summary(self) -> dict:
        return {
            "func":              self.func_name,
            "total_assumes":     self.total_assumes,
            "necessary_count":   len(self.necessary),
            "unnecessary_count": len(self.unnecessary),
            "unnecessary_exprs": [a.expr for a in self.unnecessary],
            "necessary_exprs":   [a.expr for a in self.necessary],
        }


def minimize(func_name: str, harness_code: str,
             verbose: bool = True) -> MinimizationResult:
    """
    Run precondition minimization for one harness.
    Returns MinimizationResult.
    """
    assumes = extract_assumes(harness_code)

    if verbose:
        print(f"\n  {func_name}: {len(assumes)} __CPROVER_assume found")
        for a in assumes:
            print(f"    __CPROVER_assume({a['expr']})")

    necessary   = []
    unnecessary = []

    for assume in assumes:
        trimmed = remove_assume(harness_code, assume)

        # Write to temp file and run CBMC
        with tempfile.NamedTemporaryFile(suffix=".c", mode="w",
                                         delete=False, dir="/tmp") as f:
            f.write(trimmed)
            tmp_path = Path(f.name)

        result = run_cbmc(func_name, tmp_path)
        tmp_path.unlink(missing_ok=True)

        ar = AssumeResult(
            expr        = assume["expr"],
            necessary   = result.verification_result != "SUCCESS",
            cbmc_verify = result.verification_result,
        )

        tag = "NECESSARY  " if ar.necessary else "UNNECESSARY"
        if verbose:
            print(f"    [{tag}]  __CPROVER_assume({assume['expr']})"
                  f"  → remove → {result.verification_result}")

        if ar.necessary:
            necessary.append(ar)
        else:
            unnecessary.append(ar)

    # Build minimal harness (all unnecessary assumes stripped)
    minimal = harness_code
    # Remove in reverse order to preserve indices
    for assume in reversed(sorted(unnecessary,
                                   key=lambda a: harness_code.find(a.expr))):
        # Find and blank out the full assume statement
        full_re = re.compile(
            r'__CPROVER_assume\s*\(\s*' + re.escape(assume.expr) + r'\s*\)\s*;'
        )
        minimal = full_re.sub("/* minimized out */", minimal, count=1)

    return MinimizationResult(
        func_name     = func_name,
        total_assumes = len(assumes),
        necessary     = necessary,
        unnecessary   = unnecessary,
        minimal_code  = minimal,
    )


# ── Finding harnesses ─────────────────────────────────────────────────────────

def find_harness(func_name: str, variant: str, iteration: int) -> Path | None:
    p = RESULTS_DIR / variant / func_name / f"iter_{iteration}_harness.c"
    return p if p.exists() else None


# ── Main ─────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--func",    help="Function name (e.g. aws_byte_buf_init)")
    parser.add_argument("--all",     action="store_true", help="Run all pilot functions")
    parser.add_argument("--variant", default="feedback_loop_B_claude",
                        help="Result variant dir (default: feedback_loop_B_claude)")
    parser.add_argument("--iter",    type=int, default=1,
                        help="Harness iteration (default: 1)")
    parser.add_argument("--save",    action="store_true",
                        help="Save minimal harness and JSON report")
    args = parser.parse_args()

    funcs = PILOT_FUNCTIONS if args.all else []
    if args.func:
        funcs = [(d, n) for d, n in PILOT_FUNCTIONS if n == args.func]
        if not funcs:
            funcs = [("", args.func)]

    if not funcs:
        parser.print_help()
        sys.exit(1)

    all_results = []

    for _, func_name in funcs:
        harness_path = find_harness(func_name, args.variant, args.iter)
        if harness_path is None:
            print(f"  [SKIP] {func_name}: no harness at "
                  f"{args.variant}/iter_{args.iter}")
            continue

        if func_name not in FUNC_CONFIGS:
            print(f"  [SKIP] {func_name}: not in FUNC_CONFIGS")
            continue

        code = harness_path.read_text()
        result = minimize(func_name, code, verbose=True)
        all_results.append(result)

        if args.save:
            out_dir = RESULTS_DIR / args.variant / func_name
            (out_dir / f"iter_{args.iter}_minimal.c").write_text(result.minimal_code)
            (out_dir / f"iter_{args.iter}_minimize_report.json").write_text(
                json.dumps(result.summary(), indent=2)
            )
            print(f"  Saved minimal harness → {out_dir}")

    # ── Summary table ─────────────────────────────────────────────────────────
    if all_results:
        print(f"\n{'='*65}")
        print(f"{'PRECONDITION MINIMIZATION SUMMARY':^65}")
        print(f"{'='*65}")
        print(f"{'Function':<35} {'Total':^6} {'Needed':^7} {'Extra':^7}")
        print(f"{'-'*65}")
        for r in all_results:
            print(f"{r.func_name:<35} {r.total_assumes:^6} "
                  f"{len(r.necessary):^7} {len(r.unnecessary):^7}")
            for a in r.unnecessary:
                print(f"  {'EXTRA →':<10} __CPROVER_assume({a.expr})")
        print(f"{'='*65}")

        total    = sum(r.total_assumes  for r in all_results)
        extra    = sum(len(r.unnecessary) for r in all_results)
        if total:
            print(f"\nOverall: {extra}/{total} assumes unnecessary "
                  f"({extra/total:.0%} over-constraining rate)")


if __name__ == "__main__":
    main()
