#!/usr/bin/env python3
"""
patch_llm_harnesses.py — Fix systematic compile errors in LLM-generated harnesses
for the 18 functions with 100% COMPILE_ERROR in the CBMC mutation oracle.

Observed error patterns from manual inspection of 4 harnesses:
  1. Fake header: #include "cbmc_proof.h"
  2. Non-existent AWS header: #include <aws/common/array.h>
  3. Missing #define MAX_BUFFER_SIZE
  4. Wrong nondet names: nondet_char(), nondet_uintptr(), nondet_uint64()
  5. Conflicting forward declarations of CBMC nondet stubs

Usage:
    # Diagnose: show CBMC errors for compile-error functions from oracle JSON
    python3 patch_llm_harnesses.py --dataset feedback_loop_A_gptoss120b --diagnose

    # Dry run (show what would be patched):
    python3 patch_llm_harnesses.py --dataset feedback_loop_A_gptoss120b --dry-run

    # Apply patches:
    python3 patch_llm_harnesses.py --dataset feedback_loop_A_gptoss120b

    # Single function:
    python3 patch_llm_harnesses.py --dataset feedback_loop_A_gptoss120b --func aws_ring_buffer_init
"""

import re, argparse, shutil, subprocess, json
from pathlib import Path

RESULTS_DIR = Path("/root/experiment_aws_cbmc/results")

# Headers that don't exist — map fake_pattern → replacement_line (or None to just remove)
FAKE_HEADERS: list[tuple[str, str | None]] = [
    # cbmc_proof.h is a hallucinated header; replace with the real proof helpers include
    (r'#include\s+"cbmc_proof\.h"',
     '#include "proof_helpers/make_common_data_structures.h"'),
    # harness_helpers.h is another common hallucination
    (r'#include\s+"harness_helpers\.h"',
     '#include "proof_helpers/make_common_data_structures.h"'),
    # cbmc_helpers.h — same pattern
    (r'#include\s+"cbmc_helpers\.h"',
     '#include "proof_helpers/make_common_data_structures.h"'),
    # aws/common/array.h doesn't exist (aws/common/array_list.h does, but likely already included)
    (r'#include\s+<aws/common/array\.h>', None),
    # aws/common/memory.h doesn't exist; common.h covers allocation
    (r'#include\s+<aws/common/memory\.h>', None),
]

# Nondet function name substitutions (wrong → correct CBMC names)
NONDET_SUBS = [
    (r'\bnondet_char\s*\(\)', 'nondet_uint8_t()'),
    (r'\bnondet_uint64\s*\(\)', 'nondet_uint64_t()'),
    (r'\bnondet_uint32\s*\(\)', 'nondet_uint32_t()'),
    (r'\bnondet_uint16\s*\(\)', 'nondet_uint16_t()'),
    (r'\bnondet_uint8\s*\(\)', 'nondet_uint8_t()'),
    (r'\bnondet_int64\s*\(\)', 'nondet_int64_t()'),
    (r'\bnondet_uintptr\s*\(\)', '(uintptr_t)nondet_size_t()'),
    (r'\bnondet_intptr\s*\(\)', '(intptr_t)nondet_size_t()'),
    # nondet_ptr() is not a CBMC primitive; use malloc + assume
    (r'\bnondet_ptr\s*\(\)', 'malloc(sizeof(void *))'),
]

# Forward declarations that conflict with CBMC builtins — remove these lines
CONFLICTING_DECLS = [
    r'^\s*size_t\s+nondet_size_t\s*\(void\)\s*;\s*$',
    r'^\s*bool\s+nondet_bool\s*\(void\)\s*;\s*$',
    r'^\s*uint8_t\s+nondet_uint8_t\s*\(void\)\s*;\s*$',
    r'^\s*uint64_t\s+nondet_uint64_t\s*\(void\)\s*;\s*$',
    r'^\s*int\s+nondet_int\s*\(void\)\s*;\s*$',
    r'^\s*unsigned\s+int\s+nondet_uint\s*\(void\)\s*;\s*$',
]

