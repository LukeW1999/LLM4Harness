#!/usr/bin/env python3
"""
cbmc_runner_s2n.py
==================
Run CBMC on s2n-tls stuffer harnesses (GT or LLM-generated).

Uses a three-step approach matching the s2n-tls Makefile.common:
  1. goto-cc  : compile all sources to a single .goto binary
  2. goto-instrument : apply REMOVE_FUNCTION_BODY transformations
  3. cbmc     : run bounded model checking on the .goto binary

Auto-parses per-proof Makefiles to extract compilation parameters.
"""

import re
import subprocess
import tempfile
from pathlib import Path
from dataclasses import dataclass
from typing import Optional

def _path_exists(p):
    try:
        return p.exists()
    except PermissionError:
        return False
_server_s2n = Path("/root/s2n-tls")
S2N_DIR   = _server_s2n if _path_exists(_server_s2n) else Path("/home/weiqi/Verification/s2n-tls")
CBMC_ROOT = S2N_DIR / "tests/cbmc"
PROOFS_DIR = CBMC_ROOT / "proofs"
PROOF_SOURCE = CBMC_ROOT / "sources"
PROOF_STUB   = CBMC_ROOT / "stubs"

# Preprocessor/include flags for goto-cc compilation
COMPILE_INCLUDES = [
    f"-I{CBMC_ROOT}/include",
    f"-I{CBMC_ROOT}/aws-verification-model-for-libcrypto/include",
    f"-I{S2N_DIR}",
    f"-I{S2N_DIR}/api",
    f"-I{S2N_DIR}/crypto",
]

COMPILE_DEFINES = [
    "-DAWS_DEEP_CHECKS=0",
    "-DCBMC=1",
    "-DCBMC_OBJECT_BITS=8",
    "-DCBMC_MAX_OBJECT_SIZE=(SIZE_MAX>>(CBMC_OBJECT_BITS+1))",
]

# CBMC flags for the analysis step.
# --no-standard-checks disables built-in overflow/pointer checks so CBMC only
# verifies explicit assert() statements — matching the original proof intent.
CBMC_ANALYSIS_FLAGS = [
    "--object-bits", "8",
    "--flush",
]


OPENSSL_SOURCE = CBMC_ROOT / "aws-verification-model-for-libcrypto/source"


def _expand_make_vars(val: str, local_vars: dict) -> str:
    """Expand simple $(VAR) references using local_vars dict."""
    def _sub(m):
        name = m.group(1)
        return local_vars.get(name, m.group(0))
    return re.sub(r'\$\((\w+)\)', _sub, val)


def _resolve_makefile_var(val: str, local_vars: dict | None = None) -> Optional[Path]:
    """Resolve Makefile variable references to absolute paths."""
    if local_vars:
        val = _expand_make_vars(val, local_vars)
    val = val.strip()
    if val.startswith("$(PROOF_SOURCE)/"):
        return PROOF_SOURCE / val[len("$(PROOF_SOURCE)/"):]
    if val.startswith("$(PROOF_STUB)/"):
        return PROOF_STUB / val[len("$(PROOF_STUB)/"):]
    if val.startswith("$(SRCDIR)/"):
        return S2N_DIR / val[len("$(SRCDIR)/"):]
    if val.startswith("$(OPENSSL_SOURCE)/"):
        return OPENSSL_SOURCE / val[len("$(OPENSSL_SOURCE)/"):]
    if val.startswith("$(PROOFDIR)/$(HARNESS_FILE)"):
        return None  # Harness file — caller injects this
    return None


