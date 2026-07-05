#!/usr/bin/env python3
"""
esbmc_runner.py - Run ESBMC on aws-c-common harnesses (GT or LLM-generated).

Drop-in replacement for cbmc_runner.py, adapted for ESBMC:
  - Replaces __CPROVER_assume → __ESBMC_assume via -D flag
  - Replaces utils.c with ESBMC-compatible version (no __CPROVER_uninterpreted_hasher)
  - Drops CBMC-specific memcpy/memset/memmove override stubs (ESBMC models these natively)
  - Drops --unwindset with CBMC loop identifiers (not applicable to ESBMC)
  - Adds --force-malloc-success and --z3 (required for quantifier support in AWS_IS_ZEROED)

Usage:
    python esbmc_runner.py aws_byte_cursor_from_buf    # run GT harness
    python esbmc_runner.py --llm aws_byte_buf_init path/to/harness.c
"""

import subprocess
import re
import sys
from pathlib import Path
from dataclasses import dataclass
from typing import Optional

# ── paths (env-overridable; falls back to local checkout if /root absent) ─────
import os
def _safe_exists(p):
    try:
        return Path(p).exists()
    except (PermissionError, OSError):
        return False
_DEF_SRC = "/root/aws-c-common" if _safe_exists("/root/aws-c-common") else "/home/weiqi/Verification/aws-c-common"
SRCDIR  = Path(os.environ.get("AWSC_SRC", _DEF_SRC))
PROOFDIR = SRCDIR / "verification/cbmc"

_DEF_ESBMC = "/usr/local/bin/esbmc" if _safe_exists("/usr/local/bin/esbmc") else "/home/weiqi/ESBMC_Project/esbmc/build/src/esbmc/esbmc"
ESBMC_BIN = Path(os.environ.get("ESBMC_BIN", _DEF_ESBMC))

# Override directory: ESBMC-compatible headers and utilities, stored in the
# repo so they survive reboots (unlike /tmp).
_REPO_DIR           = Path(__file__).parent.parent                      # experiment_aws_cbmc/
ESBMC_OVERRIDE_DIR  = _REPO_DIR / "esbmc_overrides"                     # repo/esbmc_overrides/
ESBMC_HELPERS_DIR   = ESBMC_OVERRIDE_DIR                                 # proof_helpers/nondet.h is inside here
ESBMC_UTILS_C       = ESBMC_OVERRIDE_DIR / "esbmc_utils.c"
ESBMC_MEMSET_STUB_C = ESBMC_OVERRIDE_DIR / "esbmc_memset_stub.c"
ESBMC_MEMCPY_STUB_C = ESBMC_OVERRIDE_DIR / "esbmc_memcpy_stub.c"

# ── stubs that are CBMC-specific and must be dropped for ESBMC ───────────────
# These files use __CPROVER_r_ok / __CPROVER_w_ok / __CPROVER_precondition which
# are not available in ESBMC; ESBMC models memcpy/memset/memmove natively.
_CBMC_ONLY_STUBS = {
    PROOFDIR / "stubs/memcpy_override.c",
    PROOFDIR / "stubs/memcpy_override_havoc.c",
    PROOFDIR / "stubs/memset_override.c",
    PROOFDIR / "stubs/memset_override_havoc.c",
    PROOFDIR / "stubs/memmove_override_havoc.c",
    PROOFDIR / "stubs/memmove_override_no_op.c",
}

