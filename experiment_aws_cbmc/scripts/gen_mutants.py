#!/usr/bin/env python3
"""
gen_mutants.py — Phase 3 mutant generation using universalmutator.

For each function in FUNC_CONFIGS:
  1. Find the source file containing the function definition
  2. Extract line range of the function
  3. Run universalmutator --lines to generate mutants only for that function
  4. Apply compilation filter (gcc -fsyntax-only)
  5. Save valid mutants with metadata JSON

Output structure:
  mutants/<func_name>/
    ├── metadata.json         (source_file, line_range, n_raw, n_compiled, operator_dist)
    ├── mutant_0001.c         (compiled-valid mutant — full source file with mutation)
    ├── mutant_0002.c
    └── ...

Usage:
    python3 scripts/gen_mutants.py --func aws_byte_buf_init       # single function
    python3 scripts/gen_mutants.py --all                          # all functions
    python3 scripts/gen_mutants.py --trial --n 10                 # first 10 functions (trial)
    python3 scripts/gen_mutants.py --all --workers 4              # parallel
"""

import os
import re
import sys
import json
import shutil
import argparse
import subprocess
import tempfile
from pathlib import Path
from collections import Counter
from concurrent.futures import ProcessPoolExecutor, as_completed

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))

from cbmc_runner import FUNC_CONFIGS, SRCDIR

SRCDIR_AWS = Path("/home/weiqi/Verification/aws-c-common")
MUTANT_DIR = SCRIPT_DIR.parent / "mutants"

# goto-cc (CBMC's compiler frontend) compile check flags
GOTO_CC = "goto-cc"  # falls back to cbmc --show-goto-functions if not available

# CBMC compilation check: use cbmc to verify only compilation (no verification)
# This uses the same flags as cbmc_runner.py but stops after compilation
from cbmc_runner import COMMON_FLAGS, PROOFDIR


# ── Source file search ─────────────────────────────────────────────────────

# Manual overrides for known-hard-to-find functions
FUNC_SOURCE_OVERRIDES = {
    "aws_add_size_checked":     SRCDIR_AWS / "include/aws/common/math.h",
    "aws_add_size_saturating":  SRCDIR_AWS / "source/byte_buf.c",
    "aws_mul_size_checked":     SRCDIR_AWS / "include/aws/common/math.h",
    "aws_mul_size_saturating":  SRCDIR_AWS / "include/aws/common/math.h",
    "aws_is_power_of_two":      SRCDIR_AWS / "source/hash_table.c",
    "aws_round_up_to_power_of_two": SRCDIR_AWS / "source/allocator_sba.c",
    "aws_linked_list_init":     SRCDIR_AWS / "source/thread_scheduler.c",
    "aws_linked_list_push_back": SRCDIR_AWS / "source/task_scheduler.c",
    "aws_linked_list_push_front": SRCDIR_AWS / "source/thread_scheduler.c",
    "aws_linked_list_pop_back": SRCDIR_AWS / "source/thread_scheduler.c",
    "aws_linked_list_pop_front": SRCDIR_AWS / "source/thread_scheduler.c",
}


def find_func_definition(func_name: str, filepath: Path):
    """Find (start_line, end_line) of a function definition in a source file."""
    if not filepath.exists():
        return None, None
    text = filepath.read_text()
    lines = text.split("\n")
    # Regex: find line that starts a function definition (not indented for .c files)
    for i, line in enumerate(lines):
        if filepath.suffix in (".c", ".h", ".inl"):
            if not re.search(rf"\b{re.escape(func_name)}\s*\(", line):
                continue
            # Check that this looks like a definition (followed by { within 10 lines)
            context = "\n".join(lines[i : i + 10])
            if "{" not in context:
                continue
        # Find matching closing brace
        depth = 0
        in_func = False
        for j in range(i, len(lines)):
            depth += lines[j].count("{") - lines[j].count("}")
            if depth > 0:
                in_func = True
            if in_func and depth == 0:
                return i + 1, j + 1  # 1-indexed
    return None, None


