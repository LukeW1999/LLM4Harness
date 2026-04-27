#!/usr/bin/env python3
"""
build_new_datasets.py - Build dataset entries for new functions (func31 onwards).

For each function:
  1. Find the function declaration + Doxygen in aws-c-common headers
  2. Find the function implementation in aws-c-common sources
  3. Copy GT harness from proofs directory
  4. Write to dataset/<funcN_name>/{header.h, implementation.c, ground_truth_harness.c}

Then calls prepare_blind_datasets.py logic to create condA/condB entries.
"""

import re
import sys
import subprocess
from pathlib import Path

SCRIPT_DIR    = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent
SRCDIR        = Path("/home/weiqi/aws-c-common")
PROOFDIR      = SRCDIR / "verification/cbmc"
DATASET_DIR   = EXPERIMENT_DIR / "dataset"
CONDA_DIR     = EXPERIMENT_DIR / "dataset_condA"
CONDB_DIR     = EXPERIMENT_DIR / "dataset_condB"

# Header files to search (ordered by relevance)
HEADER_FILES = [
    SRCDIR / "include/aws/common/byte_buf.h",
    SRCDIR / "include/aws/common/array_list.h",
    SRCDIR / "include/aws/common/array_list.inl",
    SRCDIR / "include/aws/common/linked_list.h",
    SRCDIR / "include/aws/common/linked_list.inl",
    SRCDIR / "include/aws/common/string.h",
    SRCDIR / "include/aws/common/ring_buffer.h",
    SRCDIR / "include/aws/common/math.h",
    SRCDIR / "include/aws/common/common.h",
    SRCDIR / "include/aws/common/math.h",
    SRCDIR / "include/aws/common/math.cbmc.inl",
    SRCDIR / "include/aws/common/byte_order.h",
    SRCDIR / "include/aws/common/byte_order.inl",
    SRCDIR / "include/aws/common/private/byte_buf.h",
    SRCDIR / "include/aws/common/hash_table.h",
    SRCDIR / "include/aws/common/string.inl",
]

SOURCE_FILES = [
    SRCDIR / "source/byte_buf.c",
    SRCDIR / "source/array_list.c",
    SRCDIR / "source/string.c",
    SRCDIR / "source/ring_buffer.c",
    SRCDIR / "source/common.c",
    SRCDIR / "source/allocator.c",
    # Inline implementations
    SRCDIR / "include/aws/common/array_list.inl",
    SRCDIR / "include/aws/common/linked_list.inl",
    SRCDIR / "include/aws/common/string.inl",
    SRCDIR / "include/aws/common/string.h",
    SRCDIR / "include/aws/common/byte_buf.h",
    SRCDIR / "include/aws/common/private/byte_buf.h",
]

IMPL_MACRO_RE = re.compile(
    r'^\s*AWS_(?:PRECONDITION|POSTCONDITION|ASSERT)\s*\(.*\)\s*;?\s*$'
)
HEADER_MACRO_COMMENT_RE = re.compile(
    r'^\s*\*\s*AWS_(?:PRECONDITION|POSTCONDITION)\s*\(.*\)\s*;?\s*$'
)


def strip_impl_macros(text: str) -> str:
    lines = text.splitlines(keepends=True)
    return ''.join(l for l in lines if not IMPL_MACRO_RE.match(l))


def strip_header_macro_comments(text: str) -> str:
    lines = text.splitlines(keepends=True)
    return ''.join(l for l in lines if not HEADER_MACRO_COMMENT_RE.match(l))


def strip_header_all_comments(text: str) -> str:
    text = re.sub(r'/\*.*?\*/', '', text, flags=re.DOTALL)
    text = re.sub(r'//.*$', '', text, flags=re.MULTILINE)
    text = re.sub(r'\n{3,}', '\n\n', text)
    return text.strip() + '\n'


