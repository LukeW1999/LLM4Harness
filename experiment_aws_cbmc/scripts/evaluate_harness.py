#!/usr/bin/env python3
"""
evaluate_harness.py - Compare LLM-generated CBMC harness against ground truth.

Extracts assert() and __CPROVER_assume() statements from both harnesses,
then computes:
  - Overlap: conditions present in both
  - Missing: in ground truth but not in LLM output
  - Extra: in LLM output but not in ground truth
  - Compilability: whether the LLM harness has balanced braces (basic check)

Usage:
    python evaluate_harness.py --func <func_dir> --prompt <A|B|C>
    python evaluate_harness.py --all --prompt B
    python evaluate_harness.py --all --all-prompts   # compare all prompts
"""

import re
import sys
import json
import argparse
from pathlib import Path
from dataclasses import dataclass, field
from typing import List, Set

script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
DATASET_DIR = experiment_dir / "dataset"
RESULTS_DIR = experiment_dir / "results"
EVAL_DIR = experiment_dir / "evaluation"

PILOT_FUNCTIONS = [
    "func1_aws_add_size_checked",
    "func2_aws_byte_buf_init",
    "func3_aws_array_list_back",
    "func4_aws_byte_buf_append",
    "func5_aws_linked_list_push_back",
]


def get_function_name(func_dir: str) -> str:
    parts = func_dir.split("_", 1)
    return parts[1] if len(parts) == 2 else func_dir


def semantic_normalize(cond: str) -> str:
    """
    Normalize a condition expression for semantic comparison.
    Handles:
    1. Variable name aliasing for old-state variables (to_old/old_to, etc.)
    2. Equality direction (a==b → canonical sorted form)
    3. Pointer vs value access (list-> vs list.)
    4. Whitespace normalization
    """
    # Collapse whitespace
    s = re.sub(r'\s+', ' ', cond).strip()

    # Normalize old-state variable naming conventions:
    # "to_old" and "old_to" are both "old state of to"
    # Pattern: {name}_old or old_{name} → OLD_{name}
    s = re.sub(r'\bold_(\w+)\b', r'OLD_\1', s)
    s = re.sub(r'\b(\w+)_old\b', r'OLD_\1', s)

    # Similarly for from_old/old_from etc.
    # Also: "old_last" → could be "OLD_last", "to_add" might equal "node"
    # We can't rename arbitrary variables, but we can normalize access patterns

    # Normalize pointer vs value access: list-> → list. for struct members
    # (In harnesses, list->x and list.x are semantically equivalent depending
    # on whether list is a pointer or value — normalize to list.x)
    s = re.sub(r'(\w+)->', r'\1.', s)

    # Normalize equality direction: sort both sides of == for canonical form
    # Simple case: "a == b" → sorted("a", "b") joined by " == "
    eq_match = re.match(r'^(.+?)\s*==\s*(.+)$', s)
    if eq_match:
        lhs = eq_match.group(1).strip()
        rhs = eq_match.group(2).strip()
        # Canonical form: lexicographically smaller side first
        if lhs > rhs:
            s = f"{rhs} == {lhs}"

    return s


def extract_statements(code: str, kind: str) -> Set[str]:
    """
    Extract and normalize assert() or __CPROVER_assume() statements from C code.
    kind: 'assert' | 'assume'
    Returns a set of normalized condition strings (both raw and semantic).
    """
    if kind == "assert":
        pattern = r'\bassert\s*\((.+?)\)\s*;'
    else:  # assume
        pattern = r'__CPROVER_assume\s*\((.+?)\)\s*;'

    # Remove single-line comments
    code_no_comments = re.sub(r'//[^\n]*', '', code)
    # Remove multi-line comments
    code_no_comments = re.sub(r'/\*.*?\*/', '', code_no_comments, flags=re.DOTALL)

    matches = re.findall(pattern, code_no_comments, re.DOTALL)
    # Normalize: collapse whitespace, strip
    normalized = set()
    for m in matches:
        norm = re.sub(r'\s+', ' ', m).strip()
        normalized.add(norm)
    return normalized