def find_source_file_for_func(func_name: str):
    """Return (source_path, start_line, end_line) for a function."""
    # Try overrides first
    if func_name in FUNC_SOURCE_OVERRIDES:
        fpath = FUNC_SOURCE_OVERRIDES[func_name]
        start, end = find_func_definition(func_name, fpath)
        if start:
            return fpath, start, end

    # Try project_sources from FUNC_CONFIGS
    cfg = FUNC_CONFIGS.get(func_name, {})
    for src_path in cfg.get("project_sources", []):
        if not src_path.exists():
            continue
        start, end = find_func_definition(func_name, src_path)
        if start:
            return src_path, start, end

    # Broad search in aws-c-common source/include
    for search_dir in [SRCDIR_AWS / "source", SRCDIR_AWS / "include/aws/common"]:
        if not search_dir.exists():
            continue
        for ext in ("*.c", "*.h", "*.inl"):
            for fpath in sorted(search_dir.glob(ext)):
                start, end = find_func_definition(func_name, fpath)
                if start:
                    return fpath, start, end

    return None, None, None


# ── Compilation filter ─────────────────────────────────────────────────────

def compile_check(mutant_path: Path, func_name: str) -> bool:
    """Return True if the mutant compiles using CBMC (CBMC-aware compilation check).

    Uses a minimal CBMC invocation with the function's proof sources + the mutant file.
    Checks only for compilation errors (PARSING ERROR, CONVERSION ERROR, fatal error).
    """
    cfg = FUNC_CONFIGS.get(func_name)
    if cfg is None:
        return False

    all_sources = (
        cfg["proof_sources"] +
        [mutant_path] +
        cfg["project_sources"]
    )

    cmd = (
        ["cbmc"] +
        COMMON_FLAGS +
        cfg.get("defines", []) +
        cfg.get("unwind", ["--unwind", "1"]) +  # minimal unwind for compilation only
        ["--function", f"{func_name}_harness"] +  # dummy harness target
        [str(s) for s in all_sources]
    )

    try:
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=60)
        output = result.stdout + result.stderr
        # Check for compilation errors (same logic as cbmc_runner.py)
        if any(err in output for err in ["PARSING ERROR", "CONVERSION ERROR",
                                          "fatal error", "compilation terminated",
                                          "error: unknown type"]):
            return False
        return True
    except (subprocess.TimeoutExpired, FileNotFoundError):
        return False


# ── Mutant generation for one function ────────────────────────────────────

def generate_mutants_for_func(func_name: str, force: bool = False) -> dict:
    """Generate, filter, and save mutants for one function. Returns metadata dict."""
    out_dir = MUTANT_DIR / func_name
    meta_path = out_dir / "metadata.json"

    if meta_path.exists() and not force:
        return json.loads(meta_path.read_text())

    src_path, start_line, end_line = find_source_file_for_func(func_name)
    if src_path is None:
        return {"func": func_name, "status": "source_not_found", "n_raw": 0, "n_compiled": 0}

    n_lines = end_line - start_line + 1

    # Write lines file
    with tempfile.NamedTemporaryFile(mode="w", suffix=".txt", delete=False) as lf:
        for i in range(start_line, end_line + 1):
            lf.write(f"{i}\n")
        lines_file = lf.name

    # Run universalmutator into a temp directory
    with tempfile.TemporaryDirectory() as tmp_mutant_dir:
        try:
            result = subprocess.run(
                ["mutate", str(src_path), "c",
                 "--lines", lines_file,
                 "--noCheck",
                 "--mutantDir", tmp_mutant_dir],
                capture_output=True, text=True, timeout=300
            )
        except subprocess.TimeoutExpired:
            os.unlink(lines_file)
            return {"func": func_name, "status": "mutate_timeout", "n_raw": 0, "n_compiled": 0}

        raw_mutants = sorted(Path(tmp_mutant_dir).glob("*.c"))
        n_raw = len(raw_mutants)

        if n_raw == 0:
            os.unlink(lines_file)
            return {
                "func": func_name, "status": "no_mutants",
                "source_file": str(src_path), "line_range": [start_line, end_line],
                "n_lines": n_lines, "n_raw": 0, "n_compiled": 0
            }

        # Apply compilation filter
        out_dir.mkdir(parents=True, exist_ok=True)
        compiled = []
        for i, mpath in enumerate(raw_mutants):
            if compile_check(mpath, func_name):
                dest = out_dir / f"mutant_{i:04d}.c"
                shutil.copy2(mpath, dest)
                compiled.append(dest.name)

    os.unlink(lines_file)

    meta = {
        "func": func_name,
        "status": "ok",
        "source_file": str(src_path),
        "line_range": [start_line, end_line],
        "n_lines": n_lines,
        "n_raw": n_raw,
        "n_compiled": len(compiled),
        "compile_pass_rate": len(compiled) / n_raw if n_raw > 0 else 0,
    }
    meta_path.write_text(json.dumps(meta, indent=2))
    return meta


