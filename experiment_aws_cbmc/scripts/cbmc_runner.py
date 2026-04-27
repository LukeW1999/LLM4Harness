#!/usr/bin/env python3
"""
cbmc_runner.py - Run CBMC on harnesses (GT or LLM-generated) and return results.

Knows the per-function compilation parameters extracted from the proof Makefiles.
"""

import subprocess
import re
from pathlib import Path
from dataclasses import dataclass
from typing import Optional

SRCDIR = Path("/home/weiqi/Verification/aws-c-common")
PROOFDIR = SRCDIR / "verification/cbmc"

# Common CBMC flags
COMMON_FLAGS = [
    f"-I{SRCDIR}/include",
    f"-I{PROOFDIR}/include",
    "-DCBMC",
    "-DCBMC_OBJECT_BITS=8",
    "-DCBMC_MAX_OBJECT_SIZE=(SIZE_MAX>>(CBMC_OBJECT_BITS+1))",
    "--object-bits", "8",
    "--no-standard-checks",
]

# Per-function compilation config
FUNC_CONFIGS = {
    "aws_add_size_checked": {
        "project_sources": [
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": [],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_byte_buf_init": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_array_list_back": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memcpy_impl.0:3"],
    },
    "aws_byte_buf_append": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memcpy_impl.0:11"],
    },
    "aws_linked_list_push_back": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
        "unwindset": [
            "--unwindset",
            "__CPROVER_file_local_linked_list_inl_aws_linked_list_is_valid_deep.0:7,"
            "ensure_linked_list_is_allocated.0:5"
        ],
    },

    # ── Arithmetic ──────────────────────────────────────────────────────────
    "aws_add_size_saturating": {
        "project_sources": [SRCDIR / "source/common.c"],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": [],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_mul_size_checked": {
        "project_sources": [SRCDIR / "source/common.c"],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": [],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_mul_size_saturating": {
        "project_sources": [SRCDIR / "source/common.c"],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": [],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_is_power_of_two": {
        "project_sources": [PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources": [],
        "defines": [],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_round_up_to_power_of_two": {
        "project_sources": [PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources": [PROOFDIR / "stubs/error.c"],
        "defines": [],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },

    # ── aws_byte_buf / aws_byte_cursor ───────────────────────────────────────
    "aws_byte_buf_clean_up": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memset_override.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=40"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memset_impl.0:41"],
    },
    "aws_byte_buf_secure_zero": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memset_override.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=40"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memset_impl.0:41"],
    },
    "aws_byte_buf_reset": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memset_override.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memset_impl.0:11"],
    },
    "aws_byte_buf_from_array": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_byte_buf_from_empty_array": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_byte_buf_write_u8": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memcpy_impl.0:11"],
    },
    "aws_byte_buf_eq": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memcmp.0:11"],
    },
    "aws_byte_cursor_advance": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },

    # ── aws_array_list ───────────────────────────────────────────────────────
    "aws_array_list_length": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_array_list_capacity": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_array_list_front": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4", "-DAWS_DEEP_CHECKS=1"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memcpy_impl.0:3"],
    },
    "aws_array_list_clear": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },
    "aws_array_list_pop_back": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memset_override.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memset_impl.0:3"],
    },
    "aws_array_list_push_back": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4",
                    "-DCBMC_OBJECT_BITS=10"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
        "object_bits": 10,   # needs more addressed objects than default 256
    },
    "aws_array_list_get_at": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": ["--unwindset", "memcpy_impl.0:3"],
    },
    # ── aws_linked_list ──────────────────────────────────────────────────────
    "aws_linked_list_push_front": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4", "-DAWS_DEEP_CHECKS=1"],
        "unwind": ["--unwind", "5"],
        "unwindset": [
            "--unwindset",
            "__CPROVER_file_local_linked_list_inl_aws_linked_list_is_valid_deep.0:7,"
            "ensure_linked_list_is_allocated.0:5"
        ],
    },
    "aws_linked_list_pop_back": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
        "unwindset": [
            "--unwindset",
            "__CPROVER_file_local_linked_list_inl_aws_linked_list_is_valid_deep.0:6,"
            "ensure_linked_list_is_allocated.0:5"
        ],
    },
    "aws_linked_list_pop_front": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
        "unwindset": [
            "--unwindset",
            "__CPROVER_file_local_linked_list_inl_aws_linked_list_is_valid_deep.0:6,"
            "ensure_linked_list_is_allocated.0:5"
        ],
    },
    "aws_linked_list_init": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": [
            "--unwindset",
            "__CPROVER_file_local_linked_list_inl_aws_linked_list_is_valid_deep.0:2"
        ],
    },
    "aws_linked_list_node_reset": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
        "unwindset": [],
    },

    # ── aws_byte_buf / aws_byte_cursor (new) ─────────────────────────────────────
    "aws_array_eq": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcmp.0:11'],
    },
    "aws_array_eq_c_str": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'aws_array_eq_c_str.0:11,strlen.0:11'],
    },
    "aws_byte_buf_advance": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memset_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memset_impl.0:11'],
    },
    "aws_byte_buf_append_and_update": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_buf_cat": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'aws_byte_buf_cat.0:4'],
    },
    "aws_byte_buf_clean_up_secure": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "stubs/memset_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memset_impl.0:11'],
    },
    "aws_byte_buf_eq_c_str": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'aws_array_eq_c_str.0:11,strlen.0:11'],
    },
    "aws_byte_buf_from_c_str": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'strlen.0:11'],
    },
    "aws_byte_buf_init_copy": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcpy_impl.0:11'],
    },
    "aws_byte_buf_init_copy_from_cursor": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcpy_impl.0:11'],
    },
    "aws_byte_buf_write": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_buf_write_be16": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcpy_impl.0:11'],
    },
    "aws_byte_buf_write_be32": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcpy_impl.0:11'],
    },
    "aws_byte_buf_write_be64": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcpy_impl.0:11'],
    },
    "aws_byte_buf_write_from_whole_buffer": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcpy_impl.0:11'],
    },
    "aws_byte_buf_write_from_whole_cursor": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcpy_impl.0:11'],
    },
    "aws_byte_cursor_advance_nospec": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_eq": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcmp.0:11'],
    },
    "aws_byte_cursor_eq_byte_buf": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcmp.0:11'],
    },
    "aws_byte_cursor_eq_c_str": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'aws_array_eq_c_str.0:11,strlen.0:11'],
    },
    "aws_byte_cursor_from_array": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_from_buf": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_from_c_str": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'strlen.0:11'],
    },
    "aws_byte_cursor_read": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_read_and_fill_buffer": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_read_be16": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_read_be32": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_read_be64": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_read_u8": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_nospec_mask": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },

    # ── aws_string (new) ─────────────────────────────────────────────────────────
    "aws_byte_buf_write_from_whole_string": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/error.c",
            SRCDIR / "source/error.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_byte_cursor_from_string": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
            SRCDIR / "source/error.c",
            SRCDIR / "source/error.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_string_bytes": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_string_destroy": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
            SRCDIR / "source/error.c",
            SRCDIR / "source/error.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_string_destroy_secure": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
            SRCDIR / "source/error.c",
            SRCDIR / "source/error.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_string_eq": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcmp.0:17'],
    },
    "aws_string_eq_byte_buf": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
            SRCDIR / "source/error.c",
            SRCDIR / "source/error.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcmp.0:17,strlen.0:17'],
    },
    "aws_string_eq_byte_cursor": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
            SRCDIR / "source/error.c",
            SRCDIR / "source/error.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'memcmp.0:17'],
    },
    "aws_string_eq_c_str": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'aws_array_eq_c_str.0:17,memcmp.0:17,strlen.0:17'],
    },
    "aws_string_new_from_array": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_string_new_from_c_str": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": ['--unwindset', 'strlen.0:17'],
    },
    "aws_string_new_from_string": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/string.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_STRING_LEN=16', '-DMAX_BUFFER_SIZE=10'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },

    # ── aws_array_list (new) ──────────────────────────────────────────────────────
    "aws_array_list_clean_up": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_erase": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memmove_override_havoc.c",
            PROOFDIR / "stubs/memcpy_override_havoc.c",
            PROOFDIR / "stubs/memset_override_havoc.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_get_at_ptr": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_init_dynamic": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_init_static": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_pop_front": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memmove_override_no_op.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_pop_front_n": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memmove_override_no_op.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_push_front": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memmove_override_no_op.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_set_at": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_swap": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/memcpy_override_havoc.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_array_list_swap_contents": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/array_list.c",
            SRCDIR / "source/common.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": ['-DMAX_ITEM_SIZE=2', '-DMAX_INITIAL_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },

    # ── aws_linked_list (new) ─────────────────────────────────────────────────────
    "aws_linked_list_back": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_begin": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_end": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_front": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_insert_after": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_insert_before": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_next": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_prev": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_rbegin": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_remove": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_rend": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },
    "aws_linked_list_swap_contents": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            PROOFDIR / "stubs/empty-source-file.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "sources/utils.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": ['-DMAX_LINKED_LIST_ITEM_ALLOCATION=4'],
        "unwind": ['--unwind', '5'],
        "unwindset": [],
    },

    # ── misc (new) ────────────────────────────────────────────────────────────────
    "aws_ptr_eq": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/hash_table.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
        ],
        "defines": [],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_ring_buffer_acquire": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/ring_buffer.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": [],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_ring_buffer_buf_belongs_to_pool": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/ring_buffer.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": [],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_ring_buffer_clean_up": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/ring_buffer.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": [],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },
    "aws_ring_buffer_init": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/ring_buffer.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": [],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
        "remove_function_body": ['__CPROVER_file_local_ring_buffer_c_s_ring_buffer_mem_acquire'],
    },
    "aws_ring_buffer_release": {
        "project_sources": [
            SRCDIR / "source/allocator.c",
            SRCDIR / "source/common.c",
            SRCDIR / "source/byte_buf.c",
            SRCDIR / "source/ring_buffer.c",
        ],
        "proof_sources": [
            PROOFDIR / "sources/make_common_data_structures.c",
            PROOFDIR / "stubs/error.c",
            PROOFDIR / "sources/utils.c",
        ],
        "defines": [],
        "unwind": ['--unwind', '3'],
        "unwindset": [],
    },

}


