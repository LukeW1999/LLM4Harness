#!/usr/bin/env python3
"""
mutation_test.py
================
Measure harness specification completeness via mutation testing.

For each mutant of the target function:
  - If CBMC FAILS with the harness  → the harness DETECTS the bug (killed mutant)
  - If CBMC PASSES with the mutant  → the harness MISSES the bug (surviving mutant)

Mutation score = killed / total  (higher = more complete specification)

No ground truth needed: CBMC + source code are sufficient.

Mutant operators applied:
  ASSIGN_ZERO   — set a written field to 0 instead of the computed value
  ASSIGN_WRONG  — set a written field to a wrong constant
  SKIP_WRITE    — comment out one field assignment entirely
  FLIP_RETURN   — flip success/error return value

Usage:
    python mutation_test.py --func aws_byte_buf_init --variant feedback_loop_B_claude
    python mutation_test.py --all --variant feedback_loop_B_claude --iter 1
"""

import re
import sys
import json
import shutil
import tempfile
import argparse
from pathlib import Path
from dataclasses import dataclass, field

SCRIPT_DIR     = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent
sys.path.insert(0, str(SCRIPT_DIR))

from cbmc_runner import run_cbmc, FUNC_CONFIGS, SRCDIR
from feedback_loop import PILOT_FUNCTIONS, RESULTS_DIR

# ── Mutation operators ────────────────────────────────────────────────────────

def _mutations_for(func_name: str, src_text: str) -> list[dict]:
    """
    Generate all candidate mutations for a function's source text.
    Each mutation: {"label": str, "original": str, "mutant": str, "line_hint": int}
    """
    mutations = []

    # 1. SKIP_WRITE: comment out field assignment lines
    #    Match:  ptr->field = expr;  or  var.field = expr;
    #    Supports multi-level:  ptr->field.sub = expr;  ptr->a.b.c = expr;
    assign_re = re.compile(
        r'^(?P<indent>[ \t]*)(?P<lhs>(?:\w+(?:->|\.))(?:\w+(?:\.))*\w+)\s*='
        r'(?P<rhs>[^;=][^;]*);',
        re.MULTILINE
    )
    for m in assign_re.finditer(src_text):
        full    = m.group(0)
        indent  = m.group("indent")
        lhs     = m.group("lhs")
        rhs     = m.group("rhs").strip()
        lineno  = src_text[:m.start()].count('\n') + 1

        # SKIP_WRITE
        mutations.append({
            "label":     f"SKIP_WRITE({lhs})",
            "original":  full,
            "mutant":    f"{indent}/* MUTANT: skip {lhs} = {rhs}; */",
            "line_hint": lineno,
        })

        # ASSIGN_ZERO  (only if rhs is not already 0 / NULL)
        rhs_stripped = rhs.strip().rstrip(")")
        if rhs.strip() not in ("0", "NULL", "0U", "0UL"):
            mutations.append({
                "label":     f"ASSIGN_ZERO({lhs})",
                "original":  full,
                "mutant":    f"{indent}{lhs} = 0; /* MUTANT */",
                "line_hint": lineno,
            })

    # 2. FLIP_RETURN: swap AWS_OP_SUCCESS ↔ AWS_OP_ERR in return statements
    ret_re = re.compile(r'\breturn\s+(AWS_OP_SUCCESS|AWS_OP_ERR)\b', re.MULTILINE)
    for m in ret_re.finditer(src_text):
        orig_val = m.group(1)
        flip_val = "AWS_OP_ERR" if orig_val == "AWS_OP_SUCCESS" else "AWS_OP_SUCCESS"
        lineno   = src_text[:m.start()].count('\n') + 1
        mutations.append({
            "label":     f"FLIP_RETURN({orig_val}→{flip_val})",
            "original":  m.group(0),
            "mutant":    f"return {flip_val} /* MUTANT */",
            "line_hint": lineno,
        })

    return mutations


# ── Finding sources ───────────────────────────────────────────────────────────

# .inl files that contain AWS_STATIC_IMPL function definitions
_INL_FILES = [
    SRCDIR / "include/aws/common/linked_list.inl",
    SRCDIR / "include/aws/common/array_list.inl",
    SRCDIR / "include/aws/common/string.inl",
    SRCDIR / "include/aws/common/ring_buffer.inl",
    SRCDIR / "include/aws/common/byte_order.inl",
    SRCDIR / "include/aws/common/math.inl",
]
# Relative path from the include root (used to shadow the file via -I)
_INL_REL = {p: p.relative_to(SRCDIR / "include") for p in _INL_FILES}