def extract_function_from_header(func_name: str) -> str | None:
    """
    Extract the Doxygen comment block + function declaration for func_name.
    Returns the relevant section of the header file.
    """
    for header_path in HEADER_FILES:
        if not header_path.exists():
            continue
        content = header_path.read_text(encoding='utf-8')

        # Find the function declaration line
        decl_pattern = re.compile(
            r'(?:AWS_COMMON_API\s+)?[^\n]*\b' + re.escape(func_name) + r'\s*\(',
            re.MULTILINE
        )
        m = decl_pattern.search(content)
        if m is None:
            continue

        # Walk backwards to find the start of the preceding Doxygen comment
        pre = content[:m.start()]
        # Find the last /** or /* comment block before this declaration
        comment_start = pre.rfind('/**')
        if comment_start == -1:
            comment_start = pre.rfind('/*')
        # If comment is too far back (another function in between), skip
        between = pre[comment_start:] if comment_start != -1 else ""
        if comment_start == -1 or between.count(';') > 2:
            # No comment or too far — just return the declaration
            comment_start = m.start()

        # Walk forward to find end of function declaration (semicolon)
        rest = content[m.start():]
        semi_pos = rest.find(';')
        if semi_pos == -1:
            semi_pos = rest.find('{')
        decl_end = m.start() + semi_pos + 1

        section = content[comment_start:decl_end]
        # Include relevant struct definitions from the same header
        # (extract the full header for context)
        return header_path.read_text(encoding='utf-8'), header_path
    return None, None


def extract_function_impl(func_name: str) -> str | None:
    """Extract function implementation from source files."""
    for src_path in SOURCE_FILES:
        if not src_path.exists():
            continue
        content = src_path.read_text(encoding='utf-8')

        # Find function definition (not declaration)
        # Look for the function name followed by { on the same or next line
        pattern = re.compile(
            r'\b' + re.escape(func_name) + r'\s*\([^)]*\)\s*\{',
            re.DOTALL
        )
        m = pattern.search(content)
        if m is None:
            # Try without the { (function might span multiple lines)
            pattern2 = re.compile(
                r'(?:^|\n)(?:(?:AWS_STATIC_IMPL|static\s+)?(?:int|void|bool|size_t|'
                r'struct\s+\w+|AWS_COMMON_API)[^{]*)\b' + re.escape(func_name) + r'\b',
                re.MULTILINE
            )
            m = pattern2.search(content)
            if m is None:
                continue

        # Find the opening brace
        start = m.start()
        brace_pos = content.find('{', m.start())
        if brace_pos == -1:
            continue

        # Find the matching closing brace
        depth = 0
        pos = brace_pos
        while pos < len(content):
            if content[pos] == '{':
                depth += 1
            elif content[pos] == '}':
                depth -= 1
                if depth == 0:
                    break
            pos += 1

        impl = content[start:pos + 1]
        return impl
    return None


def build_dataset_entry(func_num: int, func_name: str) -> bool:
    dir_name = f"func{func_num}_{func_name}"
    out_dir = DATASET_DIR / dir_name

    if out_dir.exists():
        print(f"  SKIP (exists): {dir_name}")
        return True

    # 1. GT harness
    gt_harness = PROOFDIR / "proofs" / func_name / f"{func_name}_harness.c"
    if not gt_harness.exists():
        print(f"  FAIL (no GT harness): {func_name}")
        return False

    # 2. Header
    header_content, header_path = extract_function_from_header(func_name)
    if header_content is None:
        print(f"  FAIL (no header): {func_name}")
        return False

    # 3. Implementation
    impl_content = extract_function_impl(func_name)
    if impl_content is None:
        print(f"  WARN (no impl found, using placeholder): {func_name}")
        impl_content = f"/* Implementation of {func_name} — see {func_name} in aws-c-common source */\n"

    # Write raw dataset
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "header.h").write_text(header_content, encoding='utf-8')
    (out_dir / "implementation.c").write_text(impl_content, encoding='utf-8')
    (out_dir / "ground_truth_harness.c").write_text(
        gt_harness.read_text(encoding='utf-8'), encoding='utf-8'
    )

    # Write condA (NL kept, formal macros stripped)
    cond_a_dir = CONDA_DIR / dir_name
    cond_a_dir.mkdir(parents=True, exist_ok=True)
    (cond_a_dir / "header.h").write_text(
        strip_header_macro_comments(header_content), encoding='utf-8')
    (cond_a_dir / "implementation.c").write_text(
        strip_impl_macros(impl_content), encoding='utf-8')
    (cond_a_dir / "ground_truth_harness.c").write_text(
        gt_harness.read_text(encoding='utf-8'), encoding='utf-8'
    )

    # Write condB (no NL, code-only)
    cond_b_dir = CONDB_DIR / dir_name
    cond_b_dir.mkdir(parents=True, exist_ok=True)
    (cond_b_dir / "header.h").write_text(
        strip_header_all_comments(header_content), encoding='utf-8')
    (cond_b_dir / "implementation.c").write_text(
        strip_impl_macros(impl_content), encoding='utf-8')
    (cond_b_dir / "ground_truth_harness.c").write_text(
        gt_harness.read_text(encoding='utf-8'), encoding='utf-8'
    )

    print(f"  OK: {dir_name}  (header: {header_path.name})")
    return True


