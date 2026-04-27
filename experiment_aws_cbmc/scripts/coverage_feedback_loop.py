#!/usr/bin/env python3
"""
coverage_feedback_loop.py
=========================
Two-phase CBMC feedback loop with coverage-guided assume relaxation.

Phase 1 — Assertion loop (same as feedback_loop.py):
    Generate harness → CBMC → fix FAIL/compile errors → repeat until PASS or max_iter

Phase 2 — Coverage loop (new):
    After PASS: run CBMC --cover branch on the LLM harness.
    Extract FAILED branches in the target function's source files.
    Tell LLM: "these branches are unreachable — relax your __CPROVER_assume()".
    Verify the new harness still passes CBMC.
    Repeat until no blocked branches remain, or max_cov_iter reached.

IMPORTANT — what the LLM sees vs. what we measure:
    LLM receives:   FAILED branches from its OWN harness (no GT leakage)
    Evaluation:     coverage_ratio = |LLM ∩ GT| / |GT|  (internal, saved to summary)

Usage:
    python coverage_feedback_loop.py --func aws_byte_buf_init --condition B --model claude
    python coverage_feedback_loop.py --all --condition B --model claude
    python coverage_feedback_loop.py --func aws_byte_buf_init --cov-only --condition B
"""

import sys
import json
import argparse
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

SCRIPT_DIR     = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent
sys.path.insert(0, str(SCRIPT_DIR))

from cbmc_runner import run_cbmc, run_gt, CBMCResult, FUNC_CONFIGS, COMMON_FLAGS
from feedback_loop import (
    extract_c_code,
    build_initial_prompt,
    build_fix_compilation_prompt,
    build_fix_verification_prompt,
    build_fix_unknown_prompt,
    DATASET_DIR,
    PROMPTS_DIR,
    RESULTS_DIR,
    EVAL_DIR,
    SYSTEM_PROMPT,
    PILOT_FUNCTIONS,
    CONDITION_DATASET,
    CONDITION_PROMPT,
)

# call_qwen resolved at runtime
call_qwen = None

def _load_model_backend(model: str):
    global call_qwen
    if model == "claude":
        from call_claude_api import call_qwen as _cq
    else:
        from call_qwen_api import call_qwen as _cq
    call_qwen = _cq

# ── CBMC coverage helpers ─────────────────────────────────────────────────────

# Infrastructure files excluded from coverage feedback
_INFRA_FILES = {
    "make_common_data_structures.c",
    "error.c",
    "nondet.h",
}

def _run_cover_branch(func_name: str, harness_path: Path,
                      timeout: int = 300) -> Optional[str]:
    """Run CBMC --cover branch and return raw XML stdout, or None on failure."""
    cfg = FUNC_CONFIGS.get(func_name)
    if cfg is None:
        return None
    cmd = (
        ["cbmc", "--cover", "branch", "--xml-ui"]
        + COMMON_FLAGS
        + cfg.get("unwind", [])
        + cfg.get("unwindset", [])
        + cfg.get("defines", [])
        + [str(p) for p in cfg.get("project_sources", [])]
        + [str(p) for p in cfg.get("proof_sources", [])]
        + [str(harness_path)]
        + ["--function", f"{func_name}_harness"]
    )
    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
        return proc.stdout
    except (subprocess.TimeoutExpired, Exception):
        return None


def _parse_coverage(xml_text: str, target_source_files: set,
                    func_name: str = "") -> dict:
    """
    Parse CBMC --cover branch XML.

    Returns:
        {
          "satisfied": [branch_id, ...],   # reachable under current harness
          "failed":    [branch_id, ...],   # blocked by assumptions
          "target_failed": [              # blocked branches IN the target function itself
              {"file": ..., "function": ..., "line": ..., "description": ...}, ...
          ]
        }
    where branch_id = "file:function:line:description"

    "target_failed" is filtered to branches WHERE the CBMC location function
    attribute matches func_name.  This avoids flooding the LLM with hundreds
    of allocator/library branches that are irrelevant to its assume choices.
    For the GT-ratio computation we still use the full satisfied/failed sets.
    """
    result = {"satisfied": [], "failed": [], "target_failed": []}
    if not xml_text or not xml_text.strip():
        return result

    try:
        start = xml_text.find("<?xml")
        root = ET.fromstring(xml_text if start == -1 else xml_text[start:])
    except ET.ParseError:
        return result

    for goal in root.iter("goal"):
        status = goal.get("status", "")
        loc    = goal.find("location")
        if loc is None:
            continue

        fname = Path(loc.get("file", "")).name
        if fname in _INFRA_FILES:
            continue

        function    = loc.get("function", "?")
        line        = loc.get("line", "?")
        description = goal.get("description", "?")
        branch_id   = f"{fname}:{function}:{line}:{description}"

        if status == "SATISFIED":
            result["satisfied"].append(branch_id)
        else:
            result["failed"].append(branch_id)
            # Only surface branches directly inside the target function to LLM.
            # This avoids noise from allocator/library helper functions.
            if fname in target_source_files and function == func_name:
                result["target_failed"].append({
                    "file": fname,
                    "function": function,
                    "line": line,
                    "description": description,
                })

    return result


