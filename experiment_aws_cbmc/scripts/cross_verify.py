#!/usr/bin/env python3
"""
cross_verify.py - Property-level cross-verification for LLM-generated CBMC harnesses.

Uses CBMC --show-properties to enumerate verification properties from each harness,
then compares property sets between GT and LLM harnesses.

Two metrics:
  1. Property recall: fraction of GT properties matched in LLM harness
  2. Verification equivalence: do both harnesses pass/fail CBMC?

Classification:
  STRONG_EQUIV       - verification equivalent AND high property recall (>=80%)
  WEAK_EQUIV         - verification equivalent but lower property recall
  VERIFY_EQUIV_ONLY  - verification equivalent, very low property recall
  LLM_STRONGER       - LLM has more/different properties, GT fails in LLM harness
  NOT_EQUIV          - verification not equivalent

Usage:
    python cross_verify.py --func aws_array_list_back
    python cross_verify.py --all
    python cross_verify.py --all --save-json
"""

import re
import sys
import json
import subprocess
import argparse
import tempfile
import xml.etree.ElementTree as ET
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
sys.path.insert(0, str(script_dir))

from cbmc_runner import run_cbmc, run_gt, CBMCResult, PROOFDIR, FUNC_CONFIGS, COMMON_FLAGS, SRCDIR

DATASET_DIR = experiment_dir / "dataset"
RESULTS_DIR = experiment_dir / "results"
EVAL_DIR = experiment_dir / "evaluation"

