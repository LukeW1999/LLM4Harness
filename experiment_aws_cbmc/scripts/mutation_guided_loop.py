#!/usr/bin/env python3
"""
mutation_guided_loop.py
=======================
Mutation-guided harness improvement: feed surviving mutants back to the LLM
to improve postcondition completeness.

For each function where LLM score < GT score:
  1. Load the existing LLM harness (iter_1_harness.c)
  2. Read survived mutants from the mutation report
  3. Construct a feedback prompt explaining which bugs the harness missed
  4. Ask Claude to add missing assertions to the harness
  5. Run CBMC to verify the improved harness still passes
  6. Re-run mutation testing to measure improvement
  7. Save as iter_1_mutguided_harness.c + mutguided_report.json

Usage:
    python mutation_guided_loop.py --func aws_byte_buf_init
    python mutation_guided_loop.py --all --variant feedback_loop_C_claude
    python mutation_guided_loop.py --all --variant feedback_loop_C_claude --dry-run
"""

import sys
import json
import argparse
from pathlib import Path
from dataclasses import dataclass

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))

from cbmc_runner   import run_cbmc, FUNC_CONFIGS
from feedback_loop import PILOT_FUNCTIONS, RESULTS_DIR, SYSTEM_PROMPT, extract_c_code
from mutation_test import run_mutation_test

EVAL_DIR = SCRIPT_DIR.parent / "evaluation"


# ── Prompt construction ───────────────────────────────────────────────────────

def _label_to_description(label: str) -> str:
    """
    Convert a mutation label like "SKIP_WRITE(buf->len)" to a human-readable
    description of what bug it introduces and what assertion would catch it.
    """
    if label.startswith("SKIP_WRITE("):
        field = label[len("SKIP_WRITE("):-1]
        return (f"SKIP_WRITE({field}): the function was modified to NOT assign {field}. "
                f"Your harness should assert the final value of {field}.")
    elif label.startswith("ASSIGN_ZERO("):
        field = label[len("ASSIGN_ZERO("):-1]
        return (f"ASSIGN_ZERO({field}): the function was modified to set {field} = 0 "
                f"instead of the correct value. "
                f"Your harness should assert {field} equals its expected non-zero value.")
    elif label.startswith("FLIP_RETURN("):
        detail = label[len("FLIP_RETURN("):-1]
        return (f"FLIP_RETURN({detail}): the function was modified to return the wrong "
                f"success/error code. "
                f"Your harness should assert the return value.")
    return label


def build_mutation_feedback_prompt(func_name: str, harness_code: str,
                                   missed_labels: list[str],
                                   func_source: str = "") -> str:
    """
    Build a prompt asking the LLM to add assertions that would catch the
    surviving mutants.
    """
    descriptions = "\n".join(
        f"  {i+1}. {_label_to_description(lbl)}"
        for i, lbl in enumerate(missed_labels)
    )

    source_block = ""
    if func_source:
        source_block = f"""
The function source code (for reference):
```c
{func_source}
```
""".strip()

    prompt = f"""You wrote a CBMC harness for `{func_name}`. Below is your current harness.

Your harness was evaluated using mutation testing: small bugs were introduced
into `{func_name}` and we checked whether CBMC + your harness would detect them.

The following mutations SURVIVED (i.e., your harness did NOT detect these bugs):

{descriptions}

{source_block}

Your current harness:
```c
{harness_code}
```

Please update the harness to add assertions that would catch each of the above
mutations. Keep all existing __CPROVER_assume() preconditions. Only add new
assert() statements or strengthen existing ones.

Return ONLY the complete updated harness C code, with no explanation.
"""
    return prompt


# ── Per-function guided improvement ──────────────────────────────────────────

@dataclass
class GuidedResult:
    func:           str
    missed_before:  int
    missed_after:   int
    score_before:   float
    score_after:    float
    cbmc_passed:    bool
    harness_path:   Path | None