def extract_statements_semantic(code: str, kind: str) -> Set[str]:
    """Same as extract_statements but applies semantic normalization."""
    raw = extract_statements(code, kind)
    return {semantic_normalize(s) for s in raw}


def compute_semantic_overlap(set_a: Set[str], set_b: Set[str]) -> tuple[Set[str], Set[str], Set[str]]:
    """Compute overlap/missing/extra using semantic normalization."""
    norm_a = {semantic_normalize(s) for s in set_a}
    norm_b = {semantic_normalize(s) for s in set_b}
    overlap_norms = norm_a & norm_b
    # Map back to original strings for readability (use GT string for overlap)
    overlap_gt = {s for s in set_a if semantic_normalize(s) in overlap_norms}
    missing = {s for s in set_a if semantic_normalize(s) not in norm_b}
    extra = {s for s in set_b if semantic_normalize(s) not in norm_a}
    return overlap_gt, missing, extra


def check_syntax(code: str) -> dict:
    """Basic structural checks on generated C code."""
    open_braces = code.count('{')
    close_braces = code.count('}')
    has_harness_func = bool(re.search(r'void\s+\w+_harness\s*\(\s*\)', code))
    has_includes = bool(re.search(r'#include', code))

    issues = []
    if open_braces != close_braces:
        issues.append(f"Brace mismatch: {open_braces} {{ vs {close_braces} }}")
    if not has_harness_func:
        issues.append("Missing harness function (void XXX_harness())")
    if not has_includes:
        issues.append("No #include directives found")

    return {
        "balanced_braces": open_braces == close_braces,
        "has_harness_func": has_harness_func,
        "has_includes": has_includes,
        "issues": issues,
        "syntax_ok": len(issues) == 0,
    }


@dataclass
class EvalResult:
    func_dir: str
    prompt_type: str
    func_name: str

    # Assert analysis (syntactic)
    gt_asserts: Set[str] = field(default_factory=set)
    llm_asserts: Set[str] = field(default_factory=set)
    assert_overlap: Set[str] = field(default_factory=set)
    assert_missing: Set[str] = field(default_factory=set)
    assert_extra: Set[str] = field(default_factory=set)

    # Assert analysis (semantic)
    assert_sem_overlap: Set[str] = field(default_factory=set)
    assert_sem_missing: Set[str] = field(default_factory=set)
    assert_sem_extra: Set[str] = field(default_factory=set)

    # Assume analysis (syntactic)
    gt_assumes: Set[str] = field(default_factory=set)
    llm_assumes: Set[str] = field(default_factory=set)
    assume_overlap: Set[str] = field(default_factory=set)
    assume_missing: Set[str] = field(default_factory=set)
    assume_extra: Set[str] = field(default_factory=set)

    # Assume analysis (semantic)
    assume_sem_overlap: Set[str] = field(default_factory=set)
    assume_sem_missing: Set[str] = field(default_factory=set)
    assume_sem_extra: Set[str] = field(default_factory=set)

    # Syntax
    syntax: dict = field(default_factory=dict)

    def assert_recall(self) -> float:
        """Fraction of GT asserts captured by LLM (syntactic)."""
        if not self.gt_asserts:
            return 1.0
        return len(self.assert_overlap) / len(self.gt_asserts)

    def assert_sem_recall(self) -> float:
        """Fraction of GT asserts captured by LLM (semantic)."""
        if not self.gt_asserts:
            return 1.0
        return len(self.assert_sem_overlap) / len(self.gt_asserts)

    def assume_recall(self) -> float:
        """Fraction of GT assumes captured by LLM (syntactic)."""
        if not self.gt_assumes:
            return 1.0
        return len(self.assume_overlap) / len(self.gt_assumes)

    def assume_sem_recall(self) -> float:
        """Fraction of GT assumes captured by LLM (semantic)."""
        if not self.gt_assumes:
            return 1.0
        return len(self.assume_sem_overlap) / len(self.gt_assumes)

    def to_dict(self) -> dict:
        return {
            "func": self.func_name,
            "prompt": self.prompt_type,
            "syntax_ok": self.syntax.get("syntax_ok", False),
            "syntax_issues": self.syntax.get("issues", []),
            "assert_recall_syntactic": round(self.assert_recall(), 3),
            "assert_recall_semantic": round(self.assert_sem_recall(), 3),
            "assume_recall_syntactic": round(self.assume_recall(), 3),
            "assume_recall_semantic": round(self.assume_sem_recall(), 3),
            "assert_recall": round(self.assert_recall(), 3),
            "assume_recall": round(self.assume_recall(), 3),
            "gt_asserts": sorted(self.gt_asserts),
            "llm_asserts": sorted(self.llm_asserts),
            "assert_overlap": sorted(self.assert_overlap),
            "assert_missing": sorted(self.assert_missing),
            "assert_extra": sorted(self.assert_extra),
            "gt_assumes": sorted(self.gt_assumes),
            "llm_assumes": sorted(self.llm_assumes),
            "assume_overlap": sorted(self.assume_overlap),
            "assume_missing": sorted(self.assume_missing),
            "assume_extra": sorted(self.assume_extra),
        }