PILOT_FUNCTIONS = [
    ("func1_aws_add_size_checked",      "aws_add_size_checked"),
    ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    ("func3_aws_array_list_back",       "aws_array_list_back"),
    ("func4_aws_byte_buf_append",       "aws_byte_buf_append"),
    ("func5_aws_linked_list_push_back", "aws_linked_list_push_back"),
    # Extended set (25 more)
    ("func6_aws_add_size_saturating",      "aws_add_size_saturating"),
    ("func7_aws_mul_size_checked",         "aws_mul_size_checked"),
    ("func8_aws_mul_size_saturating",      "aws_mul_size_saturating"),
    ("func9_aws_is_power_of_two",          "aws_is_power_of_two"),
    ("func10_aws_round_up_to_power_of_two","aws_round_up_to_power_of_two"),
    ("func11_aws_byte_buf_clean_up",       "aws_byte_buf_clean_up"),
    ("func12_aws_byte_buf_secure_zero",    "aws_byte_buf_secure_zero"),
    ("func13_aws_byte_buf_reset",          "aws_byte_buf_reset"),
    ("func14_aws_byte_buf_from_array",     "aws_byte_buf_from_array"),
    ("func15_aws_byte_buf_from_empty_array","aws_byte_buf_from_empty_array"),
    ("func17_aws_byte_buf_eq",             "aws_byte_buf_eq"),
    ("func18_aws_byte_cursor_advance",     "aws_byte_cursor_advance"),
    ("func19_aws_array_list_length",       "aws_array_list_length"),
    ("func20_aws_array_list_capacity",     "aws_array_list_capacity"),
    ("func21_aws_array_list_front",        "aws_array_list_front"),
    ("func22_aws_array_list_clear",        "aws_array_list_clear"),
    ("func23_aws_array_list_pop_back",     "aws_array_list_pop_back"),
    ("func24_aws_array_list_push_back",    "aws_array_list_push_back"),
    ("func25_aws_array_list_get_at",       "aws_array_list_get_at"),
    ("func26_aws_linked_list_push_front",  "aws_linked_list_push_front"),
    ("func27_aws_linked_list_pop_back",    "aws_linked_list_pop_back"),
    ("func28_aws_linked_list_pop_front",   "aws_linked_list_pop_front"),
    ("func29_aws_linked_list_init",        "aws_linked_list_init"),
    ("func30_aws_linked_list_node_reset",  "aws_linked_list_node_reset"),
    # Extended set (71 more — full aws-c-common coverage)
    ("func31_aws_array_eq",                   "aws_array_eq"),
    ("func32_aws_array_eq_c_str",             "aws_array_eq_c_str"),
    ("func33_aws_array_list_clean_up",        "aws_array_list_clean_up"),
    ("func34_aws_array_list_erase",           "aws_array_list_erase"),
    ("func35_aws_array_list_get_at_ptr",      "aws_array_list_get_at_ptr"),
    ("func36_aws_array_list_init_dynamic",    "aws_array_list_init_dynamic"),
    ("func37_aws_array_list_init_static",     "aws_array_list_init_static"),
    ("func38_aws_array_list_pop_front",       "aws_array_list_pop_front"),
    ("func39_aws_array_list_pop_front_n",     "aws_array_list_pop_front_n"),
    ("func40_aws_array_list_push_front",      "aws_array_list_push_front"),
    ("func41_aws_array_list_set_at",          "aws_array_list_set_at"),
    ("func42_aws_array_list_swap",            "aws_array_list_swap"),
    ("func43_aws_array_list_swap_contents",   "aws_array_list_swap_contents"),
    ("func44_aws_byte_buf_advance",           "aws_byte_buf_advance"),
    ("func46_aws_byte_buf_cat",               "aws_byte_buf_cat"),
    ("func47_aws_byte_buf_clean_up_secure",   "aws_byte_buf_clean_up_secure"),
    ("func48_aws_byte_buf_eq_c_str",          "aws_byte_buf_eq_c_str"),
    ("func49_aws_byte_buf_from_c_str",        "aws_byte_buf_from_c_str"),
    ("func50_aws_byte_buf_init_copy",         "aws_byte_buf_init_copy"),
    ("func51_aws_byte_buf_init_copy_from_cursor", "aws_byte_buf_init_copy_from_cursor"),
    ("func60_aws_byte_cursor_eq",             "aws_byte_cursor_eq"),
    ("func61_aws_byte_cursor_eq_byte_buf",    "aws_byte_cursor_eq_byte_buf"),
    ("func62_aws_byte_cursor_eq_c_str",       "aws_byte_cursor_eq_c_str"),
    ("func63_aws_byte_cursor_from_array",     "aws_byte_cursor_from_array"),
    ("func64_aws_byte_cursor_from_buf",       "aws_byte_cursor_from_buf"),
    ("func65_aws_byte_cursor_from_c_str",     "aws_byte_cursor_from_c_str"),
    ("func66_aws_byte_cursor_from_string",    "aws_byte_cursor_from_string"),
    ("func73_aws_linked_list_back",           "aws_linked_list_back"),
    ("func74_aws_linked_list_begin",          "aws_linked_list_begin"),
    ("func75_aws_linked_list_end",            "aws_linked_list_end"),
    ("func76_aws_linked_list_front",          "aws_linked_list_front"),
    ("func77_aws_linked_list_insert_after",   "aws_linked_list_insert_after"),
    ("func78_aws_linked_list_insert_before",  "aws_linked_list_insert_before"),
    ("func79_aws_linked_list_next",           "aws_linked_list_next"),
    ("func80_aws_linked_list_prev",           "aws_linked_list_prev"),
    ("func81_aws_linked_list_rbegin",         "aws_linked_list_rbegin"),
    ("func82_aws_linked_list_rend",           "aws_linked_list_rend"),
    ("func83_aws_linked_list_remove",         "aws_linked_list_remove"),
    ("func84_aws_linked_list_swap_contents",  "aws_linked_list_swap_contents"),
    ("func85_aws_nospec_mask",                "aws_nospec_mask"),
    ("func86_aws_ptr_eq",                     "aws_ptr_eq"),
    ("func87_aws_ring_buffer_buf_belongs_to_pool", "aws_ring_buffer_buf_belongs_to_pool"),
    ("func88_aws_ring_buffer_clean_up",       "aws_ring_buffer_clean_up"),
    ("func89_aws_ring_buffer_init",           "aws_ring_buffer_init"),
    ("func90_aws_ring_buffer_release",        "aws_ring_buffer_release"),
    ("func91_aws_string_bytes",               "aws_string_bytes"),
    ("func92_aws_string_destroy",             "aws_string_destroy"),
    ("func93_aws_string_destroy_secure",      "aws_string_destroy_secure"),
    ("func94_aws_string_eq",                  "aws_string_eq"),
    ("func95_aws_string_eq_byte_buf",         "aws_string_eq_byte_buf"),
    ("func96_aws_string_eq_byte_cursor",      "aws_string_eq_byte_cursor"),
    ("func97_aws_string_eq_c_str",            "aws_string_eq_c_str"),
    ("func99_aws_string_new_from_c_str",      "aws_string_new_from_c_str"),
    ("func101_aws_ring_buffer_acquire",       "aws_ring_buffer_acquire"),
]

FEEDBACK_DIR = RESULTS_DIR / "feedback_loop"   # default; overridden by --condition