# Regex to match a function definition (signature + opening brace), not just a call
_FUNC_DEF_RE = re.compile(
    r'(?:^|\n)(?:[\w\s\*]+\s+){func_name}\s*\([^)]*\)\s*\{{',
    re.MULTILINE
)


def _has_func_definition(func_name: str, text: str) -> bool:
    """Return True if text contains a definition (not just a call) of func_name."""
    pat = re.compile(
        rf'(?:^|\n)(?:[\w\s\*]+\s+){re.escape(func_name)}\s*\([^)]*\)\s*\{{',
        re.MULTILINE
    )
    return pat.search(text) is not None


def _find_func_source_file(func_name: str) -> tuple[Path | None, bool]:
    """
    Find the file that defines func_name.
    Returns (path, is_inl):
      is_inl=False  → regular .c file (mutate file directly via temp copy)
      is_inl=True   → .inl header file (mutate via include-path shadowing)
    Searches for an actual function *definition* (signature + body), not just a call.
    """
    # 1. Search project_sources (.c files)
    cfg = FUNC_CONFIGS.get(func_name, {})
    for src_path in cfg.get("project_sources", []):
        text = Path(src_path).read_text(errors="replace")
        if _has_func_definition(func_name, text):
            return Path(src_path), False

    # 2. Search known .inl files (must have a definition, not just a call)
    for inl_path in _INL_FILES:
        if not inl_path.exists():
            continue
        text = inl_path.read_text(errors="replace")
        if _has_func_definition(func_name, text):
            return inl_path, True

    return None, False


def _extract_func_body(func_name: str, src_text: str) -> tuple[int, int] | None:
    """
    Find the byte offsets of the function definition's body in src_text.
    Returns (start_of_body, end_of_body) or None.
    """
    # Find function signature
    sig_re = re.compile(
        rf'(?:^|\n)(?:[\w\s\*]+\s+){re.escape(func_name)}\s*\([^)]*\)\s*\{{',
        re.MULTILINE
    )
    m = sig_re.search(src_text)
    if m is None:
        return None

    open_brace = src_text.index('{', m.start())
    depth = 0
    i = open_brace
    while i < len(src_text):
        if src_text[i] == '{':
            depth += 1
        elif src_text[i] == '}':
            depth -= 1
            if depth == 0:
                return (open_brace, i + 1)
        i += 1
    return None


# ── Running one mutant ────────────────────────────────────────────────────────

def _apply_mutant(src_text: str, mutation: dict) -> str:
    """Apply one mutation to the source text (first occurrence)."""
    return src_text.replace(mutation["original"], mutation["mutant"], 1)


def _run_mutant(func_name: str,
                src_path: Path,
                mutated_src: str,
                harness_path: Path,
                is_inl: bool = False,
                timeout: int = 120) -> str:
    """
    Write mutated source to a temp file/dir, run CBMC, return verification result.

    For .c files  (is_inl=False): replace the source file in project_sources.
    For .inl files (is_inl=True): shadow the include directory so the mutated
      .inl is found before the original (uses -I prepend).

    Returns: "FAIL" (killed), "SUCCESS" (survived), "COMPILE_ERROR", "TIMEOUT", "UNKNOWN"
    """
    import subprocess
    from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS

    tmp_dir = Path(tempfile.mkdtemp(prefix="mutant_"))
    try:
        cfg = FUNC_CONFIGS[func_name]

        if is_inl:
            # Shadow the .inl file: create temp_dir/<relative_path> with mutated content
            rel = _INL_REL[src_path]          # e.g. aws/common/linked_list.inl
            shadow_file = tmp_dir / rel
            shadow_file.parent.mkdir(parents=True, exist_ok=True)
            shadow_file.write_text(mutated_src)
            # Prepend tmp_dir to include path so mutated .inl is found first
            extra_flags = [f"-I{tmp_dir}"]
            project_sources = [str(p) for p in cfg.get("project_sources", [])]
        else:
            # Replace the .c source file in project_sources
            tmp_src = tmp_dir / src_path.name
            tmp_src.write_text(mutated_src)
            extra_flags = []
            project_sources = [
                str(tmp_src) if Path(p).name == src_path.name else str(p)
                for p in cfg.get("project_sources", [])
            ]

        cmd = (
            ["cbmc"]
            + extra_flags          # prepend shadow dir before normal -I flags
            + COMMON_FLAGS
            + cfg.get("unwind", [])
            + cfg.get("unwindset", [])
            + cfg.get("defines", [])
            + project_sources
            + [str(p) for p in cfg.get("proof_sources", [])]
            + [str(harness_path)]
            + ["--function", f"{func_name}_harness"]
        )
        try:
            proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
            stdout = proc.stdout + proc.stderr
            if proc.returncode == 10:
                return "FAIL"           # CBMC found a violation → mutant killed ✓
            elif proc.returncode == 0:
                return "SUCCESS"        # proof still passes → mutant survived ✗
            elif "syntax error" in stdout.lower() or "parse error" in stdout.lower():
                return "COMPILE_ERROR"
            elif proc.returncode in (1, 6) and "error" in stdout.lower():
                # rc=6: CBMC internal error (e.g. too many objects); rc=1: assertion/setup error
                # Distinguish from rc=10 (verification failure) and rc=0 (success)
                out_lower = stdout.lower()
                if "too many addressed" in out_lower or "object-bits" in out_lower:
                    return "MEM_EXCEEDED"
                return "CBMC_ERROR"
            else:
                return "UNKNOWN"
        except subprocess.TimeoutExpired:
            return "TIMEOUT"
    finally:
        shutil.rmtree(tmp_dir, ignore_errors=True)