def evaluate_one(func_dir: str, prompt_type: str) -> EvalResult:
    """Evaluate one function/prompt combination."""
    func_name = get_function_name(func_dir)

    gt_path = DATASET_DIR / func_dir / "ground_truth_harness.c"
    llm_path = RESULTS_DIR / f"prompt_{prompt_type}" / func_dir / f"{func_name}_llm_harness.c"

    if not gt_path.exists():
        raise FileNotFoundError(f"Ground truth not found: {gt_path}")
    if not llm_path.exists():
        raise FileNotFoundError(f"LLM result not found: {llm_path}\n"
                                f"Run: python call_qwen_api.py --func {func_dir} --prompt {prompt_type}")

    gt_code = gt_path.read_text(encoding="utf-8")
    llm_code = llm_path.read_text(encoding="utf-8")

    result = EvalResult(func_dir=func_dir, prompt_type=prompt_type, func_name=func_name)

    result.gt_asserts = extract_statements(gt_code, "assert")
    result.llm_asserts = extract_statements(llm_code, "assert")
    # Syntactic overlap
    result.assert_overlap = result.gt_asserts & result.llm_asserts
    result.assert_missing = result.gt_asserts - result.llm_asserts
    result.assert_extra = result.llm_asserts - result.gt_asserts
    # Semantic overlap
    result.assert_sem_overlap, result.assert_sem_missing, result.assert_sem_extra = \
        compute_semantic_overlap(result.gt_asserts, result.llm_asserts)

    result.gt_assumes = extract_statements(gt_code, "assume")
    result.llm_assumes = extract_statements(llm_code, "assume")
    # Syntactic overlap
    result.assume_overlap = result.gt_assumes & result.llm_assumes
    result.assume_missing = result.gt_assumes - result.llm_assumes
    result.assume_extra = result.llm_assumes - result.gt_assumes
    # Semantic overlap
    result.assume_sem_overlap, result.assume_sem_missing, result.assume_sem_extra = \
        compute_semantic_overlap(result.gt_assumes, result.llm_assumes)

    result.syntax = check_syntax(llm_code)

    return result


