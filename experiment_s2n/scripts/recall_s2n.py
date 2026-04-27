#!/usr/bin/env python3
"""
recall_s2n.py
=============
Compute postcondition recall for s2n-tls harnesses.

For each target function, extract CBMC verification properties from:
  - GT harness (ground truth)
  - LLM-generated harness (best iteration)

Then fuzzy-match GT properties against LLM properties to compute recall.

Also computes a simpler "check coverage" metric: |LLM checks| / |GT checks|.

Usage:
    python recall_s2n.py --condition A
    python recall_s2n.py --condition E
    python recall_s2n.py --condition A --save
"""

import sys
import json
import re
import subprocess
import argparse
import xml.etree.ElementTree as ET
import tempfile
from pathlib import Path
from dataclasses import dataclass

script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
sys.path.insert(0, str(script_dir))

from cbmc_runner_s2n import (
    run_gt, run_cbmc, parse_proof_makefile,
    COMPILE_INCLUDES, COMPILE_DEFINES,
    PROOFS_DIR, S2N_DIR, CBMC_ANALYSIS_FLAGS
)
import cbmc_runner_s2n as _runner

RESULTS_DIR = experiment_dir / "results"
EVAL_DIR    = experiment_dir / "evaluation"
EVAL_DIR.mkdir(parents=True, exist_ok=True)

TARGET_FUNCS = [
    "s2n_stuffer_init",
    "s2n_stuffer_alloc",
    "s2n_stuffer_growable_alloc",
    "s2n_stuffer_free",
    "s2n_stuffer_resize",
    "s2n_stuffer_resize_if_empty",
    "s2n_stuffer_rewrite",
    "s2n_stuffer_rewind_read",
    "s2n_stuffer_wipe",
    "s2n_stuffer_wipe_n",
    "s2n_stuffer_skip_read",
    "s2n_stuffer_skip_write",
    "s2n_stuffer_read",
    "s2n_stuffer_write",
    "s2n_stuffer_read_bytes",
    "s2n_stuffer_write_bytes",
    "s2n_stuffer_erase_and_read",
    "s2n_stuffer_read_uint8",
    "s2n_stuffer_write_uint8",
    "s2n_stuffer_read_uint32",
    "s2n_stuffer_write_uint32",
    "s2n_stuffer_copy",
    "s2n_stuffer_extract_blob",
    "s2n_stuffer_reserve_space",
    "s2n_stuffer_is_consumed",
]