def _compute_coverage_ratio(llm_satisfied: list, gt_satisfied: list) -> Optional[float]:
    """coverage_ratio = |LLM ∩ GT| / |GT|  (internal evaluation metric, NOT shown to LLM)."""
    if not gt_satisfied:
        return None
    gt_set  = set(gt_satisfied)
    llm_set = set(llm_satisfied)
    return len(llm_set & gt_set) / len(gt_set)


def _load_gt_coverage(func_name: str) -> Optional[list]:
    """Load GT satisfied branches from the pre-computed benchmark."""
    p = RESULTS_DIR / "gt_branch_coverage.json"
    if not p.exists():
        return None
    data = json.loads(p.read_text())
    entry = data.get(func_name)
    if entry is None:
        return None
    return entry.get("satisfied", [])


def _get_target_source_files(func_name: str) -> set:
    """Basenames of the project source files for this function."""
    cfg = FUNC_CONFIGS.get(func_name, {})
    return {Path(p).name for p in cfg.get("project_sources", [])}


# ── Prompt builders ───────────────────────────────────────────────────────────

def build_coverage_fix_prompt(
    harness_code: str,
    func_name: str,
    target_failed: list,
    iteration: int,
) -> str:
    """
    Coverage feedback prompt.

    Shows LLM only the FAILED branches from its OWN harness in the target
    function source files.  No GT information is revealed.
    """
    # Format the blocked branches for readability
    if target_failed:
        branch_lines = []
        for b in target_failed:
            branch_lines.append(
                f"  {b['file']}  function={b['function']}  line={b['line']}"
                f"  → \"{b['description']}\""
            )
        blocked_text = "\n".join(branch_lines)
    else:
        blocked_text = "  (none — coverage is complete)"

    return f"""Your CBMC harness for `{func_name}` passes verification, but CBMC \
branch analysis shows the following execution paths in the target function are \
UNREACHABLE under your current assumptions:

{blocked_text}

Each "FAILED" branch means CBMC's SAT solver cannot find any input that reaches \
that point — your __CPROVER_assume() conditions are blocking those paths.

A complete proof harness should allow CBMC to explore all valid execution paths \
of the target function.

## How to fix:
1. Look at each blocked branch and ask: which of my __CPROVER_assume() calls prevents it?
2. Remove or weaken that assumption.
3. If you need to split cases, use `nondet_bool()` to branch instead of assuming one side:
   ```c
   if (nondet_bool()) {{
       __CPROVER_assume(capacity == 0);  // explore the NULL-buffer path
   }} else {{
       __CPROVER_assume(capacity > 0);   // explore the allocation path
   }}
   ```
4. The harness MUST still pass `cbmc --no-standard-checks` after your changes.

## Your current harness:
```c
{harness_code}
```

Rewrite the harness so the blocked branches become reachable, while still passing \
CBMC verification. Output ONLY the corrected C code. Coverage iteration {iteration}."""


# ── Per-function pipeline ─────────────────────────────────────────────────────

@dataclass
class CoverageIterRecord:
    phase: str           # "assert" or "coverage"
    iteration: int
    harness_code: str
    cbmc_result: CBMCResult
    coverage_data: Optional[dict]      # parsed from --cover branch (None if not run)
    coverage_ratio: Optional[float]    # vs GT (internal eval metric)
    num_target_failed: int             # blocked branches in target sources
    action_taken: str

    def to_dict(self) -> dict:
        return {
            "phase":             self.phase,
            "iteration":         self.iteration,
            "compile_ok":        self.cbmc_result.compilation_ok,
            "verify":            self.cbmc_result.verification_result,
            "num_failed_checks": self.cbmc_result.num_failed,
            "coverage_ratio":    round(self.coverage_ratio, 4)
                                 if self.coverage_ratio is not None else None,
            "num_target_failed_branches": self.num_target_failed,
            "action_taken":      self.action_taken,
        }