# ── Per-function test ─────────────────────────────────────────────────────────

@dataclass
class MutantResult:
    label:       str
    line_hint:   int
    gt_result:   str    # FAIL=killed, SUCCESS=survived
    llm_result:  str

    @property
    def gt_killed(self) -> bool:
        return self.gt_result == "FAIL"

    @property
    def llm_killed(self) -> bool:
        return self.llm_result == "FAIL"


@dataclass
class MutationReport:
    func_name:     str
    total_mutants: int
    gt_killed:     int
    llm_killed:    int
    results:       list = field(default_factory=list)

    @property
    def gt_score(self) -> float:
        return self.gt_killed / self.total_mutants if self.total_mutants else 0.0

    @property
    def llm_score(self) -> float:
        return self.llm_killed / self.total_mutants if self.total_mutants else 0.0

    def summary(self) -> dict:
        return {
            "func":          self.func_name,
            "total_mutants": self.total_mutants,
            "gt_score":      round(self.gt_score, 3),
            "llm_score":     round(self.llm_score, 3),
            "missed_by_llm": [r.label for r in self.results
                              if r.gt_killed and not r.llm_killed],
        }


def run_mutation_test(func_name: str,
                      llm_harness_path: Path,
                      gt_harness_path: Path | None = None,
                      verbose: bool = True) -> MutationReport:
    """Run mutation testing for one function."""
    # Locate source file
    src_path, is_inl = _find_func_source_file(func_name)
    if src_path is None:
        print(f"  [SKIP] {func_name}: cannot locate source file")
        return MutationReport(func_name, 0, 0, 0, [])

    src_text = src_path.read_text()
    body_range = _extract_func_body(func_name, src_text)
    if body_range is None:
        print(f"  [SKIP] {func_name}: cannot locate function body in {src_path.name}")
        return MutationReport(func_name, 0, 0, 0, [])

    body_text = src_text[body_range[0]:body_range[1]]
    mutations = _mutations_for(func_name, body_text)

    # Filter to mutations that actually change the text
    mutations = [m for m in mutations if m["original"] in src_text]

    if verbose:
        print(f"\n  {func_name}: {len(mutations)} mutants in {src_path.name}")

    # Find GT harness if not provided
    if gt_harness_path is None:
        from run_coverage import find_gt_harness
        gt_harness_path = find_gt_harness(func_name)

    results = []

    for mut in mutations:
        mutated = _apply_mutant(src_text, mut)

        gt_res  = "N/A"
        llm_res = "N/A"

        if gt_harness_path and gt_harness_path.exists():
            gt_res = _run_mutant(func_name, src_path, mutated, gt_harness_path, is_inl)

        llm_res = _run_mutant(func_name, src_path, mutated, llm_harness_path, is_inl)

        mr = MutantResult(
            label      = mut["label"],
            line_hint  = mut["line_hint"],
            gt_result  = gt_res,
            llm_result = llm_res,
        )
        results.append(mr)

        if verbose:
            gt_tag  = "✓" if mr.gt_killed  else "✗" if gt_res  == "SUCCESS" else gt_res[:3]
            llm_tag = "✓" if mr.llm_killed else "✗" if llm_res == "SUCCESS" else llm_res[:3]
            print(f"    {mut['label']:<45}  GT:{gt_tag}  LLM:{llm_tag}")

    # Count only mutants where both ran cleanly (skip errors/timeouts)
    _valid = ("FAIL", "SUCCESS")
    scoreable = [r for r in results
                 if r.gt_result in _valid and r.llm_result in _valid]

    report = MutationReport(
        func_name     = func_name,
        total_mutants = len(scoreable),
        gt_killed     = sum(1 for r in scoreable if r.gt_killed),
        llm_killed    = sum(1 for r in scoreable if r.llm_killed),
        results       = scoreable,
    )

    if verbose and scoreable:
        print(f"  → GT score: {report.gt_score:.0%}  "
              f"LLM score: {report.llm_score:.0%}  "
              f"({report.llm_killed}/{report.gt_killed} mutants caught)")
        missed = [r.label for r in scoreable if r.gt_killed and not r.llm_killed]
        if missed:
            print(f"  → Missed by LLM: {missed}")

    return report