# ── base ESBMC flags ──────────────────────────────────────────────────────────
# Note: no -DCBMC (would enable CBMC-specific precondition/postcondition macros)
#       no --no-standard-checks (CBMC flag, not recognised by ESBMC)
#       no --object-bits (CBMC flag)
ESBMC_BASE_FLAGS = [
    f"-I{ESBMC_OVERRIDE_DIR}",       # must be first: shadows aws/common/zero.h etc.
    f"-I{ESBMC_HELPERS_DIR}",        # shadows proof_helpers/nondet.h
    f"-I{SRCDIR}/include",
    f"-I{PROOFDIR}/include",
    "-DCBMC_OBJECT_BITS=8",
    "-DCBMC_MAX_OBJECT_SIZE=(SIZE_MAX>>(CBMC_OBJECT_BITS+1))",
    # Remap CBMC intrinsics to ESBMC equivalents at preprocessing level
    "-D__CPROVER_assume(x)=__ESBMC_assume(x)",
    "-D__CPROVER_assert(c,s)=assert(c)",
    # Arithmetic overflow predicates: use portable unsigned-type check via __typeof__
    "-D__CPROVER_overflow_plus(a,b)=((b)!=0&&(a)>((__typeof__(a))(-1)-(b)))",
    "-D__CPROVER_overflow_mult(a,b)=((a)!=0&&(b)!=0&&(a)>((__typeof__(a))(-1)/(b)))",
    "-D__CPROVER_overflow_minus(a,b)=((a)<(b))",
    # ESBMC execution flags
    "--force-malloc-success",
    "--z3",                          # required for __ESBMC_forall (used in AWS_IS_ZEROED)
    # --no-bounds-check/--no-pointer-check REMOVED 2026-07-05: CBMC's COMMON_FLAGS
    # never disables its own default pointer/bounds checks, so leaving these on
    # made ESBMC blind to a whole class of real memory-safety mutants (verified:
    # 16 NULL-deref mutants across aws_string_eq_byte_cursor/aws_string_eq_byte_buf
    # flip SUCCESS->FAIL once removed, with zero new false positives on GT, ~3.4x
    # slower). See esbmc-second-verifier memory / esbmc_vs_cbmc_report.md.
    "--no-unwinding-assertions",     # don't fail on loops that exceed unwind bound (CBMC-like)
]

# Strict-contract mode (env ESBMC_STRICT=1): enable AWS's real CBMC contract
# path so ESBMC is comparable to CBMC, not the weak base!=NULL fallback. Wires
# __CPROVER_r_ok to ESBMC's native __ESBMC_r_ok and treats preconditions as
# assumptions. Verified to lift aws_byte_buf_cat GT from 20 to 44 checks.
STRICT_CONTRACT_FLAGS = [
    "-DCBMC",
    "-D__CPROVER_r_ok(p,s)=__ESBMC_r_ok((void*)(p),(s))",
    # __CPROVER_precondition is an ASSERTION in plain BMC (AWS proofs are not in
    # function-contract mode), matching how CBMC checks it. Mapping it to assume
    # would silently swallow precondition mutations (vacuous paths).
    "-D__CPROVER_precondition(c,m)=assert(c)",
]
if os.environ.get("ESBMC_STRICT") == "1":
    ESBMC_BASE_FLAGS = ESBMC_BASE_FLAGS + STRICT_CONTRACT_FLAGS

_ESBMC_MEMLIMIT = os.environ.get("ESBMC_MEMLIMIT")
if _ESBMC_MEMLIMIT:
    ESBMC_BASE_FLAGS = ESBMC_BASE_FLAGS + ["--memlimit", _ESBMC_MEMLIMIT]

# ── per-function configuration (mirrors cbmc_runner.py FUNC_CONFIGS) ─────────
# Only lists functions from the pilot study; project/proof sources are as in
# cbmc_runner.py, but:
#   * CBMC-only stubs are removed (filtered at runtime by _filter_sources)
#   * utils.c is kept; _filter_sources replaces it with esbmc_utils.c
#   * --unwindset entries referencing CBMC loop IDs are dropped