CONDITION_FEEDBACK_DIR = {
    "original": RESULTS_DIR / "feedback_loop",
    "A":        RESULTS_DIR / "feedback_loop_A",
    "B":        RESULTS_DIR / "feedback_loop_B",
    "A_v3":     RESULTS_DIR / "feedback_loop_A_v3",
    "B_v3":     RESULTS_DIR / "feedback_loop_B_v3",
    "A_claude": RESULTS_DIR / "feedback_loop_A_claude",
    "B_claude": RESULTS_DIR / "feedback_loop_B_claude",
    "C_claude": RESULTS_DIR / "feedback_loop_C_claude",
    "D_claude": RESULTS_DIR / "feedback_loop_D_claude",
    "E_claude": RESULTS_DIR / "feedback_loop_E_claude",
    "F_claude": RESULTS_DIR / "feedback_loop_F_claude",
    "A_gptoss120b": RESULTS_DIR / "feedback_loop_A_gptoss120b",
    "G_gptoss120b": RESULTS_DIR / "feedback_loop_G_gptoss120b",
    "H_gptoss120b": RESULTS_DIR / "feedback_loop_H_gptoss120b",
    "E_qwen":   RESULTS_DIR / "feedback_loop_E",
    "C_qwen":   RESULTS_DIR / "feedback_loop_C",
    "D_qwen":   RESULTS_DIR / "feedback_loop_D",
    "A_llama3370binstruct": RESULTS_DIR / "feedback_loop_A_llama3370binstruct",
    "G_llama3370binstruct": RESULTS_DIR / "feedback_loop_G_llama3370binstruct",
    "H_llama3370binstruct": RESULTS_DIR / "feedback_loop_H_llama3370binstruct",
    "I_gptoss120b":    RESULTS_DIR / "feedback_loop_I_gptoss120b",
    "J_gptoss120b":    RESULTS_DIR / "feedback_loop_J_gptoss120b",
    "K_gptoss120b":    RESULTS_DIR / "feedback_loop_K_gptoss120b",
    "Oracle_gptoss120b": RESULTS_DIR / "feedback_loop_Oracle_gptoss120b",
}


def get_properties(func_name: str, harness_path: Path, timeout: int = 60) -> tuple[list[dict], list[dict]]:
    """
    Run CBMC --show-properties --xml-ui to enumerate all verification properties.

    Returns (all_props, harness_props) where harness_props are only those
    originating from the harness file itself (not library internals).
    Each prop dict: {id, description, class, expression, file, line}
    """
    cfg = FUNC_CONFIGS.get(func_name)
    if cfg is None:
        raise ValueError(f"No config for function: {func_name}")

    all_sources = (
        cfg["proof_sources"] +
        [harness_path] +
        cfg["project_sources"]
    )

    cmd = (
        ["cbmc"] +
        COMMON_FLAGS +
        cfg["defines"] +
        ["--show-properties", "--xml-ui"] +
        ["--function", f"{func_name}_harness"] +
        [str(s) for s in all_sources]
    )

    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return [], []

    # Check for compilation error
    if "PARSING ERROR" in proc.stdout or "CONVERSION ERROR" in proc.stdout \
            or "fatal error" in proc.stderr:
        return [], []

    all_props = []
    harness_props = []
    harness_stem = harness_path.stem.lower()  # e.g. "iter_2_harness" or "aws_byte_buf_init_harness"

    try:
        xml_text = proc.stdout
        m = re.search(r'<cprover>.*?</cprover>', xml_text, re.DOTALL)
        if not m:
            return [], []
        root = ET.fromstring(m.group(0))
        for prop in root.findall("property"):
            desc_el = prop.find("description")
            expr_el = prop.find("expression")
            loc_el = prop.find("location")
            file_attr = loc_el.get("file", "") if loc_el is not None else ""
            p = {
                "id": prop.get("name", ""),
                "description": desc_el.text.strip() if desc_el is not None and desc_el.text else "",
                "class": prop.get("class", ""),
                "expression": expr_el.text.strip() if expr_el is not None and expr_el.text else "",
                "file": file_attr,
                "line": loc_el.get("line", "") if loc_el is not None else "",
            }
            all_props.append(p)
            # Harness-originated: file contains "harness" in its path
            if "harness" in file_attr.lower():
                harness_props.append(p)
    except ET.ParseError:
        pass

    return all_props, harness_props