def parse_proof_makefile(func_name: str) -> dict:
    """
    Parse per-proof Makefile → config dict with resolved paths.
    Returns:
      proof_sources         : list of Path (non-harness)
      project_sources       : list of Path
      remove_function_body  : list of str  (function names to stub out)
      unwindset             : str          (comma-separated, may be empty)
      extra_defines         : list of str  (additional -D flags from DEFINES +=)
    """
    makefile = PROOFS_DIR / func_name / "Makefile"
    if not makefile.exists():
        raise FileNotFoundError(f"Makefile not found: {makefile}")

    proof_sources = []
    project_sources = []
    remove_funcs = []
    unwindset_parts = []
    extra_defines = []
    # Simple variable assignments for expansion (e.g. MAX_BLOB_SIZE = 3)
    local_vars = {}

    for line in makefile.read_text().splitlines():
        line = line.strip()

        # Simple variable assignment: VAR = value  or  VAR ?= value
        m = re.match(r'^(\w+)\s*\??=\s*(.+)$', line)
        if m and not any(line.startswith(kw) for kw in
                         ("PROOF_SOURCES", "PROJECT_SOURCES", "REMOVE_FUNCTION_BODY",
                          "UNWINDSET", "DEFINES", "CHECKFLAGS")):
            local_vars[m.group(1)] = m.group(2).strip()
            continue

        m = re.match(r'^PROOF_SOURCES\s*\+=\s*(.+)$', line)
        if m:
            resolved = _resolve_makefile_var(m.group(1), local_vars)
            if resolved is not None:
                proof_sources.append(resolved)
            continue

        m = re.match(r'^PROJECT_SOURCES\s*\+=\s*(.+)$', line)
        if m:
            resolved = _resolve_makefile_var(m.group(1), local_vars)
            if resolved is not None:
                project_sources.append(resolved)
            continue

        m = re.match(r'^REMOVE_FUNCTION_BODY\s*\+=\s*(.+)$', line)
        if m:
            remove_funcs.append(m.group(1).strip())
            continue

        m = re.match(r'^UNWINDSET\s*\+=\s*(.+)$', line)
        if m:
            unwindset_parts.append(m.group(1).strip())
            continue

        m = re.match(r'^DEFINES\s*\+=\s*(.+)$', line)
        if m:
            flag = _expand_make_vars(m.group(1).strip(), local_vars)
            extra_defines.append(flag)
            continue

    return {
        "proof_sources": proof_sources,
        "project_sources": project_sources,
        "remove_function_body": remove_funcs,
        "unwindset": ",".join(unwindset_parts),
        "extra_defines": extra_defines,
    }


@dataclass
class CBMCResult:
    func_name: str
    harness_path: str
    returncode: int
    stdout: str
    stderr: str
    compilation_ok: bool
    verification_result: str   # "SUCCESS", "FAIL", "UNKNOWN", "TIMEOUT", "COMPILE_ERROR"
    num_checks: int
    num_failed: int
    error_summary: str         # human-readable for LLM feedback


def _run_subprocess(cmd, timeout, stdin=None):
    """Run subprocess and return (rc, stdout, stderr)."""
    try:
        proc = subprocess.run(
            cmd, capture_output=True, text=True,
            timeout=timeout, stdin=stdin,
        )
        return proc.returncode, proc.stdout, proc.stderr
    except subprocess.TimeoutExpired:
        return -999, "", "SUBPROCESS_TIMEOUT"