def run_coverage_feedback_loop(
    func_dir:        str,
    func_name:       str,
    condition:       str  = "B",
    model:           str  = "claude",
    max_assert_iter: int  = 4,
    max_cov_iter:    int  = 3,
    cov_threshold:   float = 0.80,
    save_harnesses:  bool  = True,
) -> list[CoverageIterRecord]:
    """
    Full two-phase coverage feedback loop for one function.

    Returns list of CoverageIterRecord (one per iteration across both phases).
    """
    print(f"\n{'='*64}")
    print(f"  {func_name}  [condition={condition}, model={model}]")
    print(f"{'='*64}")

    # Resolve dataset/prompt for the chosen condition
    active_dataset = CONDITION_DATASET.get(condition, DATASET_DIR)
    prompt_file    = CONDITION_PROMPT.get(condition, "prompt_condB.txt")

    target_src_files = _get_target_source_files(func_name)
    gt_satisfied     = _load_gt_coverage(func_name)   # None if no GT benchmark

    output_dir = RESULTS_DIR / f"coverage_loop_{condition}_{model}" / func_name
    output_dir.mkdir(parents=True, exist_ok=True)

    gt_result = run_gt(func_name)
    print(f"  GT baseline: {gt_result.verification_result}")

    records: list[CoverageIterRecord] = []

    # ── Phase 1: assertion loop ───────────────────────────────────────────────
    # Step 1a — initial generation
    print(f"\n  [Assert iter 1] generating initial harness …")

    func_path = active_dataset / func_dir
    header   = (func_path / "header.h").read_text(encoding="utf-8") if (func_path / "header.h").exists() else ""
    impl     = (func_path / "implementation.c").read_text(encoding="utf-8") if (func_path / "implementation.c").exists() else ""
    template = (PROMPTS_DIR / prompt_file).read_text(encoding="utf-8")
    initial_prompt = (
        template
        .replace("{HEADER_CONTENT}",         header)
        .replace("{IMPLEMENTATION_CONTENT}", impl)
        .replace("{FUNCTION_NAME}",          func_name)
    )

    harness_code = extract_c_code(call_qwen(SYSTEM_PROMPT, initial_prompt))
    harness_path = output_dir / "iter_1_harness.c"
    if save_harnesses:
        harness_path.write_text(harness_code, encoding="utf-8")

    cbmc_result = run_cbmc(func_name, harness_path)
    print(f"    compile={'OK' if cbmc_result.compilation_ok else 'FAIL'}"
          f"  verify={cbmc_result.verification_result}"
          f"  failed={cbmc_result.num_failed}/{cbmc_result.num_checks}")

    records.append(CoverageIterRecord(
        phase="assert", iteration=1, harness_code=harness_code,
        cbmc_result=cbmc_result, coverage_data=None,
        coverage_ratio=None, num_target_failed=-1,
        action_taken="initial",
    ))

    # Step 1b — fix assertion failures
    for i in range(2, max_assert_iter + 1):
        if cbmc_result.verification_result == "SUCCESS":
            break

        failed_lines = "\n".join(
            l for l in cbmc_result.stdout.split("\n")
            if ": FAILED" in l or ": FAILURE" in l
        )
        if not cbmc_result.compilation_ok:
            action      = "fix_compile"
            fix_prompt  = build_fix_compilation_prompt(harness_code, func_name,
                                                        cbmc_result.error_summary, i)
        elif cbmc_result.verification_result == "FAIL":
            action      = "fix_verify"
            fix_prompt  = build_fix_verification_prompt(harness_code, func_name,
                                                         failed_lines, i)
        elif cbmc_result.verification_result == "UNKNOWN":
            action      = "fix_unknown"
            fix_prompt  = build_fix_unknown_prompt(harness_code, func_name, i)
        else:
            break

        print(f"\n  [Assert iter {i}] {action} …")
        harness_code = extract_c_code(call_qwen(SYSTEM_PROMPT, fix_prompt))
        harness_path = output_dir / f"iter_{i}_harness.c"
        if save_harnesses:
            harness_path.write_text(harness_code, encoding="utf-8")

        cbmc_result = run_cbmc(func_name, harness_path)
        print(f"    compile={'OK' if cbmc_result.compilation_ok else 'FAIL'}"
              f"  verify={cbmc_result.verification_result}"
              f"  failed={cbmc_result.num_failed}/{cbmc_result.num_checks}")

        records.append(CoverageIterRecord(
            phase="assert", iteration=i, harness_code=harness_code,
            cbmc_result=cbmc_result, coverage_data=None,
            coverage_ratio=None, num_target_failed=-1,
            action_taken=action,
        ))

    # If Phase 1 didn't converge, skip Phase 2
    if cbmc_result.verification_result != "SUCCESS":
        print(f"\n  [Phase 1 did not converge — skipping coverage phase]")
        _save_summary(output_dir, func_name, gt_result, records)
        return records

    print(f"\n  [Phase 1 converged ✓]  Starting coverage phase …")

    # ── Phase 2: coverage loop ────────────────────────────────────────────────
    for cov_i in range(1, max_cov_iter + 1):
        print(f"\n  [Coverage iter {cov_i}] running --cover branch …")

        xml_out = _run_cover_branch(func_name, harness_path)
        if xml_out is None:
            print(f"    [WARN] coverage run failed — stopping coverage phase")
            break

        cov_data = _parse_coverage(xml_out, target_src_files, func_name)

        # Internal evaluation (never shown to LLM)
        ratio = _compute_coverage_ratio(cov_data["satisfied"], gt_satisfied) \
                if gt_satisfied else None

        n_sat     = len(cov_data["satisfied"])
        n_fail    = len(cov_data["failed"])
        n_target  = len(cov_data["target_failed"])
        ratio_str = f"{ratio:.1%}" if ratio is not None else "N/A (no GT)"
        print(f"    satisfied={n_sat}  failed={n_fail}"
              f"  target_blocked={n_target}  GT_ratio={ratio_str}")

        # Attach coverage data to the last record
        if records:
            records[-1].coverage_data   = cov_data
            records[-1].coverage_ratio  = ratio
            records[-1].num_target_failed = n_target

        # Save coverage JSON (the LLM's own coverage snapshot, no GT comparison)
        cov_snapshot = {
            "func_name":       func_name,
            "cov_iteration":   cov_i,
            "satisfied_count": n_sat,
            "failed_count":    n_fail,
            "target_failed":   cov_data["target_failed"],
            # GT ratio stored separately for paper evaluation
            "_internal_gt_coverage_ratio": ratio,
        }
        (output_dir / f"cov_{cov_i}_coverage.json").write_text(
            json.dumps(cov_snapshot, indent=2)
        )

        # Stop conditions
        if n_target == 0:
            print(f"    [No blocked branches in target function — coverage phase done ✓]")
            break
        if ratio is not None and ratio >= cov_threshold:
            print(f"    [GT ratio {ratio:.1%} ≥ threshold {cov_threshold:.0%} — done ✓]")
            break

        # Send coverage feedback to LLM (only its own FAILED branches, no GT)
        print(f"    [Asking LLM to relax {n_target} blocked branches …]")
        cov_prompt   = build_coverage_fix_prompt(harness_code, func_name,
                                                  cov_data["target_failed"], cov_i)
        new_harness  = extract_c_code(call_qwen(SYSTEM_PROMPT, cov_prompt))
        new_path     = output_dir / f"cov_{cov_i}_harness.c"
        if save_harnesses:
            new_path.write_text(new_harness, encoding="utf-8")

        # Verify the new harness still passes CBMC
        print(f"    Verifying new harness …")
        new_result = run_cbmc(func_name, new_path)
        print(f"    compile={'OK' if new_result.compilation_ok else 'FAIL'}"
              f"  verify={new_result.verification_result}")

        if new_result.verification_result != "SUCCESS":
            print(f"    [New harness does NOT pass CBMC — reverting, stopping coverage phase]")
            break

        # Accept the new harness and continue
        harness_code = new_harness
        harness_path = new_path
        cbmc_result  = new_result

        records.append(CoverageIterRecord(
            phase="coverage", iteration=cov_i, harness_code=harness_code,
            cbmc_result=cbmc_result, coverage_data=None,
            coverage_ratio=None, num_target_failed=-1,
            action_taken="coverage_relax",
        ))

    _save_summary(output_dir, func_name, gt_result, records)
    return records