def normalize_prop(desc: str) -> str:
    """
    Normalize a property description for strict matching.
    Handles: harness name prefix, pointer notation, old-state variable naming,
    and validity-predicate argument names.
    """
    s = desc.lower().strip()
    # Remove harness function name in assertion descriptions
    s = re.sub(r'\b(aws_\w+_harness)\b', '_harness_', s)
    # Normalize pointer/arrow notation
    s = s.replace('->', '.')
    # Normalize whitespace
    s = re.sub(r'\s+', ' ', s)
    # Remove common noisy prefixes
    s = re.sub(r'^assertion\s*', '', s)
    s = re.sub(r'^assert\s*', '', s)
    # Normalize old-state variable naming: X_old → old_X
    # e.g. "to_old.len" → "old_to.len"  so both LLM and GT use same form
    s = re.sub(r'\b([a-z][a-z0-9]*)_old\b', r'old_\1', s)
    # Normalize validity-predicate argument: aws_XXX_is_valid(&buf) == aws_XXX_is_valid(&byte_buf)
    # Replace &identifier inside is_valid(...) with &_arg_
    s = re.sub(r'(?<=_is_valid\()&[a-z]\w*', '&_arg_', s)
    return s.strip()


def normalize_prop_fuzzy(desc: str) -> str:
    """
    More aggressive normalization for a second-pass fuzzy match.
    Additionally strips local variable names in struct field access,
    e.g. "buf.len == 0" and "byte_buf.len == 0" both become "_v_.len == 0".
    Does NOT strip old_X prefixes so old-state semantics are preserved.
    """
    s = normalize_prop(desc)
    # Strip simple local variable name before a field access, but preserve old_X forms
    s = re.sub(r'\b(?!old_)[a-z][a-z0-9_]*\.', '_v_.', s)
    return s


def prop_key(prop: dict) -> str:
    """Strict matching key."""
    return normalize_prop(prop["description"]) + "|" + prop.get("class", "")


def prop_key_fuzzy(prop: dict) -> str:
    """Fuzzy matching key (strips local variable name prefixes)."""
    return normalize_prop_fuzzy(prop["description"]) + "|" + prop.get("class", "")


def match_properties(gt_props: list[dict], llm_props: list[dict]) -> dict:
    """
    Match GT properties against LLM properties.
    Two-pass:
      Pass 1 (strict)  – normalized description match (handles old_X naming, &_arg_)
      Pass 2 (fuzzy)   – additionally strips local variable name prefixes
    Returns match statistics for both passes.
    """
    gt_keys        = [prop_key(p)       for p in gt_props]
    gt_keys_fuzzy  = [prop_key_fuzzy(p) for p in gt_props]
    llm_keys       = set(prop_key(p)       for p in llm_props)
    llm_keys_fuzzy = set(prop_key_fuzzy(p) for p in llm_props)

    # Pass 1 – strict
    matched_strict   = [k for k in gt_keys if k in llm_keys]
    unmatched_strict = [k for k in gt_keys if k not in llm_keys]

    # Pass 2 – fuzzy (only for properties not already matched in pass 1)
    # A GT property counts as fuzzy-matched if its fuzzy key appears in LLM fuzzy keys
    matched_fuzzy = [k for k in gt_keys_fuzzy if k in llm_keys_fuzzy]
    unmatched_fuzzy = [k for k in gt_keys_fuzzy if k not in llm_keys_fuzzy]

    # LLM-only in strict (for assume adequacy and precision)
    gt_key_set = set(gt_keys)
    llm_only = [k for k in (prop_key(p) for p in llm_props) if k not in gt_key_set]

    n_gt  = len(gt_props)
    n_llm = len(llm_props)
    n_strict = len(matched_strict)
    n_fuzzy  = len(matched_fuzzy)

    return {
        "gt_count":          n_gt,
        "llm_count":         n_llm,
        # Strict (primary metric, backward compatible)
        "matched_count":     n_strict,
        "unmatched_gt":      unmatched_strict,
        "llm_only":          list(set(llm_only)),
        "recall":            n_strict / n_gt  if n_gt  else 0.0,
        "precision":         n_strict / n_llm if n_llm else 0.0,
        # Fuzzy (accounts for variable naming differences)
        "matched_count_fuzzy": n_fuzzy,
        "recall_fuzzy":      n_fuzzy / n_gt  if n_gt  else 0.0,
        "precision_fuzzy":   n_fuzzy / n_llm if n_llm else 0.0,
        "unmatched_gt_fuzzy": unmatched_fuzzy,
    }