FUNC_CONFIGS = {

    # ── Arithmetic ──────────────────────────────────────────────────────────
    "aws_add_size_checked": {
        "project_sources": [SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_add_size_saturating": {
        "project_sources": [SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_mul_size_checked": {
        "project_sources": [SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_mul_size_saturating": {
        "project_sources": [SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_is_power_of_two": {
        "project_sources": [PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_round_up_to_power_of_two": {
        "project_sources": [PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "stubs/error.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },

    # ── aws_byte_buf ─────────────────────────────────────────────────────────
    "aws_byte_buf_init": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_clean_up": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMSET_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=40"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_secure_zero": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMSET_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=40"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_reset": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMSET_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_from_array": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_from_empty_array": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_eq": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_append": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_advance": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMSET_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_append_and_update": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_cat": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_clean_up_secure": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMSET_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_eq_c_str": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_from_c_str": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_init_copy": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_init_copy_from_cursor": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_write": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_write_u8": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_write_be16": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_write_be32": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_write_be64": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_write_from_whole_buffer": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_buf_write_from_whole_cursor": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },

    # ── aws_byte_cursor ──────────────────────────────────────────────────────
    "aws_byte_cursor_advance": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_advance_nospec": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_eq": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_eq_byte_buf": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_eq_c_str": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_from_array": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_from_buf": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_from_c_str": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_read": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_read_and_fill_buffer": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_read_be16": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_read_be32": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_read_be64": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_read_u8": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_nospec_mask": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },

    # ── aws_array_list ────────────────────────────────────────────────────────
    "aws_array_list_back": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_length": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_capacity": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_front": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4", "-DAWS_DEEP_CHECKS=1"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_clear": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMSET_STUB_C],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_pop_back": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c",
                            ESBMC_MEMSET_STUB_C],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_push_back": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_get_at": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_clean_up": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_erase": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_get_at_ptr": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_init_dynamic": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_init_static": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_pop_front": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_pop_front_n": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_push_front": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_set_at": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_swap": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c",
                            ESBMC_MEMCPY_STUB_C],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_list_swap_contents": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_ITEM_SIZE=2", "-DMAX_INITIAL_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_array_eq": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },
    "aws_array_eq_c_str": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_BUFFER_SIZE=10"], "unwind": ["--unwind", "3"],
    },

    # ── aws_linked_list ───────────────────────────────────────────────────────
    "aws_linked_list_push_back": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_push_front": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4", "-DAWS_DEEP_CHECKS=1"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_pop_back": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_pop_front": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_init": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_linked_list_node_reset": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "3"],
    },
    "aws_linked_list_back": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_begin": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_end": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_front": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_insert_after": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_insert_before": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_next": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_prev": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_rbegin": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_remove": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_rend": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },
    "aws_linked_list_swap_contents": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            PROOFDIR / "stubs/empty-source-file.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_LINKED_LIST_ITEM_ALLOCATION=4"],
        "unwind": ["--unwind", "5"],
    },

    # ── aws_string ────────────────────────────────────────────────────────────
    "aws_byte_buf_write_from_whole_string": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c",
                            SRCDIR / "source/array_list.c",
                            SRCDIR / "source/error.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16", "-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
    },
    "aws_byte_cursor_from_string": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c",
                            SRCDIR / "source/error.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16", "-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
    },
    "aws_string_bytes": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },
    "aws_string_destroy": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c",
                            SRCDIR / "source/error.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },
    "aws_string_destroy_secure": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c",
                            SRCDIR / "source/error.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },
    "aws_string_eq": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },
    "aws_string_eq_byte_buf": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c",
                            SRCDIR / "source/error.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16", "-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
    },
    "aws_string_eq_byte_cursor": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c",
                            SRCDIR / "source/error.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16", "-DMAX_BUFFER_SIZE=10"],
        "unwind": ["--unwind", "3"],
    },
    "aws_string_eq_c_str": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },
    "aws_string_new_from_array": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },
    "aws_string_new_from_c_str": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },
    "aws_string_new_from_string": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/string.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": ["-DMAX_STRING_LEN=16"], "unwind": ["--unwind", "3"],
    },

    # ── misc ──────────────────────────────────────────────────────────────────
    "aws_ptr_eq": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/hash_table.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_ring_buffer_acquire": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/ring_buffer.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_ring_buffer_buf_belongs_to_pool": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/ring_buffer.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_ring_buffer_clean_up": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/ring_buffer.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_ring_buffer_init": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/ring_buffer.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
    "aws_ring_buffer_release": {
        "project_sources": [SRCDIR / "source/allocator.c",
                            SRCDIR / "source/common.c",
                            SRCDIR / "source/byte_buf.c",
                            SRCDIR / "source/ring_buffer.c"],
        "proof_sources":   [PROOFDIR / "sources/make_common_data_structures.c",
                            PROOFDIR / "stubs/error.c",
                            PROOFDIR / "sources/utils.c"],
        "defines": [], "unwind": ["--unwind", "3"],
    },
}


# ── helper: filter and remap source files ────────────────────────────────────

def _filter_sources(sources):
    """
    Given a list of source Paths:
      - Drop CBMC-only stubs (memcpy_override, memset_override, memmove_override_*)
      - Replace utils.c with ESBMC-compatible esbmc_utils.c
    """
    result = []
    utils_c = PROOFDIR / "sources/utils.c"
    for s in sources:
        if s in _CBMC_ONLY_STUBS:
            continue                   # drop CBMC-specific stubs
        if s == utils_c:
            result.append(ESBMC_UTILS_C)   # swap in ESBMC-compatible version
        else:
            result.append(s)
    return result


# ── result dataclass ──────────────────────────────────────────────────────────

