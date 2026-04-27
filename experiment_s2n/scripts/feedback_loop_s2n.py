#!/usr/bin/env python3
"""
feedback_loop_s2n.py
====================
CBMC-guided iterative harness generation for s2n-tls stuffer functions.

Conditions:
  A : NL docstring + implementation (no few-shot example)
  E : NL docstring + implementation + same-family GT harness as few-shot example

Usage:
    python feedback_loop_s2n.py --func s2n_stuffer_init --condition A
    python feedback_loop_s2n.py --all --condition E --max-iter 4
    python feedback_loop_s2n.py --func s2n_stuffer_write --condition A --model claude
"""

import sys
import json
import argparse
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
sys.path.insert(0, str(script_dir))

from cbmc_runner_s2n import run_cbmc, run_gt, CBMCResult, PROOFS_DIR

# LLM backend (loaded dynamically by --model)
call_llm = None

def _load_model_backend(model: str):
    global call_llm
    sys.path.insert(0, str(script_dir.parent.parent / "experiment_aws_cbmc/scripts"))
    if model == "claude":
        from call_claude_api import call_qwen as _f
    else:
        from call_qwen_api import call_qwen as _f
    call_llm = _f


def extract_c_code(response: str) -> str:
    """Extract C code block from LLM response, stripping markdown fences."""
    s = response.strip()
    if s.startswith("```c\n"):
        s = s[5:]
    elif s.startswith("```\n"):
        s = s[4:]
    last_brace = s.rfind("}")
    if last_brace != -1:
        s = s[:last_brace + 1]
    s = s.rstrip("`").strip()
    return s + "\n"


DATASET_DIR  = experiment_dir / "dataset"
PROMPTS_DIR  = experiment_dir / "prompts"
RESULTS_DIR  = experiment_dir / "results"
EVAL_DIR     = experiment_dir / "evaluation"

CONDITION_PROMPT = {
    "A": "prompt_s2n_condA.txt",
    "E": "prompt_s2n_condE.txt",
}

ACTIVE_CONDITION = "A"
ACTIVE_MODEL     = "qwen"

# 25 target functions (all GT-verified SUCCESS)
TARGET_FUNCS = [
    "s2n_stuffer_init",
    "s2n_stuffer_alloc",
    "s2n_stuffer_growable_alloc",
    "s2n_stuffer_free",
    "s2n_stuffer_resize",
    "s2n_stuffer_resize_if_empty",
    "s2n_stuffer_rewrite",
    "s2n_stuffer_rewind_read",
    "s2n_stuffer_wipe",
    "s2n_stuffer_wipe_n",
    "s2n_stuffer_skip_read",
    "s2n_stuffer_skip_write",
    "s2n_stuffer_read",
    "s2n_stuffer_write",
    "s2n_stuffer_read_bytes",
    "s2n_stuffer_write_bytes",
    "s2n_stuffer_erase_and_read",
    "s2n_stuffer_read_uint8",
    "s2n_stuffer_write_uint8",
    "s2n_stuffer_read_uint32",
    "s2n_stuffer_write_uint32",
    "s2n_stuffer_copy",
    "s2n_stuffer_extract_blob",
    "s2n_stuffer_reserve_space",
    "s2n_stuffer_is_consumed",
]

# For condition E: few-shot GT example for each function.
# All are from the same s2n_stuffer family — use s2n_stuffer_init as the example
# (excluded from E evaluation since it would see its own harness).
_E_EXAMPLE_FUNC = "s2n_stuffer_init"
_E_EXAMPLE_HARNESS_PATH = PROOFS_DIR / "s2n_stuffer_init" / "s2n_stuffer_init_harness.c"
_E_EXCLUDED = {"s2n_stuffer_init"}   # don't run E on the example function itself

SYSTEM_PROMPT = (
    "You are an expert in CBMC formal verification of C programs. "
    "Output only valid C code, with no explanations or text after the code."
)


def _load_dataset() -> dict:
    path = DATASET_DIR / "s2n_stuffer_dataset.json"
    if not path.exists():
        raise FileNotFoundError(f"Dataset not found: {path}. Run build_dataset.py first.")
    return json.loads(path.read_text())