def strip_comments(code: str) -> str:
    code = re.sub(r'//[^\n]*', '', code)
    code = re.sub(r'/\*.*?\*/', '', code, flags=re.DOTALL)
    return code


def extract_assume_exprs(code: str) -> list[str]:
    """Return the expression strings inside every __CPROVER_assume(...) call."""
    clean = strip_comments(code)
    # Handles single-line assumes; multi-line unlikely in harnesses
    assumes = re.findall(r'__CPROVER_assume\s*\((.+?)\)\s*;', clean, re.DOTALL)
    return [re.sub(r'\s+', ' ', a.strip()) for a in assumes]


def normalize_assume(expr: str) -> str:
    """Normalise an assume expression for fuzzy comparison."""
    s = expr.lower().strip()
    s = re.sub(r'\s+', ' ', s)
    s = s.replace('->', '.')          # pointer vs dot
    s = re.sub(r'&(\w)', r'\1', s)   # &var -> var (address-of noise)
    return s


def build_weakened_harness(llm_code: str, gt_code: str) -> tuple[str, list[str]]:
    """
    Comment out __CPROVER_assume lines in the LLM harness that have no
    normalised match in the GT harness.

    Returns (weakened_code, list_of_stripped_expressions).
    """
    gt_exprs = extract_assume_exprs(gt_code)
    gt_norm  = set(normalize_assume(e) for e in gt_exprs)

    lines   = llm_code.split('\n')
    stripped = []

    for i, line in enumerate(lines):
        s = line.strip()
        if '__CPROVER_assume' not in s or s.startswith('//'):
            continue
        m = re.search(r'__CPROVER_assume\s*\((.+?)\)\s*;', s, re.DOTALL)
        if not m:
            continue
        expr = re.sub(r'\s+', ' ', m.group(1).strip())
        if normalize_assume(expr) not in gt_norm:
            # Prefix the line with a comment marker, preserve indentation
            indent = len(line) - len(line.lstrip())
            lines[i] = line[:indent] + '// [STRIPPED] ' + line[indent:]
            stripped.append(expr)

    return '\n'.join(lines), stripped


@dataclass
class CrossVerifyResult:
    func_name: str

    # Verification results
    gt_verify: str
    llm_verify: str

    # All-property counts (includes library internals - baseline)
    gt_all_count: int
    llm_all_count: int
    all_matched: int
    all_recall: float

    # Harness-level property counts (explicit assert() calls in harness)
    gt_harness_count: int
    llm_harness_count: int
    harness_matched: int
    harness_recall: float
    harness_precision: float
    # Fuzzy recall (also normalizes local variable name prefixes)
    harness_matched_fuzzy: int = 0
    harness_recall_fuzzy: float = 0.0
    harness_precision_fuzzy: float = 0.0

    # Assume stripping test
    gt_assume_count: int = 0
    llm_assume_count: int = 0
    llm_extra_assume_count: int = 0       # assumes in LLM but not in GT
    stripped_assumes: list = field(default_factory=list)
    weakened_verify: str = "N/A"          # CBMC result after stripping extra assumes
    over_constrained: bool = False        # True if removing extras breaks verification

    # Property details
    unmatched_gt_harness: list = field(default_factory=list)
    llm_only_harness: list = field(default_factory=list)

    def verification_equivalent(self) -> bool:
        return self.gt_verify == self.llm_verify

    def assume_adequacy(self) -> str:
        """
        TIGHT      - LLM has same assumes as GT (no extras)
        OVER       - LLM has extra assumes AND weakened harness breaks → over-constrained
        REDUNDANT  - LLM has extra assumes but weakened harness still passes → extras are harmless
        UNDER      - LLM has fewer assumes than GT (under-constrained)
        """
        gt_n, llm_n, extra = self.gt_assume_count, self.llm_assume_count, self.llm_extra_assume_count
        if extra == 0 and llm_n >= gt_n:
            return "TIGHT"
        elif extra == 0 and llm_n < gt_n:
            return "UNDER"
        elif extra > 0 and self.over_constrained:
            return "OVER"
        else:
            return "REDUNDANT"

    def classification(self) -> str:
        ve  = self.verification_equivalent()
        r   = self.harness_recall
        adeq = self.assume_adequacy()
        if ve and r >= 0.80 and adeq in ("TIGHT", "REDUNDANT"):
            return "STRONG_EQUIV"
        elif ve and r >= 0.80 and adeq == "OVER":
            return "CONSTRAINED_EQUIV"   # success only because of over-constraining
        elif ve and r >= 0.50:
            return "WEAK_EQUIV"
        elif ve:
            return "VERIFY_EQUIV_ONLY"
        else:
            if self.llm_verify == "SUCCESS" and self.gt_verify != "SUCCESS":
                return "LLM_STRONGER"
            return "NOT_EQUIV"

    def to_dict(self) -> dict:
        return {
            "func": self.func_name,
            "gt_verify": self.gt_verify,
            "llm_verify": self.llm_verify,
            "verification_equivalent": self.verification_equivalent(),
            "gt_all_count": self.gt_all_count,
            "llm_all_count": self.llm_all_count,
            "gt_harness_count": self.gt_harness_count,
            "llm_harness_count": self.llm_harness_count,
            "harness_matched": self.harness_matched,
            "harness_recall": round(self.harness_recall, 3),
            "harness_precision": round(self.harness_precision, 3),
            "harness_matched_fuzzy": self.harness_matched_fuzzy,
            "harness_recall_fuzzy": round(self.harness_recall_fuzzy, 3),
            "harness_precision_fuzzy": round(self.harness_precision_fuzzy, 3),
            "gt_assume_count": self.gt_assume_count,
            "llm_assume_count": self.llm_assume_count,
            "llm_extra_assume_count": self.llm_extra_assume_count,
            "stripped_assumes": self.stripped_assumes,
            "weakened_verify": self.weakened_verify,
            "over_constrained": self.over_constrained,
            "assume_adequacy": self.assume_adequacy(),
            "classification": self.classification(),
            "unmatched_gt_harness": self.unmatched_gt_harness[:5],
            "llm_only_harness": self.llm_only_harness[:5],
        }