# Patterns for duplicate/conflicting function forward declarations
# (functions that already exist in included headers)
CONFLICTING_FN_DECLS = [
    # aws_array_eq_c_str is in byte_buf.h, don't re-declare
    r'^\s*bool\s+aws_array_eq_c_str\s*\(',
]

MACRO_GUARD = "#define MAX_BUFFER_SIZE 256\n"
MACRO_PATTERN = r'#define\s+MAX_BUFFER_SIZE\b'


def patch_content(src: str, func_name: str) -> tuple[str, list[str]]:
    """Apply all patches to harness source. Returns (patched_src, list_of_changes)."""
    changes = []
    lines = src.splitlines(keepends=True)
    out_lines = []

    for line in lines:
        original = line

        # 1. Replace/remove fake headers
        removed = False
        for (pat, replacement) in FAKE_HEADERS:
            if re.search(pat, line):
                if replacement:
                    # Only inject replacement if not already present in file
                    if replacement not in src:
                        changes.append(f"REPLACED fake header: {line.strip()} → {replacement}")
                        out_lines.append(replacement + "\n")
                    else:
                        changes.append(f"REMOVED duplicate fake header: {line.strip()}")
                else:
                    changes.append(f"REMOVED fake header: {line.strip()}")
                removed = True
                break
        if removed:
            continue

        # 2. Remove conflicting forward declarations of CBMC nondet stubs
        for pat in CONFLICTING_DECLS:
            if re.match(pat, line, re.MULTILINE):
                changes.append(f"REMOVED conflicting decl: {line.strip()}")
                removed = True
                break
        if removed:
            continue

        # 3. Remove conflicting forward declarations of library functions
        for pat in CONFLICTING_FN_DECLS:
            if re.search(pat, line):
                changes.append(f"REMOVED conflicting fn decl: {line.strip()}")
                removed = True
                break
        if removed:
            continue

        # 4. Fix nondet function names
        for (wrong_pat, correct) in NONDET_SUBS:
            if re.search(wrong_pat, line):
                new_line = re.sub(wrong_pat, correct, line)
                changes.append(f"FIXED nondet name: {line.strip()} → {new_line.strip()}")
                line = new_line

        out_lines.append(line)

    patched = "".join(out_lines)

    # 5. Inject MAX_BUFFER_SIZE if used but not defined
    if 'MAX_BUFFER_SIZE' in patched and not re.search(MACRO_PATTERN, patched):
        # Inject after the last #include line
        lines2 = patched.splitlines(keepends=True)
        last_include_idx = -1
        for i, l in enumerate(lines2):
            if l.startswith('#include'):
                last_include_idx = i
        if last_include_idx >= 0:
            lines2.insert(last_include_idx + 1, MACRO_GUARD)
            patched = "".join(lines2)
            changes.append("ADDED #define MAX_BUFFER_SIZE 256")

    return patched, changes


def get_harness_path(results_dir: Path, dataset: str, func: str) -> Path | None:
    func_dir = results_dir / dataset / func
    # Use iter_3 (final iteration) if it exists, else highest iter
    for i in range(10, -1, -1):
        p = func_dir / f"iter_{i}_harness.c"
        if p.exists():
            return p
    return None


def patch_func(results_dir: Path, dataset: str, func: str, dry_run: bool) -> bool:
    harness_path = get_harness_path(results_dir, dataset, func)
    if harness_path is None:
        print(f"  [{func}] No harness found")
        return False

    src = harness_path.read_text()
    patched, changes = patch_content(src, func)

    if not changes:
        print(f"  [{func}] No patches needed (different error type?)")
        return False

    print(f"  [{func}] {len(changes)} fix(es):")
    for c in changes:
        print(f"    • {c}")

    if not dry_run:
        # Backup original
        backup = harness_path.with_suffix(".c.orig")
        if not backup.exists():
            shutil.copy(harness_path, backup)
        harness_path.write_text(patched)
        print(f"    → Written to {harness_path}")

    return True