def _save_summary(output_dir: Path, func_name: str,
                  gt_result: CBMCResult, records: list):
    summary = {
        "func":          func_name,
        "gt_verify":     gt_result.verification_result,
        "num_records":   len(records),
        "assert_iters":  sum(1 for r in records if r.phase == "assert"),
        "coverage_iters": sum(1 for r in records if r.phase == "coverage"),
        "final_verify":  records[-1].cbmc_result.verification_result if records else "N/A",
        "final_coverage_ratio": next(
            (r.coverage_ratio for r in reversed(records)
             if r.coverage_ratio is not None), None
        ),
        "iterations": [r.to_dict() for r in records],
    }
    (output_dir / "summary.json").write_text(json.dumps(summary, indent=2))
    print(f"\n  Summary saved → {output_dir / 'summary.json'}")


# ── CLI ───────────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--func",      help="Function name (e.g. aws_byte_buf_init)")
    parser.add_argument("--all",       action="store_true", help="Run all configured functions")
    parser.add_argument("--condition", choices=list(CONDITION_DATASET.keys()), default="B",
                        help="Dataset condition (default: B)")
    parser.add_argument("--model",     choices=["qwen", "claude"], default="claude",
                        help="LLM backend (default: claude)")
    parser.add_argument("--max-assert-iter", type=int, default=4,
                        help="Max assertion-fix iterations (default: 4)")
    parser.add_argument("--max-cov-iter",    type=int, default=3,
                        help="Max coverage-relaxation iterations (default: 3)")
    parser.add_argument("--cov-threshold",   type=float, default=0.80,
                        help="GT coverage ratio threshold to stop (default: 0.80)")
    parser.add_argument("--cov-only",  action="store_true",
                        help="Skip Phase 1 — reuse existing PASS harness, only run coverage phase")
    args = parser.parse_args()

    _load_model_backend(args.model)

    funcs = PILOT_FUNCTIONS if args.all else []
    if args.func:
        funcs = [(d, n) for d, n in PILOT_FUNCTIONS if n == args.func]
        if not funcs:
            funcs = [(f"func__{args.func}", args.func)]

    if not funcs:
        parser.print_help()
        sys.exit(1)

    all_results = []
    for func_dir, func_name in funcs:
        # Resume support: skip if summary already exists
        output_dir   = RESULTS_DIR / f"coverage_loop_{args.condition}_{args.model}" / func_name
        summary_path = output_dir / "summary.json"
        if summary_path.exists() and not args.cov_only:
            saved = json.loads(summary_path.read_text())
            print(f"  SKIP (done): {func_name}  "
                  f"final={saved.get('final_verify')}  "
                  f"gt_ratio={saved.get('final_coverage_ratio')}")
            continue

        try:
            records = run_coverage_feedback_loop(
                func_dir, func_name,
                condition=args.condition,
                model=args.model,
                max_assert_iter=args.max_assert_iter,
                max_cov_iter=args.max_cov_iter,
                cov_threshold=args.cov_threshold,
            )
            all_results.append((func_name, records))
        except Exception as e:
            print(f"  ERROR {func_name}: {e}")
            import traceback; traceback.print_exc()

    # Print aggregate summary
    if len(all_results) > 1:
        print(f"\n{'='*70}")
        print(f"{'COVERAGE FEEDBACK LOOP SUMMARY':^70}")
        print(f"{'='*70}")
        print(f"{'Function':<35} {'Final':^8} {'A_it':^5} {'C_it':^5} {'GT_ratio':^9}")
        print(f"{'-'*70}")
        for fn, recs in all_results:
            final = recs[-1].cbmc_result.verification_result if recs else "N/A"
            a_it  = sum(1 for r in recs if r.phase == "assert")
            c_it  = sum(1 for r in recs if r.phase == "coverage")
            ratio = next((r.coverage_ratio for r in reversed(recs)
                          if r.coverage_ratio is not None), None)
            ratio_s = f"{ratio:.1%}" if ratio is not None else "N/A"
            print(f"{fn:<35} {final:^8} {a_it:^5} {c_it:^5} {ratio_s:^9}")
        print(f"{'='*70}")


if __name__ == "__main__":
    main()