def run_guided(func_name: str, variant: str, iteration: int = 1,
               dry_run: bool = False, verbose: bool = True) -> GuidedResult | None:
    """
    Run mutation-guided improvement for one function.
    Returns GuidedResult or None if prerequisites missing.
    """
    from call_claude_api import call_qwen

    func_dir = RESULTS_DIR / variant / func_name

    # Find the best mutation report (highest iter) if no specific iter requested
    reports = sorted(
        func_dir.glob("iter_*_mutation_report.json"),
        key=lambda p: int(p.stem.split("_")[1])
    ) if func_dir.exists() else []

    if reports and iteration == 1:
        # Use best available report automatically
        report_path = reports[-1]
        iteration = int(report_path.stem.split("_")[1])
    else:
        report_path = func_dir / f"iter_{iteration}_mutation_report.json"

    harness_path = func_dir / f"iter_{iteration}_harness.c"

    if not harness_path.exists():
        if verbose: print(f"  [SKIP] {func_name}: no harness at iter {iteration}")
        return None

    if not report_path.exists():
        if verbose: print(f"  [SKIP] {func_name}: no mutation report at iter {iteration}")
        return None

    if func_name not in FUNC_CONFIGS:
        if verbose: print(f"  [SKIP] {func_name}: not in FUNC_CONFIGS")
        return None

    report     = json.loads(report_path.read_text())
    missed     = report.get("missed_by_llm", [])
    score_orig = report.get("llm_score", 0.0)

    if not missed:
        if verbose: print(f"  [SKIP] {func_name}: no missed mutants (score={score_orig:.0%})")
        return None

    harness_code = harness_path.read_text()

    # Optionally include function source for context
    from mutation_test import _find_func_source_file
    src_path, _ = _find_func_source_file(func_name)
    func_source = ""
    if src_path and src_path.exists():
        from mutation_test import _extract_func_body
        src_text = src_path.read_text()
        body_range = _extract_func_body(func_name, src_text)
        if body_range:
            func_source = src_text[body_range[0]:body_range[1]]

    if verbose:
        print(f"\n  {func_name}: {len(missed)} missed mutants, score={score_orig:.0%}")
        for m in missed:
            print(f"    → {m}")

    if dry_run:
        return GuidedResult(func_name, len(missed), -1, score_orig, -1.0, False, None)

    # Build and send prompt
    prompt = build_mutation_feedback_prompt(func_name, harness_code, missed, func_source)

    if verbose:
        print(f"  Calling Claude to fix {len(missed)} missed mutations...")

    try:
        response = call_qwen(SYSTEM_PROMPT, prompt, temperature=0.0)
        improved_code = extract_c_code(response)
    except Exception as e:
        print(f"  [ERROR] {func_name}: Claude API error: {e}")
        return None

    # Save improved harness
    improved_path = func_dir / f"iter_{iteration}_mutguided_harness.c"
    improved_path.write_text(improved_code)

    # Verify with CBMC
    cbmc_result = run_cbmc(func_name, improved_path)
    cbmc_passed = cbmc_result.verification_result == "SUCCESS"

    if verbose:
        print(f"  CBMC: {cbmc_result.verification_result} "
              f"({'✓ passes' if cbmc_passed else '✗ fails'})")

    if not cbmc_passed:
        # Save anyway but note CBMC failure
        guided_report = {
            "func": func_name, "iteration": iteration,
            "missed_before": missed, "score_before": score_orig,
            "cbmc_result": cbmc_result.verification_result,
            "note": "CBMC failed after guided improvement — may need precondition fix",
        }
        (func_dir / f"iter_{iteration}_mutguided_report.json").write_text(
            json.dumps(guided_report, indent=2))
        return GuidedResult(func_name, len(missed), -1, score_orig, -1.0, False, improved_path)

    # Re-run mutation testing on improved harness
    if verbose:
        print(f"  Re-running mutation test on improved harness...")

    new_report = run_mutation_test(func_name, improved_path, verbose=False)
    score_new  = new_report.llm_score
    missed_new = [r.label for r in new_report.results
                  if r.gt_killed and not r.llm_killed]

    if verbose:
        print(f"  Score: {score_orig:.0%} → {score_new:.0%} "
              f"(missed: {len(missed)} → {len(missed_new)})")

    # Save detailed report
    guided_report = {
        "func":          func_name,
        "iteration":     iteration,
        "score_before":  round(score_orig, 3),
        "score_after":   round(score_new,  3),
        "missed_before": missed,
        "missed_after":  missed_new,
        "cbmc_passed":   cbmc_passed,
    }
    (func_dir / f"iter_{iteration}_mutguided_report.json").write_text(
        json.dumps(guided_report, indent=2))

    return GuidedResult(
        func=func_name, missed_before=len(missed), missed_after=len(missed_new),
        score_before=score_orig, score_after=score_new,
        cbmc_passed=cbmc_passed, harness_path=improved_path,
    )


# ── Main ─────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--func",    help="Function name")
    parser.add_argument("--all",     action="store_true")
    parser.add_argument("--variant", default="feedback_loop_C_claude")
    parser.add_argument("--iter",    type=int, default=1)
    parser.add_argument("--dry-run", action="store_true",
                        help="Print planned prompts, no API calls")
    parser.add_argument("--verbose", action="store_true", default=True)
    args = parser.parse_args()

    _, func_names = zip(*PILOT_FUNCTIONS)
    funcs = list(func_names) if args.all else ([args.func] if args.func else [])
    if not funcs:
        parser.print_help(); return

    results = []
    for func_name in funcs:
        r = run_guided(func_name, args.variant, args.iter,
                       dry_run=args.dry_run, verbose=args.verbose)
        if r:
            results.append(r)

    # Summary
    if len(results) > 1:
        improved = [r for r in results if r.cbmc_passed and r.score_after > r.score_before]
        print(f"\n{'='*60}")
        print(f"MUTATION-GUIDED IMPROVEMENT SUMMARY ({args.variant})")
        print(f"{'='*60}")
        print(f"{'Function':<35} {'Before':^8} {'After':^8} {'CBMC':^6}")
        print(f"{'-'*60}")
        for r in results:
            after_str = f"{r.score_after:.0%}" if r.score_after >= 0 else "N/A"
            cbmc_str  = "✓" if r.cbmc_passed else "✗"
            print(f"  {r.func:<33} {r.score_before:^8.0%} {after_str:^8} {cbmc_str:^6}")
        print(f"{'='*60}")
        if improved:
            avg_delta = sum(r.score_after - r.score_before for r in improved) / len(improved)
            print(f"\n{len(improved)} functions improved: avg Δ = {avg_delta:+.0%}")

        # Save aggregate
        if not args.dry_run:
            EVAL_DIR.mkdir(exist_ok=True)
            agg = {
                "variant":   args.variant,
                "iteration": args.iter,
                "results": [
                    {"func": r.func, "score_before": r.score_before,
                     "score_after": r.score_after if r.score_after >= 0 else None,
                     "cbmc_passed": r.cbmc_passed}
                    for r in results
                ],
            }
            out = EVAL_DIR / f"mutguided_{args.variant}_iter{args.iter}.json"
            out.write_text(json.dumps(agg, indent=2))
            print(f"\nSaved → {out}")


if __name__ == "__main__":
    main()