def print_result(r: EvalResult):
    print(f"\n{'='*60}")
    print(f"Function: {r.func_name}  |  Prompt: {r.prompt_type}")
    print(f"{'='*60}")
    print(f"Syntax OK: {r.syntax.get('syntax_ok')}")
    if r.syntax.get("issues"):
        for issue in r.syntax["issues"]:
            print(f"  ! {issue}")

    print(f"\nAssert recall:  syntactic={r.assert_recall():.0%}  semantic={r.assert_sem_recall():.0%}  "
          f"({len(r.assert_overlap)}/{len(r.gt_asserts)} exact, "
          f"{len(r.assert_sem_overlap)}/{len(r.gt_asserts)} semantic)")
    if r.assert_sem_missing:
        print("  Semantically missing asserts (in GT, not in LLM):")
        for s in sorted(r.assert_sem_missing):
            print(f"    - {s}")
    if r.assert_sem_extra:
        print("  Semantically extra asserts (in LLM, not in GT):")
        for s in sorted(r.assert_sem_extra):
            print(f"    + {s}")

    print(f"\nAssume recall:  syntactic={r.assume_recall():.0%}  semantic={r.assume_sem_recall():.0%}  "
          f"({len(r.assume_overlap)}/{len(r.gt_assumes)} exact, "
          f"{len(r.assume_sem_overlap)}/{len(r.gt_assumes)} semantic)")
    if r.assume_sem_missing:
        print("  Semantically missing assumes (in GT, not in LLM):")
        for s in sorted(r.assume_sem_missing):
            print(f"    - {s}")
    if r.assume_sem_extra:
        print("  Semantically extra assumes (in LLM, not in GT):")
        for s in sorted(r.assume_sem_extra):
            print(f"    + {s}")


def print_summary_table(results: list):
    """Print a summary table of all results."""
    w = 90
    print(f"\n{'='*w}")
    print(f"{'SUMMARY TABLE (syntactic / semantic recall)':^{w}}")
    print(f"{'='*w}")
    print(f"{'Function':<40} {'Prompt':<7} {'Syntax':<7} {'Assert syn/sem':<17} {'Assume syn/sem':<17}")
    print(f"{'-'*w}")
    for r in results:
        syntax = "OK" if r.syntax.get("syntax_ok") else "FAIL"
        assert_str = f"{r.assert_recall():.0%} / {r.assert_sem_recall():.0%}"
        assume_str = f"{r.assume_recall():.0%} / {r.assume_sem_recall():.0%}"
        print(f"{r.func_name:<40} {'P'+r.prompt_type:<7} {syntax:<7} {assert_str:<17} {assume_str:<17}")
    print(f"{'='*w}")
    # Print averages
    if results:
        avg_asyn = sum(r.assert_recall() for r in results) / len(results)
        avg_asem = sum(r.assert_sem_recall() for r in results) / len(results)
        avg_usyn = sum(r.assume_recall() for r in results) / len(results)
        avg_usem = sum(r.assume_sem_recall() for r in results) / len(results)
        print(f"{'AVERAGE':<40} {'':<7} {'':<7} "
              f"{avg_asyn:.0%} / {avg_asem:.0%}    "
              f"{avg_usyn:.0%} / {avg_usem:.0%}")


def main():
    parser = argparse.ArgumentParser(description="Evaluate LLM-generated CBMC harnesses")
    parser.add_argument("--func", help="Function directory name")
    parser.add_argument("--prompt", choices=["A", "B", "C", "D"], default="B")
    parser.add_argument("--all", action="store_true", help="All pilot functions")
    parser.add_argument("--all-prompts", action="store_true", help="All 3 prompts")
    parser.add_argument("--save-json", action="store_true", help="Save results as JSON")
    args = parser.parse_args()

    results = []
    prompts = ["A", "B", "C", "D"] if args.all_prompts else [args.prompt]
    funcs = PILOT_FUNCTIONS if args.all else ([args.func] if args.func else None)

    if not funcs:
        parser.print_help()
        sys.exit(1)

    for func_dir in funcs:
        for prompt_type in prompts:
            try:
                r = evaluate_one(func_dir, prompt_type)
                print_result(r)
                results.append(r)
            except FileNotFoundError as e:
                print(f"\nSkipping {func_dir} [P{prompt_type}]: {e}")

    if len(results) > 1:
        print_summary_table(results)

    if args.save_json:
        EVAL_DIR.mkdir(parents=True, exist_ok=True)
        out_path = EVAL_DIR / f"eval_results_prompt_{args.prompt}.json"
        data = [r.to_dict() for r in results]
        out_path.write_text(json.dumps(data, indent=2), encoding="utf-8")
        print(f"\nSaved JSON to: {out_path}")


if __name__ == "__main__":
    main()