# New functions to add (func31 onwards) — generated by gen_func_configs.py
NEW_FUNCTIONS = [
    (31, "aws_array_eq"),
    (32, "aws_array_eq_c_str"),
    (33, "aws_array_list_clean_up"),
    (34, "aws_array_list_erase"),
    (35, "aws_array_list_get_at_ptr"),
    (36, "aws_array_list_init_dynamic"),
    (37, "aws_array_list_init_static"),
    (38, "aws_array_list_pop_front"),
    (39, "aws_array_list_pop_front_n"),
    (40, "aws_array_list_push_front"),
    (41, "aws_array_list_set_at"),
    (42, "aws_array_list_swap"),
    (43, "aws_array_list_swap_contents"),
    (44, "aws_byte_buf_advance"),
    (45, "aws_byte_buf_append_and_update"),
    (46, "aws_byte_buf_cat"),
    (47, "aws_byte_buf_clean_up_secure"),
    (48, "aws_byte_buf_eq_c_str"),
    (49, "aws_byte_buf_from_c_str"),
    (50, "aws_byte_buf_init_copy"),
    (51, "aws_byte_buf_init_copy_from_cursor"),
    (52, "aws_byte_buf_write"),
    (53, "aws_byte_buf_write_be16"),
    (54, "aws_byte_buf_write_be32"),
    (55, "aws_byte_buf_write_be64"),
    (56, "aws_byte_buf_write_from_whole_buffer"),
    (57, "aws_byte_buf_write_from_whole_cursor"),
    (58, "aws_byte_buf_write_from_whole_string"),
    (59, "aws_byte_cursor_advance_nospec"),
    (60, "aws_byte_cursor_eq"),
    (61, "aws_byte_cursor_eq_byte_buf"),
    (62, "aws_byte_cursor_eq_c_str"),
    (63, "aws_byte_cursor_from_array"),
    (64, "aws_byte_cursor_from_buf"),
    (65, "aws_byte_cursor_from_c_str"),
    (66, "aws_byte_cursor_from_string"),
    (67, "aws_byte_cursor_read"),
    (68, "aws_byte_cursor_read_and_fill_buffer"),
    (69, "aws_byte_cursor_read_be16"),
    (70, "aws_byte_cursor_read_be32"),
    (71, "aws_byte_cursor_read_be64"),
    (72, "aws_byte_cursor_read_u8"),
    (73, "aws_linked_list_back"),
    (74, "aws_linked_list_begin"),
    (75, "aws_linked_list_end"),
    (76, "aws_linked_list_front"),
    (77, "aws_linked_list_insert_after"),
    (78, "aws_linked_list_insert_before"),
    (79, "aws_linked_list_next"),
    (80, "aws_linked_list_prev"),
    (81, "aws_linked_list_rbegin"),
    (82, "aws_linked_list_rend"),
    (83, "aws_linked_list_remove"),
    (84, "aws_linked_list_swap_contents"),
    (85, "aws_nospec_mask"),
    (86, "aws_ptr_eq"),
    (87, "aws_ring_buffer_buf_belongs_to_pool"),
    (88, "aws_ring_buffer_clean_up"),
    (89, "aws_ring_buffer_init"),
    (90, "aws_ring_buffer_release"),
    (91, "aws_string_bytes"),
    (92, "aws_string_destroy"),
    (93, "aws_string_destroy_secure"),
    (94, "aws_string_eq"),
    (95, "aws_string_eq_byte_buf"),
    (96, "aws_string_eq_byte_cursor"),
    (97, "aws_string_eq_c_str"),
    (98, "aws_string_new_from_array"),
    (99, "aws_string_new_from_c_str"),
    (100, "aws_string_new_from_string"),
    (101, "aws_ring_buffer_acquire"),
]


def main():
    print(f"Building dataset entries for {len(NEW_FUNCTIONS)} new functions...")
    ok = 0
    for func_num, func_name in NEW_FUNCTIONS:
        if build_dataset_entry(func_num, func_name):
            ok += 1

    print(f"\nDone: {ok}/{len(NEW_FUNCTIONS)} succeeded")
    print(f"Dataset: {DATASET_DIR}")
    print(f"CondA:   {CONDA_DIR}")
    print(f"CondB:   {CONDB_DIR}")


if __name__ == "__main__":
    main()