def cross_verify_one(func_name: str, verbose: bool = True,
                     feedback_dir: Path = None) -> CrossVerifyResult:
    """Run cross-verification for one function."""
    if feedback_dir is None:
        feedback_dir = FEEDBACK_DIR
    gt_harness_path = PROOFDIR / "proofs" / func_name / f"{func_name}_harness.c"

    # Use the best LLM harness: feedback loop final iteration
    llm_dir = feedback_dir / func_name
    if not llm_dir.exists():
        raise FileNotFoundError(
            f"No feedback loop results for {func_name}. Run feedback_loop.py first."
        )

    iter_files = sorted(
        llm_dir.glob("iter_*_harness.c"),
        key=lambda p: int(p.stem.split('_')[1])
    )
    if not iter_files:
        return None  # skip functions with no harness files (e.g. K spec-first extraction failures)
    llm_harness_path = iter_files[-1]

    if verbose:
        print(f"  GT harness:  {gt_harness_path.name}")
        print(f"  LLM harness: {llm_harness_path.name}")

    # Run CBMC verification on both
    gt_result = run_gt(func_name)
    llm_result = run_cbmc(func_name, llm_harness_path)

    if verbose:
        print(f"  GT verify:   {gt_result.verification_result}")
        print(f"  LLM verify:  {llm_result.verification_result}")

    # Enumerate properties from both harnesses
    gt_all, gt_harness = get_properties(func_name, gt_harness_path)
    llm_all, llm_harness = get_properties(func_name, llm_harness_path)

    if verbose:
        print(f"  GT  all/harness: {len(gt_all)}/{len(gt_harness)}")
        print(f"  LLM all/harness: {len(llm_all)}/{len(llm_harness)}")

    # Match at harness level (the meaningful comparison)
    hmatch = match_properties(gt_harness, llm_harness)
    # Match at all-props level (baseline)
    amatch = match_properties(gt_all, llm_all)

    if verbose:
        print(f"  Harness-level strict: {hmatch['matched_count']}/{hmatch['gt_count']} "
              f"(recall={hmatch['recall']:.0%}, precision={hmatch['precision']:.0%})")
        print(f"  Harness-level fuzzy:  {hmatch['matched_count_fuzzy']}/{hmatch['gt_count']} "
              f"(recall={hmatch['recall_fuzzy']:.0%}, precision={hmatch['precision_fuzzy']:.0%})")
        if hmatch['unmatched_gt']:
            print(f"  Unmatched GT harness props (strict):")
            for k in hmatch['unmatched_gt'][:5]:
                print(f"    - {k}")
        if hmatch['llm_only']:
            print(f"  LLM-only harness props (extra assertions):")
            for k in hmatch['llm_only'][:5]:
                print(f"    + {k}")

    # ── Assume stripping test ────────────────────────────────────────────────
    gt_code  = gt_harness_path.read_text()
    llm_code = llm_harness_path.read_text()

    gt_assumes  = extract_assume_exprs(gt_code)
    llm_assumes = extract_assume_exprs(llm_code)
    weakened_code, stripped = build_weakened_harness(llm_code, gt_code)
    extra_count = len(stripped)

    if verbose:
        print(f"  Assumes GT/LLM:  {len(gt_assumes)}/{len(llm_assumes)}  "
              f"(extra in LLM: {extra_count})")
        for e in stripped:
            print(f"    [extra] {e}")

    weakened_verify = "N/A"
    over_constrained = False

    if extra_count > 0:
        with tempfile.NamedTemporaryFile(
            suffix='.c', mode='w', delete=False,
            prefix=f"{func_name}_weakened_"
        ) as f:
            f.write(weakened_code)
            weakened_path = Path(f.name)
        try:
            w_result = run_cbmc(func_name, weakened_path)
            weakened_verify = w_result.verification_result
            # Over-constrained: original passes but weakened does not
            over_constrained = (
                llm_result.verification_result == "SUCCESS" and
                w_result.verification_result != "SUCCESS"
            )
            if verbose:
                oc_tag = "  *** OVER-CONSTRAINED ***" if over_constrained else ""
                print(f"  Weakened verify: {weakened_verify}{oc_tag}")
        finally:
            weakened_path.unlink(missing_ok=True)
    else:
        weakened_verify = llm_result.verification_result  # nothing stripped
        if verbose:
            print(f"  No extra assumes → weakening not needed")

    return CrossVerifyResult(
        func_name=func_name,
        gt_verify=gt_result.verification_result,
        llm_verify=llm_result.verification_result,
        gt_all_count=amatch["gt_count"],
        llm_all_count=amatch["llm_count"],
        all_matched=amatch["matched_count"],
        all_recall=amatch["recall"],
        gt_harness_count=hmatch["gt_count"],
        llm_harness_count=hmatch["llm_count"],
        harness_matched=hmatch["matched_count"],
        harness_recall=hmatch["recall"],
        harness_precision=hmatch["precision"],
        harness_matched_fuzzy=hmatch["matched_count_fuzzy"],
        harness_recall_fuzzy=hmatch["recall_fuzzy"],
        harness_precision_fuzzy=hmatch["precision_fuzzy"],
        gt_assume_count=len(gt_assumes),
        llm_assume_count=len(llm_assumes),
        llm_extra_assume_count=extra_count,
        stripped_assumes=stripped,
        weakened_verify=weakened_verify,
        over_constrained=over_constrained,
        unmatched_gt_harness=hmatch["unmatched_gt"],
        llm_only_harness=hmatch["llm_only"],
    )


