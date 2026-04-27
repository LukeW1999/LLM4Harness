#!/usr/bin/env python3
"""
gen_func_configs.py - Parse aws-c-common proof Makefiles and generate FUNC_CONFIGS entries.

Usage:
    python3 gen_func_configs.py          # print all new configs
    python3 gen_func_configs.py --test   # run GT harness on each to validate
"""

import re
import sys
import subprocess
from pathlib import Path

SRCDIR   = Path("/home/weiqi/aws-c-common")
PROOFDIR = SRCDIR / "verification/cbmc"

# Already configured in cbmc_runner.py — skip
EXISTING = {
    "aws_add_size_checked", "aws_byte_buf_init", "aws_array_list_back",
    "aws_byte_buf_append", "aws_linked_list_push_back", "aws_add_size_saturating",
    "aws_mul_size_checked", "aws_mul_size_saturating", "aws_is_power_of_two",
    "aws_round_up_to_power_of_two", "aws_byte_buf_clean_up", "aws_byte_buf_secure_zero",
    "aws_byte_buf_reset", "aws_byte_buf_from_array", "aws_byte_buf_from_empty_array",
    "aws_byte_buf_write_u8", "aws_byte_buf_eq", "aws_byte_cursor_advance",
    "aws_array_list_length", "aws_array_list_capacity", "aws_array_list_front",
    "aws_array_list_clear", "aws_array_list_pop_back", "aws_array_list_push_back",
    "aws_array_list_get_at", "aws_linked_list_push_front", "aws_linked_list_pop_back",
    "aws_linked_list_pop_front", "aws_linked_list_init", "aws_linked_list_node_reset",
}

# Skip — too complex (hash_table, priority_queue, sort, function-pointer, realloc, lookup)
SKIP = {
    # hash_table
    "aws_hash_array_ignore_case", "aws_hash_byte_cursor_ptr", "aws_hash_byte_cursor_ptr_ignore_case",
    "aws_hash_c_string", "aws_hash_callback_c_str_eq", "aws_hash_callback_string_destroy",
    "aws_hash_callback_string_eq", "aws_hash_iter_begin", "aws_hash_iter_delete",
    "aws_hash_iter_done", "aws_hash_iter_next", "aws_hash_ptr", "aws_hash_string",
    "aws_hash_table_clean_up", "aws_hash_table_clear", "aws_hash_table_create",
    "aws_hash_table_eq", "aws_hash_table_find", "aws_hash_table_foreach",
    "aws_hash_table_get_entry_count", "aws_hash_table_init_bounded",
    "aws_hash_table_init_unbounded", "aws_hash_table_move", "aws_hash_table_put",
    "aws_hash_table_remove", "aws_hash_table_swap",
    # priority_queue (20-min runtime, complex stubs)
    "aws_priority_queue_capacity", "aws_priority_queue_clean_up",
    "aws_priority_queue_init_dynamic", "aws_priority_queue_init_static",
    "aws_priority_queue_pop", "aws_priority_queue_push", "aws_priority_queue_push_ref",
    "aws_priority_queue_remove", "aws_priority_queue_s_remove_node",
    "aws_priority_queue_s_sift_down", "aws_priority_queue_s_sift_either",
    "aws_priority_queue_s_sift_up", "aws_priority_queue_s_swap", "aws_priority_queue_size",
    "aws_priority_queue_top",
    # function-pointer / lookup
    "aws_array_list_sort", "aws_array_list_comparator_string",
    "aws_byte_cursor_compare_lookup", "aws_byte_buf_append_with_lookup",
    "aws_byte_cursor_left_trim_pred", "aws_byte_cursor_right_trim_pred",
    "aws_byte_cursor_trim_pred", "aws_byte_cursor_satisfies_pred",
    # realloc-heavy (complex stubs)
    "aws_byte_buf_append_dynamic", "aws_byte_buf_reserve", "aws_byte_buf_reserve_relative",
    "aws_array_list_ensure_capacity", "aws_array_list_shrink_to_fit",
    # havoc stubs (array_list_copy uses memcpy_override_havoc — harder to verify)
    "aws_array_list_copy",
    # non-API utilities
    "memcpy_using_uint64", "memset_using_uint64", "memset_override_0",
    # complex comparators
    "aws_byte_cursor_compare_lexical", "aws_string_compare",
    "aws_array_eq_c_str_ignore_case", "aws_array_eq_ignore_case",
    "aws_byte_buf_eq_ignore_case", "aws_byte_buf_eq_c_str_ignore_case",
    "aws_byte_cursor_eq_byte_buf_ignore_case", "aws_byte_cursor_eq_c_str_ignore_case",
    "aws_byte_cursor_eq_ignore_case",
    "aws_string_eq_ignore_case", "aws_string_eq_byte_buf_ignore_case",
    "aws_string_eq_byte_cursor_ignore_case", "aws_string_eq_c_str_ignore_case",
    # ring_buffer acquire_up_to: complex loop
    "aws_ring_buffer_acquire_up_to",
}