def build_header_content(entry: dict) -> str:
    """Compose header content block from dataset entry."""
    lines = []
    # Struct definitions (static — same for all)
    lines.append("""/* s2n_stuffer — buffer with read/write cursors */
struct s2n_stuffer {
    struct s2n_blob blob;
    uint32_t read_cursor;
    uint32_t write_cursor;
    uint32_t high_water_mark;
    unsigned int alloced  : 1;
    unsigned int growable : 1;
    unsigned int tainted  : 1;
};
#define s2n_stuffer_data_available(s)  ((s)->write_cursor - (s)->read_cursor)
#define s2n_stuffer_space_remaining(s) ((s)->blob.size - (s)->write_cursor)

/* s2n_blob — raw byte buffer */
struct s2n_blob {
    uint8_t *data;
    uint32_t size;
    uint32_t allocated;
    unsigned int growable : 1;
};

/* Validity predicates */
S2N_RESULT s2n_stuffer_validate(const struct s2n_stuffer *stuffer);
S2N_RESULT s2n_blob_validate(const struct s2n_blob *blob);
""")

    if entry.get("docstring"):
        lines.append(entry["docstring"])

    lines.append(entry["header_declaration"])
    return "\n".join(lines)


def build_initial_prompt(func_name: str, entry: dict) -> str:
    """Build initial generation prompt for the active condition."""
    prompt_file = CONDITION_PROMPT[ACTIVE_CONDITION]
    template = (PROMPTS_DIR / prompt_file).read_text()

    header_content = build_header_content(entry)
    impl_content   = entry.get("implementation", "/* implementation not available */")

    prompt = template
    prompt = prompt.replace("{HEADER_CONTENT}", header_content)
    prompt = prompt.replace("{IMPLEMENTATION_CONTENT}", impl_content)
    prompt = prompt.replace("{FUNCTION_NAME}", func_name)

    if ACTIVE_CONDITION == "E":
        example_func    = _E_EXAMPLE_FUNC
        example_harness = _E_EXAMPLE_HARNESS_PATH.read_text() if _E_EXAMPLE_HARNESS_PATH.exists() else "/* unavailable */"
        prompt = prompt.replace("{EXAMPLE_FUNC}",    example_func)
        prompt = prompt.replace("{EXAMPLE_HARNESS}", example_harness)

    return prompt


def build_fix_compilation_prompt(harness_code: str, func_name: str, error_msg: str, iteration: int) -> str:
    return f"""Your CBMC harness for `{func_name}` has compilation errors. Fix it.

## Your harness (has errors):
```c
{harness_code}
```

## Compilation error:
```
{error_msg[:1500]}
```

## Key rules for s2n-tls harnesses:
- Include: `#include <assert.h>`, `#include <cbmc_proof/make_common_datastructures.h>`, `#include "stuffer/s2n_stuffer.h"`
- Allocate: `struct s2n_stuffer *stuffer = cbmc_allocate_s2n_stuffer();`
- Constrain: `__CPROVER_assume(s2n_result_is_ok(s2n_stuffer_validate(stuffer)));`
- Return check: `if (result == S2N_SUCCESS)` (not AWS_OP_SUCCESS)
- Entry point: `void {func_name}_harness()`
- Do NOT use undefined macros or missing functions

Output ONLY the corrected C code. No explanations. Iteration {iteration}."""


def build_fix_verification_prompt(harness_code: str, func_name: str, failed_lines: str, iteration: int) -> str:
    return f"""Your CBMC harness for `{func_name}` compiles but VERIFICATION FAILED.

## Your harness (verification fails):
```c
{harness_code}
```

## Failed CBMC checks:
```
{failed_lines[:1500]}
```

## How to fix:
- If assertion too strong: weaken it or add `__CPROVER_assume` to constrain the input further
- If precondition missing: add `__CPROVER_assume(condition)` before the call
- If postcondition wrong: re-read the implementation to see the exact cursor arithmetic
- Remember: stuffer macros: `s2n_stuffer_data_available(s) = s->write_cursor - s->read_cursor`
- Remember: `s2n_stuffer_space_remaining(s) = s->blob.size - s->write_cursor`

Output ONLY the corrected C harness code. No explanations. Iteration {iteration}."""


def build_fix_unknown_prompt(harness_code: str, func_name: str, iteration: int) -> str:
    return f"""Your CBMC harness for `{func_name}` produced UNKNOWN result (no reachable assertions).
This means the harness has no assert() calls or all code paths are unreachable.

## Your harness:
```c
{harness_code}
```

Please rewrite ensuring:
- At least one `assert(condition)` postcondition
- The function is actually called
- All code paths are reachable

Output ONLY the corrected C code. No explanations. Iteration {iteration}."""


@dataclass
class IterationRecord:
    iteration: int
    harness_code: str
    cbmc_result: CBMCResult
    action_taken: str