@dataclass
class ESBMCResult:
    func_name: str
    harness_path: str
    returncode: int
    stdout: str
    stderr: str
    compilation_ok: bool
    verification_result: str   # "SUCCESS", "FAIL", "UNKNOWN", "TIMEOUT", "COMPILE_ERROR"
    num_checks: int
    num_failed: int
    error_summary: str

    def is_equivalent_to(self, other: "ESBMCResult") -> bool:
        return self.verification_result == other.verification_result


# ── main run function ─────────────────────────────────────────────────────────

def run_esbmc(func_name: str, harness_path: Path, timeout: int = 300) -> ESBMCResult:
    """Run ESBMC on a harness and return a structured result."""
    cfg = FUNC_CONFIGS.get(func_name)
    if cfg is None:
        raise ValueError(f"No ESBMC config for function: {func_name}")

    proof_sources   = _filter_sources(cfg["proof_sources"])
    project_sources = _filter_sources(cfg["project_sources"])

    all_sources = proof_sources + [harness_path] + project_sources

    cmd = (
        [str(ESBMC_BIN)] +
        ESBMC_BASE_FLAGS +
        cfg["defines"] +
        cfg["unwind"] +
        ["--function", f"{func_name}_harness"] +
        [str(s) for s in all_sources]
    )

    try:
        proc = subprocess.run(
            cmd,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        stdout = proc.stdout
        stderr = proc.stderr
        rc = proc.returncode
    except subprocess.TimeoutExpired:
        return ESBMCResult(
            func_name=func_name,
            harness_path=str(harness_path),
            returncode=-1,
            stdout="",
            stderr="TIMEOUT",
            compilation_ok=False,
            verification_result="TIMEOUT",
            num_checks=0,
            num_failed=0,
            error_summary=f"ESBMC timed out after {timeout}s",
        )

    # ESBMC writes all output to stderr; stdout is typically empty.
    # Combine for unified parsing.
    combined = stderr + stdout

    # Detect compilation failure
    compilation_ok = (
        "PARSING ERROR" not in combined
        and "CONVERSION ERROR" not in combined
        and "Segmentation fault" not in combined
        and rc != 139   # SIGSEGV
    )

    verification_result = "UNKNOWN"
    num_checks = 0
    num_failed = 0
    error_summary = ""

    if not compilation_ok:
        verification_result = "COMPILE_ERROR"
        lines = combined.split("\n")
        error_lines = [l for l in lines if "error" in l.lower() or "PARSING" in l or "CONVERSION" in l]
        error_summary = "\n".join(error_lines[:10])
    else:
        # ESBMC reports "Generated N VCC(s), M remaining after simplification"
        match = re.search(r'Generated (\d+) VCC', combined)
        if match:
            num_checks = int(match.group(1))

        # Failed checks appear as lines containing "FAILED"
        failed_lines = [l for l in combined.split("\n") if "FAILED" in l and "VCC" not in l]
        num_failed = len(failed_lines)

        if "VERIFICATION SUCCESSFUL" in combined:
            verification_result = "SUCCESS"
        elif "VERIFICATION FAILED" in combined:
            verification_result = "FAIL"
            error_summary = "\n".join(failed_lines[:10])

    return ESBMCResult(
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


def run_gt(func_name: str) -> ESBMCResult:
    """Run ESBMC on the ground-truth harness."""
    harness = PROOFDIR / "proofs" / func_name / f"{func_name}_harness.c"
    return run_esbmc(func_name, harness)


# ── CLI ───────────────────────────────────────────────────────────────────────

if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Run ESBMC on aws-c-common harnesses")
    parser.add_argument("func_name", help="Function name (e.g. aws_byte_buf_init)")
    parser.add_argument("--llm", metavar="HARNESS", help="Run LLM harness at this path instead of GT")
    parser.add_argument("--timeout", type=int, default=300)
    args = parser.parse_args()

    if args.llm:
        r = run_esbmc(args.func_name, Path(args.llm), args.timeout)
        label = "LLM"
    else:
        r = run_gt(args.func_name)
        label = "GT"

    print(f"[{label}] {args.func_name}")
    print(f"  Compilation : {'OK' if r.compilation_ok else 'FAIL'}")
    print(f"  Verification: {r.verification_result}")
    print(f"  VCCs        : {r.num_checks} generated, {r.num_failed} failed")
    if r.error_summary:
        print(f"  Errors:\n{r.error_summary}")