def run_cbmc(func_name: str, harness_path: Path, timeout: int = 240) -> CBMCResult:
    """
    Run CBMC on a harness (3-step: goto-cc → goto-instrument → cbmc).
    """
    cfg = parse_proof_makefile(func_name)

    all_sources = (
        cfg["proof_sources"] +
        [harness_path] +
        cfg["project_sources"]
    )

    # ── Step 1: goto-cc compilation ──────────────────────────────────────────
    with tempfile.NamedTemporaryFile(suffix=".goto", delete=False) as f:
        goto_bin = Path(f.name)
    with tempfile.NamedTemporaryFile(suffix="_instrumented.goto", delete=False) as f:
        goto_instrumented = Path(f.name)

    try:
        # goto-cc: compile all sources to a single goto binary
        goto_cc_cmd = (
            ["goto-cc",
             "--export-file-local-symbols",
             "--function", f"{func_name}_harness",
             "-o", str(goto_bin),
             ] +
            COMPILE_INCLUDES +
            COMPILE_DEFINES +
            cfg.get("extra_defines", []) +
            [str(s) for s in all_sources]
        )
        rc_cc, out_cc, err_cc = _run_subprocess(goto_cc_cmd, timeout // 3)

        if rc_cc == -999:
            return CBMCResult(
                func_name=func_name, harness_path=str(harness_path),
                returncode=-1, stdout="", stderr="TIMEOUT (goto-cc)",
                compilation_ok=False, verification_result="TIMEOUT",
                num_checks=0, num_failed=0,
                error_summary=f"goto-cc timed out",
            )

        # Detect compilation errors
        all_cc_output = out_cc + err_cc
        compilation_ok = (
            rc_cc == 0 and
            "PARSING ERROR"    not in all_cc_output and
            "CONVERSION ERROR" not in all_cc_output and
            "fatal error"      not in err_cc.lower() and
            "error:"           not in err_cc.lower() or
            # goto-cc sometimes exits 0 even on soft warnings
            (rc_cc == 0 and "error:" not in err_cc.lower() and "PARSING ERROR" not in all_cc_output)
        )
        compilation_ok = (rc_cc == 0 and
                          "PARSING ERROR" not in all_cc_output and
                          "CONVERSION ERROR" not in all_cc_output and
                          ": error:" not in err_cc)

        if not compilation_ok:
            lines = (out_cc + err_cc).split("\n")
            error_lines = [l for l in lines
                           if "error" in l.lower() or "PARSING" in l or "CONVERSION" in l]
            return CBMCResult(
                func_name=func_name, harness_path=str(harness_path),
                returncode=rc_cc, stdout=out_cc, stderr=err_cc,
                compilation_ok=False, verification_result="COMPILE_ERROR",
                num_checks=0, num_failed=0,
                error_summary="\n".join(error_lines[:10]),
            )

        # ── Step 2: goto-instrument ───────────────────────────────────────────
        # Apply proof-specific REMOVE_FUNCTION_BODY stubs, then standard
        # transformations that match Makefile.common:
        #   --remove-function-pointers  : resolve indirect calls
        #   --slice-global-inits        : prune irrelevant global initialisers
        #   --drop-unused-functions     : shrink the model for faster analysis
        # NOTE: do NOT use --remove-calls-no-body — it removes malloc/free and
        #       breaks CBMC's built-in heap models.
        gi_cmd = ["goto-instrument"]
        for fn in cfg["remove_function_body"]:
            gi_cmd += ["--remove-function-body", fn]
        gi_cmd += [
            "--remove-function-pointers",
            "--slice-global-inits",
            "--drop-unused-functions",
        ]
        gi_cmd += [str(goto_bin), str(goto_instrumented)]

        rc_gi, out_gi, err_gi = _run_subprocess(gi_cmd, timeout // 3)
        if rc_gi != 0:
            # Non-fatal: proceed with non-instrumented binary
            goto_instrumented = goto_bin

        # ── Step 3: cbmc analysis ─────────────────────────────────────────────
        unwind_flags = []
        if cfg["unwindset"]:
            unwind_flags = ["--unwindset", cfg["unwindset"]]

        cbmc_cmd = (
            ["cbmc"] +
            CBMC_ANALYSIS_FLAGS +
            unwind_flags +
            [str(goto_instrumented)]
        )
        rc_cbmc, out_cbmc, err_cbmc = _run_subprocess(cbmc_cmd, timeout)

        if rc_cbmc == -999:
            return CBMCResult(
                func_name=func_name, harness_path=str(harness_path),
                returncode=-1, stdout="", stderr="TIMEOUT (cbmc)",
                compilation_ok=True, verification_result="TIMEOUT",
                num_checks=0, num_failed=0,
                error_summary="CBMC timed out",
            )

        # Parse verification results
        verification_result = "UNKNOWN"
        num_checks = 0
        num_failed = 0
        error_summary = ""

        match = re.search(r'\*\* (\d+) of (\d+) failed', out_cbmc)
        if match:
            num_failed = int(match.group(1))
            num_checks = int(match.group(2))

        if "VERIFICATION SUCCESSFUL" in out_cbmc:
            verification_result = "SUCCESS"
        elif "VERIFICATION FAILED" in out_cbmc:
            verification_result = "FAIL"
            fail_lines = [l for l in out_cbmc.split("\n")
                          if "FAILED" in l or "failure" in l.lower()]
            error_summary = "\n".join(fail_lines[:10])

        return CBMCResult(
            func_name=func_name, harness_path=str(harness_path),
            returncode=rc_cbmc,
            stdout=out_cbmc, stderr=err_cbmc,
            compilation_ok=True,
            verification_result=verification_result,
            num_checks=num_checks, num_failed=num_failed,
            error_summary=error_summary,
        )

    finally:
        # Clean up temp files
        for p in [goto_bin, goto_instrumented]:
            try:
                p.unlink(missing_ok=True)
            except Exception:
                pass


def run_gt(func_name: str) -> CBMCResult:
    """Run CBMC on the ground truth harness."""
    harness = PROOFS_DIR / func_name / f"{func_name}_harness.c"
    return run_cbmc(func_name, harness)


if __name__ == "__main__":
    import sys
    func = sys.argv[1] if len(sys.argv) > 1 else "s2n_stuffer_init"
    print(f"Running GT harness for {func}...")
    r = run_gt(func)
    print(f"  Compilation: {'OK' if r.compilation_ok else 'FAIL'}")
    print(f"  Verification: {r.verification_result}")
    print(f"  Checks: {r.num_failed}/{r.num_checks} failed")
    if r.error_summary:
        print(f"  Errors:\n{r.error_summary}")
    if r.verification_result == "COMPILE_ERROR":
        print(f"  Stderr: {r.stderr[:500]}")