@dataclass
class FeedbackResult:
    func_name: str
    gt_result: CBMCResult
    iterations: list = field(default_factory=list)

    def final_result(self) -> Optional[CBMCResult]:
        return self.iterations[-1].cbmc_result if self.iterations else None

    def converged(self) -> bool:
        r = self.final_result()
        return r is not None and r.verification_result == "SUCCESS"

    def first_pass_compile(self) -> bool:
        return self.iterations[0].cbmc_result.compilation_ok if self.iterations else False

    def first_pass_verify(self) -> bool:
        return (self.iterations[0].cbmc_result.verification_result == "SUCCESS"
                if self.iterations else False)

    def to_dict(self) -> dict:
        return {
            "func": self.func_name,
            "condition": ACTIVE_CONDITION,
            "model": ACTIVE_MODEL,
            "gt_verification": self.gt_result.verification_result,
            "first_pass_compile": self.first_pass_compile(),
            "first_pass_verify": self.first_pass_verify(),
            "converged": self.converged(),
            "num_iterations": len(self.iterations),
            "iterations": [
                {
                    "iter": rec.iteration,
                    "action": rec.action_taken,
                    "compile_ok": rec.cbmc_result.compilation_ok,
                    "verify": rec.cbmc_result.verification_result,
                    "num_failed": rec.cbmc_result.num_failed,
                    "num_checks": rec.cbmc_result.num_checks,
                }
                for rec in self.iterations
            ]
        }


def run_feedback_loop(func_name: str, max_iterations: int = 4) -> FeedbackResult:
    """Run the CBMC feedback loop for one function."""
    dataset = _load_dataset()
    if func_name not in dataset:
        raise ValueError(f"Function {func_name} not in dataset")

    print(f"\n{'='*60}")
    print(f"Function: {func_name}  [{ACTIVE_CONDITION}, {ACTIVE_MODEL}, max {max_iterations} iters]")
    print(f"{'='*60}")

    entry  = dataset[func_name]
    result = FeedbackResult(func_name=func_name, gt_result=run_gt(func_name))
    print(f"  GT baseline: {result.gt_result.verification_result}")

    model_suffix = f"_{ACTIVE_MODEL}" if ACTIVE_MODEL != "qwen" else ""
    output_dir = RESULTS_DIR / f"s2n_cond{ACTIVE_CONDITION}{model_suffix}" / func_name
    output_dir.mkdir(parents=True, exist_ok=True)

    # ── Iteration 1: initial generation ──────────────────────────────────────
    print(f"\n  [Iter 1] Generating initial harness...")
    prompt       = build_initial_prompt(func_name, entry)
    harness_code = extract_c_code(call_llm(SYSTEM_PROMPT, prompt))

    harness_path = output_dir / "iter_1_harness.c"
    harness_path.write_text(harness_code)

    cbmc_result = run_cbmc(func_name, harness_path)
    print(f"    Compile: {'OK' if cbmc_result.compilation_ok else 'FAIL'}")
    print(f"    Verify:  {cbmc_result.verification_result}  ({cbmc_result.num_failed}/{cbmc_result.num_checks})")

    result.iterations.append(IterationRecord(1, harness_code, cbmc_result, "initial"))

    # ── Feedback iterations ───────────────────────────────────────────────────
    for i in range(2, max_iterations + 1):
        if cbmc_result.verification_result == "SUCCESS":
            print(f"  Converged at iteration {i-1}!")
            break

        failed_lines = "\n".join(
            l for l in cbmc_result.stdout.split("\n")
            if ": FAILED" in l or ": FAILURE" in l or "VERIFICATION FAILED" in l
        )

        if not cbmc_result.compilation_ok:
            action     = "fix_compile"
            fix_prompt = build_fix_compilation_prompt(harness_code, func_name, cbmc_result.error_summary, i)
            print(f"\n  [Iter {i}] Fixing compilation error...")
        elif cbmc_result.verification_result == "FAIL":
            action     = "fix_verify"
            fix_prompt = build_fix_verification_prompt(harness_code, func_name, failed_lines, i)
            print(f"\n  [Iter {i}] Fixing verification failure...")
        elif cbmc_result.verification_result in ("UNKNOWN", "TIMEOUT"):
            action     = "fix_unknown"
            fix_prompt = build_fix_unknown_prompt(harness_code, func_name, i)
            print(f"\n  [Iter {i}] Fixing UNKNOWN/TIMEOUT...")
        else:
            break

        harness_code = extract_c_code(call_llm(SYSTEM_PROMPT, fix_prompt))
        harness_path = output_dir / f"iter_{i}_harness.c"
        harness_path.write_text(harness_code)

        cbmc_result = run_cbmc(func_name, harness_path)
        print(f"    Compile: {'OK' if cbmc_result.compilation_ok else 'FAIL'}")
        print(f"    Verify:  {cbmc_result.verification_result}  ({cbmc_result.num_failed}/{cbmc_result.num_checks})")

        result.iterations.append(IterationRecord(i, harness_code, cbmc_result, action))

    # Save summary
    (output_dir / "summary.json").write_text(json.dumps(result.to_dict(), indent=2))
    return result