def _run_goto_pipeline(func_name: str, harness_path: Path, timeout: int = 120) -> Path | None:
    """
    Run goto-cc + goto-instrument to produce an instrumented goto binary.
    Returns path to .goto file on success, None on failure.
    """
    cfg = parse_proof_makefile(func_name)
    all_sources = cfg["proof_sources"] + [harness_path] + cfg["project_sources"]

    with tempfile.NamedTemporaryFile(suffix=".goto", delete=False) as f:
        goto_bin = Path(f.name)
    with tempfile.NamedTemporaryFile(suffix="_instr.goto", delete=False) as f:
        goto_instr = Path(f.name)

    try:
        # Step 1: goto-cc
        cc_cmd = (
            ["goto-cc", "--export-file-local-symbols",
             "--function", f"{func_name}_harness",
             "-o", str(goto_bin)] +
            COMPILE_INCLUDES +
            COMPILE_DEFINES +
            cfg.get("extra_defines", []) +
            [str(s) for s in all_sources]
        )
        proc = subprocess.run(cc_cmd, capture_output=True, text=True, timeout=timeout // 2)
        if proc.returncode != 0 or ": error:" in proc.stderr:
            goto_bin.unlink(missing_ok=True)
            goto_instr.unlink(missing_ok=True)
            return None

        # Step 2: goto-instrument
        gi_cmd = ["goto-instrument"]
        for fn in cfg["remove_function_body"]:
            gi_cmd += ["--remove-function-body", fn]
        gi_cmd += [
            "--remove-function-pointers",
            "--slice-global-inits",
            "--drop-unused-functions",
            str(goto_bin), str(goto_instr),
        ]
        subprocess.run(gi_cmd, capture_output=True, timeout=timeout // 2)

        goto_bin.unlink(missing_ok=True)
        return goto_instr

    except (subprocess.TimeoutExpired, Exception):
        goto_bin.unlink(missing_ok=True)
        goto_instr.unlink(missing_ok=True)
        return None


def get_properties(func_name: str, harness_path: Path,
                   timeout: int = 120) -> list[dict]:
    """
    Extract verification properties from a harness via goto pipeline + cbmc --show-properties.
    Returns list of dicts: {id, description, class, expression, file, line}
    Only properties whose file contains "harness" are returned.
    """
    cfg = parse_proof_makefile(func_name)
    unwind_flags = ["--unwindset", cfg["unwindset"]] if cfg["unwindset"] else []

    goto_instr = _run_goto_pipeline(func_name, harness_path, timeout)
    if goto_instr is None:
        return []

    try:
        cbmc_cmd = (
            ["cbmc"] +
            CBMC_ANALYSIS_FLAGS +
            unwind_flags +
            ["--show-properties", "--xml-ui", str(goto_instr)]
        )
        proc = subprocess.run(cbmc_cmd, capture_output=True, text=True, timeout=timeout)
        xml_text = proc.stdout

        # Parse XML
        props = []
        m = re.search(r'<cprover>.*?</cprover>', xml_text, re.DOTALL)
        if not m:
            return []
        root = ET.fromstring(m.group(0))
        for prop in root.findall("property"):
            desc_el = prop.find("description")
            expr_el = prop.find("expression")
            loc_el  = prop.find("location")
            file_attr = loc_el.get("file", "") if loc_el is not None else ""
            p = {
                "id":          prop.get("name", ""),
                "description": desc_el.text.strip() if desc_el is not None and desc_el.text else "",
                "class":       prop.get("class", ""),
                "expression":  expr_el.text.strip() if expr_el is not None and expr_el.text else "",
                "file":        file_attr,
                "line":        loc_el.get("line", "") if loc_el is not None else "",
            }
            # Only include assertions originating from the harness (not library internals)
            if "harness" in file_attr.lower() or p["class"] == "assertion":
                props.append(p)
        return props

    except (subprocess.TimeoutExpired, Exception):
        return []
    finally:
        if goto_instr:
            goto_instr.unlink(missing_ok=True)


def normalize_prop(desc: str) -> str:
    """Normalize property description for matching."""
    s = desc.lower().strip()
    # Remove harness function name prefix
    s = re.sub(r'\bs2n_\w+_harness\b', '_harness_', s)
    # Normalize pointer notation
    s = s.replace('->', '.')
    # Normalize whitespace
    s = re.sub(r'\s+', ' ', s)
    # Remove common prefixes
    s = re.sub(r'^(assertion|assert)\s*', '', s)
    # Normalize old-state naming
    s = re.sub(r'\b([a-z][a-z0-9]*)_old\b', r'old_\1', s)
    return s.strip()


def normalize_prop_fuzzy(desc: str) -> str:
    """More aggressive normalization — strip local variable name prefixes."""
    s = normalize_prop(desc)
    # Strip simple local variable name before a field access, preserving old_X forms
    s = re.sub(r'\b(?!old_)[a-z][a-z0-9_]*\.', '_v_.', s)
    return s


def match_properties(gt_props: list[dict], llm_props: list[dict]) -> dict:
    """
    Compute recall: fraction of GT properties matched by LLM properties.
    Two-pass: strict then fuzzy.
    """
    if not gt_props:
        return {"gt_count": 0, "llm_count": len(llm_props),
                "matched_count": 0, "matched_count_fuzzy": 0,
                "recall": 0.0, "recall_fuzzy": 0.0}

    gt_keys       = [normalize_prop(p["description"]) + "|" + p.get("class","") for p in gt_props]
    gt_keys_fuzzy = [normalize_prop_fuzzy(p["description"]) + "|" + p.get("class","") for p in gt_props]
    llm_keys      = set(normalize_prop(p["description"]) + "|" + p.get("class","") for p in llm_props)
    llm_keys_fuzzy= set(normalize_prop_fuzzy(p["description"]) + "|" + p.get("class","") for p in llm_props)

    matched       = sum(1 for k in gt_keys if k in llm_keys)
    matched_fuzzy = sum(1 for k in gt_keys_fuzzy if k in llm_keys_fuzzy)

    return {
        "gt_count":          len(gt_props),
        "llm_count":         len(llm_props),
        "matched_count":     matched,
        "matched_count_fuzzy": matched_fuzzy,
        "recall":            matched / len(gt_props),
        "recall_fuzzy":      matched_fuzzy / len(gt_props),
    }


def best_harness_path(func_name: str, cond_dir: Path) -> Path | None:
    """Return the harness with highest iteration number that compiled OK."""
    func_dir = cond_dir / func_name
    if not func_dir.exists():
        return None
    summary_path = func_dir / "summary.json"
    if not summary_path.exists():
        return None
    summary = json.loads(summary_path.read_text())
    # Find last iteration that compiled OK, preferring SUCCESS
    best = None
    for it in reversed(summary.get("iterations", [])):
        if it["verify"] == "SUCCESS":
            best = func_dir / f"iter_{it['iter']}_harness.c"
            break
    if best is None:
        for it in reversed(summary.get("iterations", [])):
            if it["compile_ok"]:
                best = func_dir / f"iter_{it['iter']}_harness.c"
                break
    return best if best and best.exists() else None


def compute_recall_for_condition(condition: str) -> list[dict]:
    """Compute recall for all functions in the given condition."""
    model_suffix = ""  # default qwen
    cond_dir = RESULTS_DIR / f"s2n_cond{condition}{model_suffix}"
    if not cond_dir.exists():
        print(f"No results for condition {condition} at {cond_dir}")
        return []

    rows = []
    for func in TARGET_FUNCS:
        print(f"  {func}...", end=" ", flush=True)

        # GT harness path
        gt_path = PROOFS_DIR / func / f"{func}_harness.c"
        if not gt_path.exists():
            print(f"[GT MISSING]")
            continue

        # Get GT properties
        gt_props = get_properties(func, gt_path)
        if not gt_props:
            print(f"[GT props empty]")
            gt_props = []  # still proceed

        # Get LLM best harness
        llm_path = best_harness_path(func, cond_dir)
        if llm_path is None:
            print(f"[NO LLM HARNESS]")
            rows.append({"func": func, "condition": condition,
                         "gt_count": len(gt_props), "llm_count": 0,
                         "matched": 0, "recall_fuzzy": 0.0,
                         "converged": False, "note": "no harness"})
            continue

        # Get LLM properties
        llm_props = get_properties(func, llm_path)
        m = match_properties(gt_props, llm_props)

        # Convergence info
        summary_path = cond_dir / func / "summary.json"
        converged = False
        if summary_path.exists():
            summary = json.loads(summary_path.read_text())
            converged = summary.get("converged", False)

        row = {
            "func":          func,
            "condition":     condition,
            "gt_count":      m["gt_count"],
            "llm_count":     m["llm_count"],
            "matched":       m["matched_count_fuzzy"],
            "recall_fuzzy":  m["recall_fuzzy"],
            "converged":     converged,
            "note":          "",
        }
        rows.append(row)
        print(f"recall={m['recall_fuzzy']:.0%} ({m['matched_count_fuzzy']}/{m['gt_count']}) "
              f"conv={'Y' if converged else 'N'}")

    return rows


def print_summary(rows: list[dict]):
    n = len(rows)
    if not n:
        return
    recall_vals = [r["recall_fuzzy"] for r in rows]
    mean_recall = sum(recall_vals) / n
    conv_rate   = sum(1 for r in rows if r["converged"]) / n
    print(f"\n{'='*60}")
    print(f"Condition {rows[0]['condition']:5s}  n={n}")
    print(f"  Mean recall (fuzzy): {mean_recall:.1%}")
    print(f"  Convergence rate:    {conv_rate:.0%}")
    print(f"\n  {'Function':45s} {'GT':4s} {'LLM':4s} {'Rec':5s} {'Conv':4s}")
    print(f"  {'-'*60}")
    for r in sorted(rows, key=lambda x: -x["recall_fuzzy"]):
        print(f"  {r['func']:45s} {r['gt_count']:4d} {r['llm_count']:4d} "
              f"{r['recall_fuzzy']:5.0%} {'Y' if r['converged'] else 'N'}")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--condition", choices=["A", "E"], required=True)
    parser.add_argument("--save", action="store_true")
    args = parser.parse_args()

    print(f"Computing s2n-tls recall for condition {args.condition}...")
    rows = compute_recall_for_condition(args.condition)
    print_summary(rows)

    if args.save and rows:
        out = EVAL_DIR / f"s2n_recall_cond{args.condition}.json"
        out.write_text(json.dumps(rows, indent=2))
        print(f"\nSaved to {out}")


if __name__ == "__main__":
    main()
