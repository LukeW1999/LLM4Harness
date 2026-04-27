#!/usr/bin/env python3
"""
prepare_blind_datasets.py - Generate Condition A and Condition B datasets.

Condition A (NL-guided): Strip AWS_PRECONDITION/AWS_POSTCONDITION/AWS_ASSERT
  macro calls from implementation.c, and strip the formal macro lines from
  header.h comments. Keep all natural language (Doxygen prose, Requires/Ensures).

Condition B (Code-only): Same impl.c stripping as A, but ALSO strip all
  comment blocks from header.h — leaving only struct definitions and
  function signatures. LLM must infer everything from code logic alone.

Output:
  experiment_aws_cbmc/dataset_condA/<func_dir>/{header.h, implementation.c}
  experiment_aws_cbmc/dataset_condB/<func_dir>/{header.h, implementation.c}
"""

import re
import shutil
from pathlib import Path

SCRIPT_DIR = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent
DATASET_DIR = EXPERIMENT_DIR / "dataset"
CONDA_DIR = EXPERIMENT_DIR / "dataset_condA"
CONDB_DIR = EXPERIMENT_DIR / "dataset_condB"

# Lines in implementation.c to strip (standalone macro call statements)
IMPL_MACRO_RE = re.compile(
    r'^\s*AWS_(?:PRECONDITION|POSTCONDITION|ASSERT)\s*\(.*\)\s*;?\s*$'
)

# Lines in header.h Doxygen comments to strip (e.g. " * AWS_PRECONDITION(...);")
HEADER_MACRO_COMMENT_RE = re.compile(
    r'^\s*\*\s*AWS_(?:PRECONDITION|POSTCONDITION)\s*\(.*\)\s*;?\s*$'
)


def strip_impl_macros(text: str) -> str:
    """Remove standalone AWS_PRECONDITION/POSTCONDITION/ASSERT lines."""
    lines = text.splitlines(keepends=True)
    result = []
    for line in lines:
        if IMPL_MACRO_RE.match(line):
            continue
        result.append(line)
    return ''.join(result)


def strip_header_macro_comments(text: str) -> str:
    """
    Condition A: remove only the formal macro lines inside Doxygen comments.
    e.g.:  * AWS_PRECONDITION(aws_byte_buf_is_valid(to));
    Keep all NL prose lines.
    """
    lines = text.splitlines(keepends=True)
    result = []
    for line in lines:
        if HEADER_MACRO_COMMENT_RE.match(line):
            continue
        result.append(line)
    return ''.join(result)


def strip_header_all_comments(text: str) -> str:
    """
    Condition B: remove ALL comment blocks, keeping only:
      - struct/enum/typedef definitions
      - function declarations
      - #define / #include lines (if any)
    Strategy: remove /* ... */ blocks (both single-line and multi-line),
    then remove // line comments.
    """
    # Remove multi-line /* ... */ comments
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    # Remove // comments
    text = re.sub(r'//.*$', '', text, flags=re.MULTILINE)
    # Collapse multiple blank lines into one
    text = re.sub(r'\n{3,}', '\n\n', text)
    return text.strip() + '\n'


def process_function(func_dir: Path, out_a: Path, out_b: Path):
    header_src = func_dir / "header.h"
    impl_src = func_dir / "implementation.c"

    if not header_src.exists() or not impl_src.exists():
        print(f"  SKIP (missing files): {func_dir.name}")
        return

    header_text = header_src.read_text(encoding='utf-8')
    impl_text = impl_src.read_text(encoding='utf-8')

    # --- Condition A ---
    header_a = strip_header_macro_comments(header_text)
    impl_a = strip_impl_macros(impl_text)
    out_a.mkdir(parents=True, exist_ok=True)
    (out_a / "header.h").write_text(header_a, encoding='utf-8')
    (out_a / "implementation.c").write_text(impl_a, encoding='utf-8')

    # --- Condition B ---
    header_b = strip_header_all_comments(header_text)
    impl_b = impl_a  # same impl stripping
    out_b.mkdir(parents=True, exist_ok=True)
    (out_b / "header.h").write_text(header_b, encoding='utf-8')
    (out_b / "implementation.c").write_text(impl_b, encoding='utf-8')

    print(f"  OK: {func_dir.name}")


def main():
    func_dirs = sorted(DATASET_DIR.iterdir())
    func_dirs = [d for d in func_dirs if d.is_dir()]

    print(f"Processing {len(func_dirs)} functions...")
    for func_dir in func_dirs:
        out_a = CONDA_DIR / func_dir.name
        out_b = CONDB_DIR / func_dir.name
        process_function(func_dir, out_a, out_b)

    print(f"\nCondition A -> {CONDA_DIR}")
    print(f"Condition B -> {CONDB_DIR}")


if __name__ == "__main__":
    main()