def print_summary_table(results: list):
    w = 80
    print(f"\n{'='*w}")
    print(f"{'S2N-TLS FEEDBACK LOOP SUMMARY':^{w}}")
    print(f"{'='*w}")
    print(f"{'Function':<35} {'GT':^6} {'P1C':^5} {'P1V':^5} {'Final':^8} {'Conv':^5} {'N':^4}")
    print(f"{'-'*w}")
    for r in results:
        gt    = r.gt_result.verification_result[:4]
        p1c   = "OK" if r.first_pass_compile() else "no"
        p1v   = "OK" if r.first_pass_verify() else "no"
        final = (r.final_result().verification_result[:4] if r.final_result() else "N/A")
        conv  = "YES" if r.converged() else "no"
        n     = len(r.iterations)
        print(f"{r.func_name:<35} {gt:^6} {p1c:^5} {p1v:^5} {final:^8} {conv:^5} {n:^4}")
    print(f"{'='*w}")

    n = len(results)
    if n:
        p1c  = sum(1 for r in results if r.first_pass_compile()) / n
        p1v  = sum(1 for r in results if r.first_pass_verify()) / n
        conv = sum(1 for r in results if r.converged()) / n
        avg  = sum(len(r.iterations) for r in results) / n
        print(f"\nAggregates (n={n}, cond={ACTIVE_CONDITION}, model={ACTIVE_MODEL}):")
        print(f"  First-pass compile: {p1c:.0%}")
        print(f"  First-pass verify:  {p1v:.0%}")
        print(f"  Convergence:        {conv:.0%}")
        print(f"  Avg iterations:     {avg:.1f}")


def main():
    global ACTIVE_CONDITION, ACTIVE_MODEL
    parser = argparse.ArgumentParser(description="s2n-tls CBMC feedback loop")
    parser.add_argument("--func",      help="Single function name")
    parser.add_argument("--all",       action="store_true", help="Run all 25 target functions")
    parser.add_argument("--max-iter",  type=int, default=4)
    parser.add_argument("--condition", choices=["A", "E"], default="A")
    parser.add_argument("--model",     choices=["qwen", "claude"], default="qwen")
    parser.add_argument("--save-json", action="store_true")
    args = parser.parse_args()

    ACTIVE_CONDITION = args.condition
    ACTIVE_MODEL     = args.model
    _load_model_backend(args.model)
    print(f"Condition: {ACTIVE_CONDITION}  Model: {ACTIVE_MODEL}")

    if args.func:
        funcs = [args.func]
    elif args.all:
        funcs = list(TARGET_FUNCS)
        if ACTIVE_CONDITION == "E":
            funcs = [f for f in funcs if f not in _E_EXCLUDED]
    else:
        parser.print_help()
        sys.exit(1)

    results = []
    model_suffix = f"_{ACTIVE_MODEL}" if ACTIVE_MODEL != "qwen" else ""
    for func_name in funcs:
        output_dir   = RESULTS_DIR / f"s2n_cond{ACTIVE_CONDITION}{model_suffix}" / func_name
        summary_path = output_dir / "summary.json"
        if summary_path.exists():
            saved = json.loads(summary_path.read_text())
            print(f"  SKIP (done): {func_name}  [converged={saved.get('converged')}]")
            continue
        try:
            r = run_feedback_loop(func_name, max_iterations=args.max_iter)
            results.append(r)
        except Exception as e:
            print(f"  ERROR {func_name}: {e}")
            import traceback; traceback.print_exc()

    if len(results) > 1:
        print_summary_table(results)

    if args.save_json and results:
        EVAL_DIR.mkdir(parents=True, exist_ok=True)
        out = EVAL_DIR / f"s2n_cond{ACTIVE_CONDITION}{model_suffix}_results.json"
        out.write_text(json.dumps([r.to_dict() for r in results], indent=2))
        print(f"\nSaved to: {out}")


if __name__ == "__main__":
    main()