# ── Main ───────────────────────────────────────────────────────────────────

def main():
    parser = argparse.ArgumentParser(description="Generate mutants for RQ2")
    parser.add_argument("--func", help="Single function name")
    parser.add_argument("--all", action="store_true", help="All functions in FUNC_CONFIGS")
    parser.add_argument("--trial", action="store_true", help="Trial: first N functions only")
    parser.add_argument("--n", type=int, default=10, help="Number of functions for --trial")
    parser.add_argument("--workers", type=int, default=1, help="Parallel workers")
    parser.add_argument("--force", action="store_true", help="Regenerate even if output exists")
    args = parser.parse_args()

    if args.func:
        funcs = [args.func]
    elif args.all:
        funcs = sorted(FUNC_CONFIGS.keys())
    elif args.trial:
        funcs = sorted(FUNC_CONFIGS.keys())[: args.n]
    else:
        parser.print_help()
        return

    print(f"Generating mutants for {len(funcs)} functions...")
    MUTANT_DIR.mkdir(parents=True, exist_ok=True)

    total_raw = 0
    total_compiled = 0
    not_found = []
    results = []

    if args.workers > 1:
        with ProcessPoolExecutor(max_workers=args.workers) as executor:
            futures = {executor.submit(generate_mutants_for_func, f, args.force): f for f in funcs}
            for fut in as_completed(futures):
                meta = fut.result()
                results.append(meta)
    else:
        for func in funcs:
            meta = generate_mutants_for_func(func, args.force)
            results.append(meta)

    # Summary
    print("\n=== Summary ===")
    print(f"{'Function':<45} {'Source':<25} {'Raw':>6} {'Compiled':>10} {'Rate':>6}")
    print("-" * 100)
    for meta in results:
        fname = meta["func"]
        if meta.get("status") != "ok":
            print(f"{fname:<45} {'NOT FOUND':25} {0:>6} {0:>10}")
            not_found.append(fname)
            continue
        src = Path(meta["source_file"]).name
        n_raw = meta["n_raw"]
        n_comp = meta["n_compiled"]
        rate = meta.get("compile_pass_rate", 0)
        total_raw += n_raw
        total_compiled += n_comp
        print(f"{fname:<45} {src:<25} {n_raw:>6} {n_comp:>10} {rate:>6.0%}")

    print(f"\nTotal raw: {total_raw}, Total compiled: {total_compiled}")
    if funcs:
        avg_comp = total_compiled / max(1, len(funcs) - len(not_found))
        extrapolated = avg_comp * 83
        print(f"Avg compiled/function: {avg_comp:.1f}")
        print(f"Extrapolated to 83 functions: ~{extrapolated:.0f}")
    if not_found:
        print(f"\nNot found ({len(not_found)}): {not_found}")


if __name__ == "__main__":
    main()