EVAL_DIR = Path("/root/experiment_aws_cbmc/evaluation")
SRCDIR   = Path("/root/aws-c-common")
PROOFDIR = SRCDIR / "verification/cbmc"


def get_compile_error_funcs(dataset: str) -> list[str]:
    """Read oracle JSON and return functions where ALL LLM results are COMPILE_ERROR."""
    oracle = EVAL_DIR / f"mutation_oracle_cbmc_{dataset}.json"
    if not oracle.exists():
        print(f"Oracle not found: {oracle}")
        return []
    data = json.loads(oracle.read_text())
    from collections import defaultdict
    stats = defaultdict(lambda: {"total": 0, "ce": 0})
    for r in data.get("results", []):
        fn = r["func"]
        stats[fn]["total"] += 1
        if r.get("llm") == "COMPILE_ERROR":
            stats[fn]["ce"] += 1
    broken = sorted(fn for fn, s in stats.items()
                    if s["total"] > 0 and s["ce"] == s["total"])
    return broken


def diagnose_func(dataset: str, func: str):
    """Run CBMC on the harness and show first error."""
    import sys
    sys.path.insert(0, str(Path("/root/experiment_aws_cbmc/scripts")))
    try:
        from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS
    except ImportError:
        print(f"  [{func}] Cannot import cbmc_runner (run on server)")
        return

    cfg = FUNC_CONFIGS.get(func)
    if cfg is None:
        print(f"  [{func}] NOT in FUNC_CONFIGS — that itself causes COMPILE_ERROR")
        return

    harness_path = get_harness_path(RESULTS_DIR, dataset, func)
    if harness_path is None:
        print(f"  [{func}] No harness file found")
        return

    proof_sources   = [str(p) for p in cfg.get("proof_sources", [])]
    project_sources = [str(p) for p in cfg.get("project_sources", [])]
    obj_bits        = cfg.get("object_bits", 8)
    base_flags      = list(COMMON_FLAGS)
    defines         = list(cfg.get("defines", []))

    cmd = (
        ["cbmc"] + base_flags + defines +
        list(cfg.get("unwind", [])) +
        ["--function", f"{func}_harness"] +
        proof_sources + [str(harness_path)] + project_sources
    )

    proc = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
    combined = proc.stdout + proc.stderr
    # Extract first error lines
    err_lines = [l for l in combined.splitlines()
                 if any(k in l for k in ("error:", "fatal error:", "PARSING ERROR", "warning:"))]
    print(f"  [{func}] RC={proc.returncode}")
    for l in err_lines[:6]:
        print(f"    {l.strip()}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--dataset", default="feedback_loop_A_gptoss120b")
    parser.add_argument("--func", help="Single function to patch")
    parser.add_argument("--dry-run", action="store_true")
    parser.add_argument("--diagnose", action="store_true",
                        help="Run CBMC and show compile errors for each CE function")
    parser.add_argument("--funcs-file", help="File with one function name per line")
    args = parser.parse_args()

    results_dir = RESULTS_DIR

    if args.func:
        funcs = [args.func]
    elif args.funcs_file:
        funcs = Path(args.funcs_file).read_text().strip().splitlines()
    elif args.diagnose:
        funcs = get_compile_error_funcs(args.dataset)
        print(f"Found {len(funcs)} functions with 100% COMPILE_ERROR:")
        for f in funcs: print(f"  {f}")
    else:
        # Auto-detect: all function dirs in dataset
        dataset_dir = results_dir / args.dataset
        funcs = sorted(d.name for d in dataset_dir.iterdir() if d.is_dir())

    if args.diagnose:
        print("\n=== Diagnosing compile errors ===")
        for func in funcs:
            diagnose_func(args.dataset, func)
        return

    mode = "DRY RUN" if args.dry_run else "APPLYING"
    print(f"=== Harness patcher [{mode}] dataset={args.dataset} ===")
    fixed = 0
    for func in funcs:
        if patch_func(results_dir, args.dataset, func, args.dry_run):
            fixed += 1
    print(f"\nPatched {fixed}/{len(funcs)} functions")


if __name__ == "__main__":
    main()