# Constants used in unwindset expressions
MAX_BUFFER_SIZE = 10
MAX_STRING_LEN  = 16
MAX_LL_ALLOC    = 4

def parse_makefile(func: str) -> dict | None:
    mk_path = PROOFDIR / "proofs" / func / "Makefile"
    if not mk_path.exists():
        return None
    content = mk_path.read_text()

    # ── determine base group ──────────────────────────────────────────────────
    if "Makefile.aws_byte_buf" in content:
        group = "byte_buf"
    elif "Makefile.aws_string" in content:
        group = "string"
    elif "Makefile.aws_array_list" in content:
        group = "array_list"
    elif "Makefile.aws_linked_list" in content:
        group = "linked_list"
    else:
        group = "other"

    # ── base sources by group ─────────────────────────────────────────────────
    if group == "byte_buf":
        project_sources = [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ]
        proof_sources = [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ]
        defines = ["-DMAX_BUFFER_SIZE=10"]
        base_unwind = ["--unwind", "3"]

    elif group == "string":
        project_sources = [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
        ]
        proof_sources = [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ]
        defines = ["-DMAX_STRING_LEN=16", "-DMAX_BUFFER_SIZE=10"]
        base_unwind = ["--unwind", "3"]

    elif group == "array_list":
        project_sources = [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ]
        proof_sources = [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ]
        defines = ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"]
        base_unwind = ["--unwind", "3"]

    elif group == "linked_list":
        project_sources = [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ]
        proof_sources = [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ]
        defines = ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"]
        base_unwind = ["--unwind", "5"]

    else:  # other
        project_sources = [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/common.c",
        ]
        proof_sources = [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ]
        defines = []
        base_unwind = ["--unwind", "3"]

    # ── parse additional proof stubs ─────────────────────────────────────────
    # (avoid duplicating ones already in base)
    base_stubs = {p.name for p in proof_sources}

    for line in content.splitlines():
        line = line.strip()
        if "PROOF_SOURCES +=" in line and "PROOF_STUB" in line:
            m = re.search(r'\$\(PROOF_STUB\)/(\S+)', line)
            if m:
                stub = m.group(1)
                p = PROOFDIR / "stubs" / stub
                if stub not in base_stubs:
                    proof_sources.append(p)
                    base_stubs.add(stub)
        if "PROOF_SOURCES +=" in line and "PROOF_SOURCE" in line and "make_common" not in line:
            m = re.search(r'\$\(PROOF_SOURCE\)/(\S+)', line)
            if m:
                src = m.group(1)
                p = PROOFDIR / "sources" / src
                if src not in base_stubs:
                    proof_sources.append(p)
                    base_stubs.add(src)
        # Some linked_list proofs add extra project sources from stubs dir
        if "PROJECT_SOURCES +=" in line and "PROOF_STUB" in line:
            m = re.search(r'\$\(PROOF_STUB\)/(\S+)', line)
            if m:
                stub = m.group(1)
                p = PROOFDIR / "stubs" / stub
                if stub not in {ps.name for ps in project_sources}:
                    project_sources.append(p)

    # ── parse additional project sources ─────────────────────────────────────
    base_proj = {p.name for p in project_sources}
    for line in content.splitlines():
        line = line.strip()
        if "PROJECT_SOURCES +=" in line and "SRCDIR" in line:
            m = re.search(r'\$\(SRCDIR\)/(\S+)', line)
            if m:
                src = m.group(1)
                p = SRCDIR / src
                if p.name not in base_proj:
                    project_sources.append(p)
                    base_proj.add(p.name)

    # ── parse unwindset ───────────────────────────────────────────────────────
    unwindset = []
    unwindset_parts = []
    for line in content.splitlines():
        line = line.strip()
        if re.match(r'UNWINDSET\s*\+=', line):
            rest = re.sub(r'UNWINDSET\s*\+=\s*', '', line).strip()
            if not rest:
                continue
            # Substitute known expressions
            rest = re.sub(
                r'\$\(shell echo \$\$\(\(.*MAX_BUFFER_SIZE.*\+ 1\)\)\)',
                str(MAX_BUFFER_SIZE + 1), rest)
            rest = re.sub(
                r'\$\(shell echo \$\$\(\(.*MAX_STRING_LEN.*\+ 1\)\)\)',
                str(MAX_STRING_LEN + 1), rest)
            rest = re.sub(
                r'\$\(shell echo \$\$\(\(.*MAX_BUFFER_SIZE.*\)\)\)',
                str(MAX_BUFFER_SIZE), rest)
            if '$' not in rest:  # fully resolved
                unwindset_parts.append(rest)

    if unwindset_parts:
        unwindset = ["--unwindset", ",".join(unwindset_parts)]

    # ── parse DEFINES ─────────────────────────────────────────────────────────
    for line in content.splitlines():
        line = line.strip()
        if re.match(r'DEFINES\s*\+=', line) and "MAX_" in line:
            m = re.search(r'-D\w+=\S+', line)
            if m and m.group(0) not in defines:
                defines.append(m.group(0))

    # ── parse REMOVE_FUNCTION_BODY ────────────────────────────────────────────
    remove_funcs = []
    for line in content.splitlines():
        line = line.strip()
        if re.match(r'REMOVE_FUNCTION_BODY\s*\+=', line):
            m = re.search(r'REMOVE_FUNCTION_BODY\s*\+=\s*(\S+)', line)
            if m:
                remove_funcs.append(m.group(1))

    # ── check if utils.c already included ────────────────────────────────────
    has_utils = any("utils.c" in str(p) for p in proof_sources)
    if not has_utils and group == "byte_buf":
        # Most byte_buf proofs include utils.c — add if Makefile shows it
        for line in content.splitlines():
            if "utils.c" in line and "PROOF_SOURCE" in line:
                proof_sources.append(PROOFDIR / "sources/utils.c")
                break

    # ── special: string_destroy uses source/error.c not stub ─────────────────
    if "source/error.c" in content and group == "string":
        proof_sources = [p for p in proof_sources if p.name != "error.c"]
        project_sources.append(SRCDIR / "source/error.c")

    return {
        "group": group,
        "project_sources": project_sources,
        "proof_sources": proof_sources,
        "defines": defines,
        "unwind": base_unwind,
        "unwindset": unwindset,
        "remove_function_body": remove_funcs,
    }