def print_table(results: list):
    # ── Assertion table ──────────────────────────────────────────────────────
    w = 120
    print(f"\n{'='*w}")
    print(f"{'CROSS-VERIFICATION: ASSERTION DIMENSION':^{w}}")
    print(f"{'='*w}")
    print(f"{'Function':<35} {'GT-Vfy':^8} {'LLM-Vfy':^8} {'GT-H':^5} {'LLM-H':^6} "
          f"{'Rec(S)':^8} {'Rec(F)':^8} {'Prec':^7} {'Class':<22}")
    print(f"{'-'*w}")
    for r in results:
        print(f"{r.func_name:<35} {r.gt_verify:<8} {r.llm_verify:<8} "
              f"{r.gt_harness_count:^5} {r.llm_harness_count:^6} "
              f"{r.harness_recall:^8.0%} {r.harness_recall_fuzzy:^8.0%} {r.harness_precision:^7.0%} "
              f"{r.classification():<22}")
    print(f"{'='*w}")

    # ── Assume table ─────────────────────────────────────────────────────────
    print(f"\n{'='*w}")
    print(f"{'CROSS-VERIFICATION: ASSUMPTION DIMENSION':^{w}}")
    print(f"{'='*w}")
    print(f"{'Function':<35} {'GT-Asm':^8} {'LLM-Asm':^9} {'Extra':^7} "
          f"{'Weakened-Vfy':^14} {'Over-Const':^12} {'Adequacy':<12}")
    print(f"{'-'*w}")
    for r in results:
        oc = "YES ***" if r.over_constrained else "no"
        print(f"{r.func_name:<35} {r.gt_assume_count:^8} {r.llm_assume_count:^9} "
              f"{r.llm_extra_assume_count:^7} {r.weakened_verify:^14} "
              f"{oc:^12} {r.assume_adequacy():<12}")
    print(f"{'='*w}")

    n = len(results)
    if n:
        ve       = sum(1 for r in results if r.verification_equivalent()) / n
        avg_rec  = sum(r.harness_recall for r in results) / n
        avg_prec = sum(r.harness_precision for r in results) / n
        strong   = sum(1 for r in results if r.classification() == "STRONG_EQUIV") / n
        oc_rate  = sum(1 for r in results if r.over_constrained) / n
        tight    = sum(1 for r in results if r.assume_adequacy() == "TIGHT") / n
        redund   = sum(1 for r in results if r.assume_adequacy() == "REDUNDANT") / n
        avg_rec_f = sum(r.harness_recall_fuzzy for r in results) / n
        print(f"\nAggregates (n={n}):")
        print(f"  Verification equivalent:              {ve:.0%}")
        print(f"  Avg assert recall strict (GT→LLM):   {avg_rec:.0%}")
        print(f"  Avg assert recall fuzzy  (GT→LLM):   {avg_rec_f:.0%}  (+{(avg_rec_f-avg_rec)*100:.1f}pp vs strict)")
        print(f"  Avg assert precision:                 {avg_prec:.0%}")
        print(f"  STRONG_EQUIV rate:                    {strong:.0%}")
        print(f"  Over-constrained (assume gaming):     {oc_rate:.0%}")
        print(f"  Assume TIGHT:                         {tight:.0%}")
        print(f"  Assume REDUNDANT (extra, harmless):   {redund:.0%}")