@dataclass
class CBMCResult:
    func_name: str
    harness_path: str
    returncode: int
    stdout: str
    stderr: str
    compilation_ok: bool
    verification_result: str   # "SUCCESS", "FAIL", "UNKNOWN", "TIMEOUT"
    num_checks: int
    num_failed: int
    error_summary: str         # human-readable, for LLM feedback

    def is_equivalent_to(self, other: "CBMCResult") -> bool:
        """Check if verification result matches another run."""
        return self.verification_result == other.verification_result


def run_cbmc(func_name: str, harness_path: Path, timeout: int = 120) -> CBMCResult:
    """Run CBMC on a harness file and return structured result."""
    cfg = FUNC_CONFIGS.get(func_name)
    if cfg is None:
        raise ValueError(f"No config for function: {func_name}")

    all_sources = (
        cfg["proof_sources"] +
        [harness_path] +
        cfg["project_sources"]
    )

    # Build base flags, allowing per-function object-bits override
    obj_bits = cfg.get("object_bits", 8)
    if obj_bits != 8:
        base_flags = [
            f"-I{SRCDIR}/include",
            f"-I{PROOFDIR}/include",
            "-DCBMC",
            f"-DCBMC_OBJECT_BITS={obj_bits}",
            "-DCBMC_MAX_OBJECT_SIZE=(SIZE_MAX>>(CBMC_OBJECT_BITS+1))",
            f"--object-bits", str(obj_bits),
            "--no-standard-checks",
        ]
        # Remove the object_bits define from cfg["defines"] if already there
        defines = [d for d in cfg["defines"] if "CBMC_OBJECT_BITS" not in d]
    else:
        base_flags = COMMON_FLAGS
        defines = cfg["defines"]

    remove_flags = []
    for fn in cfg.get("remove_function_body", []):
        remove_flags += ["--remove-function-body", fn]

    cmd = (
        ["cbmc"] +
        base_flags +
        defines +
        cfg["unwind"] +
        (cfg["unwindset"] if cfg["unwindset"] else []) +
        remove_flags +
        ["--function", f"{func_name}_harness"] +
        [str(s) for s in all_sources]
    )

    try:
        proc = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout
        )
        stdout = proc.stdout
        stderr = proc.stderr
        rc = proc.returncode
    except subprocess.TimeoutExpired:
        return CBMCResult(
            func_name=func_name,
            harness_path=str(harness_path),
            returncode=-1,
            stdout="",
            stderr="TIMEOUT",
            compilation_ok=False,
            verification_result="TIMEOUT",
            num_checks=0,
            num_failed=0,
            error_summary="CBMC timed out after {timeout}s",
        )

    # Check compilation
    compilation_ok = "PARSING ERROR" not in stdout and "CONVERSION ERROR" not in stdout and \
                     "fatal error" not in stderr and "compilation terminated" not in stderr

    # Parse verification result
    verification_result = "UNKNOWN"
    num_checks = 0
    num_failed = 0
    error_summary = ""

    if not compilation_ok:
        verification_result = "COMPILE_ERROR"
        # Extract useful error info
        lines = (stdout + stderr).split("\n")
        error_lines = [l for l in lines if "error" in l.lower() or "fatal" in l.lower()
                       or "PARSING" in l or "CONVERSION" in l]
        error_summary = "\n".join(error_lines[:10])
    else:
        # Parse results
        match = re.search(r'\*\* (\d+) of (\d+) failed', stdout)
        if match:
            num_failed = int(match.group(1))
            num_checks = int(match.group(2))

        if "VERIFICATION SUCCESSFUL" in stdout:
            verification_result = "SUCCESS"
        elif "VERIFICATION FAILED" in stdout:
            verification_result = "FAIL"
            # Extract failed assertions for LLM feedback
            fail_lines = [l for l in stdout.split("\n") if "FAILED" in l or "failure" in l.lower()]
            error_summary = "\n".join(fail_lines[:10])

    return CBMCResult(
        func_name=func_name,
        harness_path=str(harness_path),
        returncode=rc,
        stdout=stdout,
        stderr=stderr,
        compilation_ok=compilation_ok,
        verification_result=verification_result,
        num_checks=num_checks,
        num_failed=num_failed,
        error_summary=error_summary,
    )


def run_gt(func_name: str) -> CBMCResult:
    """Run CBMC on the ground truth harness."""
    harness = PROOFDIR / "proofs" / func_name / f"{func_name}_harness.c"
    return run_cbmc(func_name, harness)


if __name__ == "__main__":
    import sys
    func = sys.argv[1] if len(sys.argv) > 1 else "aws_array_list_back"
    print(f"Running GT harness for {func}...")
    r = run_gt(func)
    print(f"  Compilation: {'OK' if r.compilation_ok else 'FAIL'}")
    print(f"  Verification: {r.verification_result}")
    print(f"  Checks: {r.num_failed}/{r.num_checks} failed")
    if r.error_summary:
        print(f"  Errors: {r.error_summary}")