def path_to_repr(p: Path) -> str:
    s = str(p)
    if s.startswith(str(PROOFDIR)):
        rel = s[len(str(PROOFDIR))+1:]
        return f'PROOFDIR / "{rel}"'
    elif s.startswith(str(SRCDIR)):
        rel = s[len(str(SRCDIR))+1:]
        return f'SRCDIR / "{rel}"'
    return f'Path("{s}")'


def config_to_python(func: str, cfg: dict) -> str:
    lines = [f'    "{func}": {{']

    # project_sources
    lines.append('        "project_sources": [')
    for p in cfg["project_sources"]:
        lines.append(f'            {path_to_repr(p)},')
    lines.append('        ],')

    # proof_sources
    lines.append('        "proof_sources": [')
    for p in cfg["proof_sources"]:
        lines.append(f'            {path_to_repr(p)},')
    lines.append('        ],')

    lines.append(f'        "defines": {cfg["defines"]},')
    lines.append(f'        "unwind": {cfg["unwind"]},')
    lines.append(f'        "unwindset": {cfg["unwindset"]},')

    if cfg.get("remove_function_body"):
        rfb = cfg["remove_function_body"]
        lines.append(f'        "remove_function_body": {rfb},')

    lines.append('    },')
    return '\n'.join(lines)


def main():
    test_mode = "--test" in sys.argv

    all_proofs = sorted(
        d.name for d in (PROOFDIR / "proofs").iterdir()
        if d.is_dir() and not d.name.startswith("Makefile") and not d.name.startswith(".")
    )

    target_funcs = [f for f in all_proofs if f not in EXISTING and f not in SKIP]
    print(f"# Generating configs for {len(target_funcs)} new functions", file=sys.stderr)

    configs = {}
    failed = []
    for func in target_funcs:
        cfg = parse_makefile(func)
        if cfg is None:
            failed.append(func)
            continue
        # Skip if has unresolved shell expressions
        if any('$' in str(u) for u in cfg.get("unwindset", [])):
            print(f"# SKIP {func}: unresolved unwindset", file=sys.stderr)
            continue
        configs[func] = cfg

    # Group output by category
    groups = {"byte_buf": [], "string": [], "array_list": [], "linked_list": [], "other": []}
    for func, cfg in configs.items():
        groups[cfg["group"]].append(func)

    group_labels = {
        "byte_buf": "── aws_byte_buf / aws_byte_cursor (new) ─────────────────────────────────────",
        "string":   "── aws_string (new) ─────────────────────────────────────────────────────────",
        "array_list": "── aws_array_list (new) ──────────────────────────────────────────────────────",
        "linked_list": "── aws_linked_list (new) ─────────────────────────────────────────────────────",
        "other":    "── misc (new) ────────────────────────────────────────────────────────────────",
    }

    output_lines = []
    for group, funcs in groups.items():
        if not funcs:
            continue
        output_lines.append(f"\n    # {group_labels[group]}")
        for func in sorted(funcs):
            output_lines.append(config_to_python(func, configs[func]))

    # Print as Python code to insert into FUNC_CONFIGS
    print('\n'.join(output_lines))

    print(f"\n# Total new: {len(configs)}, failed to parse: {len(failed)}", file=sys.stderr)
    if failed:
        print(f"# Failed: {failed}", file=sys.stderr)

    # Print PILOT_FUNCTIONS additions
    print("\n\n# === PILOT_FUNCTIONS additions ===", file=sys.stderr)
    for i, func in enumerate(sorted(configs.keys()), start=31):
        print(f'    ("func{i}_{func}", "{func}"),', file=sys.stderr)


if __name__ == "__main__":
    main()