def _worker(args_tuple):
    func_name, feedback_dir, quiet = args_tuple
    try:
        r = cross_verify_one(func_name, verbose=not quiet, feedback_dir=feedback_dir)
        return r, None
    except Exception as e:
        import traceback
        return None, (func_name, traceback.format_exc())


def main():
    import concurrent.futures
    parser = argparse.ArgumentParser(description="Property-level cross-verification evaluation")
    parser.add_argument("--func", help="Function name (e.g. aws_array_list_back)")
    parser.add_argument("--all", action="store_true", help="Run all 30 functions")
    parser.add_argument("--save-json", action="store_true", help="Save results to JSON")
    parser.add_argument("--quiet", action="store_true", help="Suppress per-function verbose output")
    parser.add_argument("--workers", type=int, default=8, help="Parallel workers (default: 8)")
    parser.add_argument("--condition", choices=list(CONDITION_FEEDBACK_DIR.keys()), default="original",
                        help="Which feedback loop results to evaluate")
    args = parser.parse_args()

    active_feedback_dir = CONDITION_FEEDBACK_DIR[args.condition]
    print(f"Condition: {args.condition}  (reading from {active_feedback_dir})")

    funcs = PILOT_FUNCTIONS if args.all else None
    if args.func:
        funcs = [(d, n) for d, n in PILOT_FUNCTIONS if n == args.func]

    if not funcs:
        parser.print_help()
        sys.exit(1)

    func_names = [fn for _, fn in funcs]

    results = []
    if len(func_names) == 1 or args.workers == 1:
        for func_name in func_names:
            print(f"\n{'='*60}\n{func_name}")
            r, err = _worker((func_name, active_feedback_dir, args.quiet))
            if err:
                print(f"  ERROR: {err[1]}")
            else:
                results.append(r)
    else:
        print(f"Running {len(func_names)} functions with {args.workers} parallel workers...")
        work = [(fn, active_feedback_dir, True) for fn in func_names]
        with concurrent.futures.ProcessPoolExecutor(max_workers=args.workers) as ex:
            for r, err in ex.map(_worker, work):
                if err:
                    print(f"  ERROR ({err[0]}): {err[1][:200]}")
                elif r is None:
                    pass  # skipped (no harness files)
                else:
                    print(f"  done: {r.func_name}  verify_equiv={r.verification_equivalent()}  recall={r.harness_recall:.0%}")
                    results.append(r)
        results.sort(key=lambda r: func_names.index(r.func_name))

    if results:
        print_table(results)

    if args.save_json and results:
        EVAL_DIR.mkdir(parents=True, exist_ok=True)
        out = EVAL_DIR / f"cross_verify_results_cond{args.condition}.json"
        out.write_text(json.dumps([r.to_dict() for r in results], indent=2))
        print(f"\nSaved to: {out}")


if __name__ == "__main__":
    main()