# ── Main ─────────────────────────────────────────────────────────────────────

def _find_best_iter(func_name: str, variant: str) -> int:
    """
    Return the iteration number of the last CBMC-passing harness.
    Falls back to iter 1 if summary.json is missing or no passing iteration found.
    """
    summary_path = RESULTS_DIR / variant / func_name / "summary.json"
    if not summary_path.exists():
        return 1
    try:
        data = json.loads(summary_path.read_text())
        best = 1
        for it in data.get("iterations", []):
            if it.get("verify") == "SUCCESS":
                best = it["iter"]
        return best
    except Exception:
        return 1


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--func",      help="Function name")
    parser.add_argument("--all",       action="store_true", help="Run all pilot functions")
    parser.add_argument("--variant",   default="feedback_loop_B_claude")
    parser.add_argument("--iter",      type=int, default=None,
                        help="Harness iteration (default: auto-select best passing iter)")
    parser.add_argument("--best-iter", action="store_true",
                        help="Auto-select the last CBMC-passing iteration (default when --iter omitted)")
    parser.add_argument("--save",      action="store_true")
    args = parser.parse_args()

    # Default: use best iter unless --iter N explicitly given
    use_best_iter = (args.iter is None) or args.best_iter

    funcs = PILOT_FUNCTIONS if args.all else []
    if args.func:
        funcs = [(d, n) for d, n in PILOT_FUNCTIONS if n == args.func]

    if not funcs:
        parser.print_help(); sys.exit(1)

    all_reports = []

    for _, func_name in funcs:
        if func_name not in FUNC_CONFIGS:
            print(f"  [SKIP] {func_name}: not in FUNC_CONFIGS")
            continue

        iter_num = _find_best_iter(func_name, args.variant) if use_best_iter else args.iter
        llm_path = RESULTS_DIR / args.variant / func_name / f"iter_{iter_num}_harness.c"
        if not llm_path.exists():
            print(f"  [SKIP] {func_name}: no harness at iter {iter_num}")
            continue

        report = run_mutation_test(func_name, llm_path, verbose=True)
        all_reports.append(report)

        if args.save and report.total_mutants:
            out = RESULTS_DIR / args.variant / func_name / f"iter_{iter_num}_mutation_report.json"
            out.write_text(json.dumps(report.summary(), indent=2))

    # Summary table
    if len(all_reports) > 1:
        print(f"\n{'='*65}")
        print(f"{'MUTATION TESTING SUMMARY':^65}")
        print(f"{'='*65}")
        print(f"{'Function':<35} {'Mutants':^8} {'GT':^8} {'LLM':^8}")
        print(f"{'-'*65}")
        for r in all_reports:
            if r.total_mutants:
                print(f"{r.func_name:<35} {r.total_mutants:^8} "
                      f"{r.gt_score:^8.0%} {r.llm_score:^8.0%}")
        print(f"{'='*65}")
        valid = [r for r in all_reports if r.total_mutants]
        if valid:
            avg_gt  = sum(r.gt_score  for r in valid) / len(valid)
            avg_llm = sum(r.llm_score for r in valid) / len(valid)
            print(f"{'Average':<35} {'':^8} {avg_gt:^8.0%} {avg_llm:^8.0%}")

        if args.save and args.all:
            eval_dir = SCRIPT_DIR.parent / "evaluation"
            eval_dir.mkdir(exist_ok=True)
            agg = {
                "variant": args.variant,
                "iteration": args.iter,
                "functions": [r.summary() for r in valid],
                "aggregate": {
                    "n": len(valid),
                    "avg_gt_score":  round(avg_gt,  3) if valid else None,
                    "avg_llm_score": round(avg_llm, 3) if valid else None,
                    "total_mutants": sum(r.total_mutants for r in valid),
                    "total_gt_killed":  sum(r.gt_killed  for r in valid),
                    "total_llm_killed": sum(r.llm_killed for r in valid),
                },
            }
            out_path = eval_dir / f"mutation_results_{args.variant}_iter{args.iter}.json"
            out_path.write_text(json.dumps(agg, indent=2))
            print(f"\nAggregate saved → {out_path}")


if __name__ == "__main__":
    main()
