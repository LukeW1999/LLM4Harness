#!/usr/bin/env python3
"""
feedback_loop.py - CBMC-guided iterative harness refinement.

Pipeline:
  1. Generate initial harness (Prompt D)
  2. Run CBMC → get compilation/verification result
  3. If error: feed error message back to LLM → regenerate → repeat
  4. Stop when: verification SUCCESS, or max_iterations reached

Usage:
    python feedback_loop.py --func aws_array_list_back
    python feedback_loop.py --all
    python feedback_loop.py --func aws_byte_buf_append --max-iter 5
"""

import sys
import json
import argparse
from pathlib import Path
from dataclasses import dataclass, field
from typing import Optional

script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
sys.path.insert(0, str(script_dir))

from cbmc_runner import run_cbmc, run_gt, CBMCResult
import re as _re

# call_qwen is resolved at runtime by --model arg (see main())
call_qwen = None
read_file = None
get_function_name = None

def _load_model_backend(model: str):
    """Dynamically import the LLM backend and set module-level call_qwen."""
    global call_qwen, read_file, get_function_name
    if model == "claude":
        from call_claude_api import call_qwen as _cq, read_file as _rf, get_function_name as _gfn
    elif model == "openrouter":
        from call_openrouter_api import call_qwen as _cq, read_file as _rf, get_function_name as _gfn
    else:
        from call_qwen_api import call_qwen as _cq, read_file as _rf, get_function_name as _gfn
    call_qwen = _cq
    read_file = _rf
    get_function_name = _gfn

def extract_c_code(response: str) -> str:
    """
    Robustly extract C code from LLM response.
    Handles responses where C code is preceded by markdown spec text (Condition K).
    """
    if not response:
        return ""
    s = response.strip()
    # Find the first ```c or ``` fence anywhere in the response.
    # This handles spec-first responses (Condition K) where markdown text precedes the code.
    for fence in ["```c\n", "```c ", "```\n"]:
        idx = s.find(fence)
        if idx != -1:
            s = s[idx + len(fence):]
            break
    # Find last closing brace — everything after is likely prose
    last_brace = s.rfind("}")
    if last_brace != -1:
        s = s[:last_brace + 1]
    # Strip trailing markdown fence
    s = s.rstrip("`").strip()
    return s + "\n"


def normalize_entry_point(code: str, func_name: str) -> str:
    """Rename int main() to void {func_name}_harness(void) for CBMC --function compatibility."""
    import re
    code = re.sub(r'\bint\s+main\s*\(\s*void\s*\)\s*\{', f'void {func_name}_harness(void) {{', code)
    code = re.sub(r'\bint\s+main\s*\(\s*\)\s*\{', f'void {func_name}_harness(void) {{', code)
    return code

DATASET_DIR = experiment_dir / "dataset"
PROMPTS_DIR = experiment_dir / "prompts"
RESULTS_DIR = experiment_dir / "results"
EVAL_DIR = experiment_dir / "evaluation"

# Condition datasets and prompt files
CONDITION_DATASET = {
    "original": experiment_dir / "dataset",
    "A":        experiment_dir / "dataset_condA",
    "B":        experiment_dir / "dataset_condB",
    "C":        experiment_dir / "dataset_condA",   # same as A, CoT prompt
    "D":        experiment_dir / "dataset_condB",   # same as B, CoT prompt (no NL)
    "E":        experiment_dir / "dataset_condA",   # same as A, with few-shot GT example (same family)
    "F":        experiment_dir / "dataset_condA",   # same as A, with few-shot GT example (WRONG family)
    "G":        experiment_dir / "dataset_condA",   # single-pass baseline: no CBMC feedback
    "H":        experiment_dir / "dataset_condA",   # strategy-neutral: repair prompt gives no deletion instruction
    # Ablation conditions
    "I":        experiment_dir / "dataset_condA",   # like A + GT assertion category label at each SAT failure
    "J":        experiment_dir / "dataset_condA",   # like A + running deletion log shown
    "K":        experiment_dir / "dataset_condA",   # spec-first: NL contract before CBMC loop
    "Oracle":   experiment_dir / "dataset_condA",   # GT __CPROVER_assume preconditions provided
    "M":        experiment_dir / "dataset_condA",   # minimal CBMC guidance: scalar bounding instruction
    "A_v3":     experiment_dir / "dataset_condA",
    "B_v3":     experiment_dir / "dataset_condB",
}
CONDITION_PROMPT = {
    "original": "prompt_D_with_proof_helpers.txt",
    "A":        "prompt_condA.txt",
    "B":        "prompt_condB.txt",
    "C":        "prompt_condC.txt",   # condA dataset + chain-of-thought reasoning
    "D":        "prompt_condD.txt",   # condB dataset + chain-of-thought reasoning (no NL)
    "E":        "prompt_condE.txt",   # condA dataset + few-shot GT example from same family
    "F":        "prompt_condE.txt",   # condA dataset + few-shot GT example from WRONG family (ablation)
    "G":        "prompt_condA.txt",   # same initial prompt as A; loop never runs
    "H":        "prompt_condA.txt",   # same initial prompt as A; repair prompt is strategy-neutral
    "I":        "prompt_condA.txt",   # same initial prompt; fix_unknown prompt injects GT category label
    "J":        "prompt_condA.txt",   # same initial prompt; fix_unknown prompt shows deletion log
    "K":        "prompt_condK.txt",   # spec-first: asks for NL contract first, then harness
    "Oracle":   "prompt_condOracle.txt",  # GT preconditions pre-loaded in initial prompt
    "M":        "prompt_condM.txt",   # minimal CBMC guidance: explicit scalar bounding + corrected UNKNOWN message
    "A_v3":     "prompt_condA.txt",
    "B_v3":     "prompt_condB.txt",
}

def _model_dir_suffix(model: str) -> str:
    """Return a filesystem-safe directory suffix for the active model.
    For openrouter, includes the model slug so gpt-oss-120b and deepseek results
    never share a directory.
    """
    if model == "qwen":
        return ""
    if model == "openrouter":
        import os
        slug = os.getenv("OPENROUTER_MODEL", "openrouter")
        # e.g. "openai/gpt-oss-120b" → "gptoss120b"
        slug = slug.split("/")[-1]                     # take part after /
        slug = slug.replace("-", "").replace(".", "")  # strip punctuation
        return f"_{slug}"
    return f"_{model}"


# Active condition (set by --condition arg in main())
ACTIVE_CONDITION = "original"
# Active model backend (set by --model arg in main())
ACTIVE_MODEL = "qwen"


def _guess_gt_category_for_harness(harness_code: str, func_name: str) -> str:
    """Condition I: heuristically guess the GT assertion category for the current harness.
    Used to inject category label into fix_unknown_prompt for Condition I.
    Returns a comma-separated list of suspected categories based on assertion text."""
    import re
    asserts = re.findall(r'assert\s*\(([^;]+)\)', harness_code)
    cats = set()
    for a in asserts:
        a_lower = a.lower()
        if any(kw in a_lower for kw in ['allocator', 'old_', '.impl', 'unchanged']):
            cats.add('frame_condition')
        elif any(kw in a_lower for kw in ['.len', '.capacity', 'offset', 'length', 'size']):
            cats.add('length_invariant')
        else:
            cats.add('validity_predicate')
    return ", ".join(sorted(cats)) if cats else "validity_predicate"

# ── Condition E: few-shot family example mapping ──────────────────────────────
# Maps each function to a same-family GT harness used as a few-shot example.
# The example function must have a ground_truth_harness.c in dataset_condA.
# Functions listed as examples are excluded from condition E evaluation (they
# would be shown their own GT harness, which is not a valid test).

_FAMILY_EXAMPLES = {
    # byte_buf family → aws_byte_buf_init
    "aws_byte_buf_init":                   ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_clean_up":               ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_secure_zero":            ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_reset":                  ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_from_array":             ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_from_empty_array":       ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_from_c_str":             ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_eq":                     ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_write_u8":               ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_advance":                ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_append":                 ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_cat":                    ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_clean_up_secure":        ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_eq_c_str":               ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_init_copy":              ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_init_copy_from_cursor":  ("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    "aws_byte_buf_write_from_whole_string":("func2_aws_byte_buf_init",         "aws_byte_buf_init"),
    # byte_cursor family → aws_byte_cursor_from_array
    "aws_byte_cursor_advance":             ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_byte_cursor_from_array":          ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_byte_cursor_from_buf":            ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_byte_cursor_from_c_str":          ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_byte_cursor_from_string":         ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_byte_cursor_eq":                  ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_byte_cursor_eq_byte_buf":         ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_byte_cursor_eq_c_str":            ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    # array_list family → aws_array_list_init_static
    "aws_array_list_back":                 ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_capacity":             ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_clean_up":             ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_clear":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_erase":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_front":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_get_at":               ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_get_at_ptr":           ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_init_dynamic":         ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_init_static":          ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_length":               ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_pop_back":             ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_pop_front":            ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_pop_front_n":          ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_push_back":            ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_push_front":           ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_set_at":               ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_swap":                 ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_array_list_swap_contents":        ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    # linked_list family → aws_linked_list_push_back
    "aws_linked_list_back":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_begin":               ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_end":                 ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_front":               ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_init":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_insert_after":        ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_insert_before":       ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_next":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_node_reset":          ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_pop_back":            ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_pop_front":           ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_prev":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_push_back":           ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_push_front":          ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_rbegin":              ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_rend":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_remove":              ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_linked_list_swap_contents":       ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    # math/arithmetic family → aws_add_size_checked
    "aws_add_size_checked":                ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_add_size_saturating":             ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_mul_size_checked":                ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_mul_size_saturating":             ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_is_power_of_two":                 ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_round_up_to_power_of_two":        ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_nospec_mask":                     ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_ptr_eq":                          ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_array_eq":                        ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_array_eq_c_str":                  ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    # string family → aws_string_new_from_c_str
    "aws_string_bytes":                    ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_destroy":                  ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_destroy_secure":           ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_eq":                       ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_eq_byte_buf":              ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_eq_byte_cursor":           ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_eq_c_str":                 ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_new_from_array":           ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_new_from_c_str":           ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_string_new_from_string":          ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    # ring_buffer family → aws_ring_buffer_init
    "aws_ring_buffer_acquire":             ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_ring_buffer_buf_belongs_to_pool": ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_ring_buffer_clean_up":            ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_ring_buffer_init":                ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_ring_buffer_release":             ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
}

# Functions used as examples in condition E (should be excluded from E evaluation
# since they are shown their own GT harness)
_COND_E_EXAMPLE_FUNCS = {
    "aws_byte_buf_init",
    "aws_byte_cursor_from_array",
    "aws_array_list_init_static",
    "aws_linked_list_push_back",
    "aws_add_size_checked",
    "aws_string_new_from_c_str",
    "aws_ring_buffer_init",
}

# ── Condition F: wrong-family example mapping (ablation control) ──────────────
# Each function family is shown an example from a DIFFERENT family.
# This controls for the effect of "seeing any CBMC harness" vs "seeing a
# same-family harness with relevant predicate names".
#
# Rotation:
#   array_list   → linked_list  (aws_linked_list_push_back)
#   linked_list  → array_list   (aws_array_list_init_static)
#   byte_buf     → math         (aws_add_size_checked)
#   byte_cursor  → string       (aws_string_new_from_c_str)
#   math         → byte_buf     (aws_byte_buf_init)
#   string       → ring_buffer  (aws_ring_buffer_init)
#   ring_buffer  → byte_cursor  (aws_byte_cursor_from_array)

_WRONG_FAMILY_EXAMPLES = {
    # byte_buf → math example (aws_add_size_checked)
    "aws_byte_buf_init":                   ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_clean_up":               ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_secure_zero":            ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_reset":                  ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_from_array":             ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_from_empty_array":       ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_from_c_str":             ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_eq":                     ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_advance":                ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_append":                 ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_cat":                    ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_clean_up_secure":        ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_eq_c_str":               ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_init_copy":              ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    "aws_byte_buf_init_copy_from_cursor":  ("func1_aws_add_size_checked",       "aws_add_size_checked"),
    # byte_cursor → string example (aws_string_new_from_c_str)
    "aws_byte_cursor_advance":             ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_byte_cursor_from_array":          ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_byte_cursor_from_buf":            ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_byte_cursor_from_c_str":          ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_byte_cursor_from_string":         ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_byte_cursor_eq":                  ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_byte_cursor_eq_byte_buf":         ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    "aws_byte_cursor_eq_c_str":            ("func99_aws_string_new_from_c_str", "aws_string_new_from_c_str"),
    # array_list → linked_list example (aws_linked_list_push_back)
    "aws_array_list_back":                 ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_capacity":             ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_clean_up":             ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_clear":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_erase":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_front":                ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_get_at":               ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_get_at_ptr":           ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_init_dynamic":         ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_init_static":          ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_length":               ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_pop_back":             ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_pop_front":            ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_pop_front_n":          ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_push_back":            ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_push_front":           ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_set_at":               ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_swap":                 ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    "aws_array_list_swap_contents":        ("func5_aws_linked_list_push_back",  "aws_linked_list_push_back"),
    # linked_list → array_list example (aws_array_list_init_static)
    "aws_linked_list_back":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_begin":               ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_end":                 ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_front":               ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_init":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_insert_after":        ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_insert_before":       ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_next":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_node_reset":          ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_pop_back":            ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_pop_front":           ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_prev":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_push_back":           ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_push_front":          ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_rbegin":              ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_rend":                ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_remove":              ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    "aws_linked_list_swap_contents":       ("func37_aws_array_list_init_static","aws_array_list_init_static"),
    # math → byte_buf example (aws_byte_buf_init)
    "aws_add_size_checked":                ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_add_size_saturating":             ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_mul_size_checked":                ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_mul_size_saturating":             ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_is_power_of_two":                 ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_round_up_to_power_of_two":        ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_nospec_mask":                     ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_ptr_eq":                          ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_array_eq":                        ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    "aws_array_eq_c_str":                  ("func2_aws_byte_buf_init",          "aws_byte_buf_init"),
    # string → ring_buffer example (aws_ring_buffer_init)
    "aws_string_bytes":                    ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_string_destroy":                  ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_string_destroy_secure":           ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_string_eq":                       ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_string_eq_byte_buf":              ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_string_eq_byte_cursor":           ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_string_eq_c_str":                 ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    "aws_string_new_from_c_str":           ("func89_aws_ring_buffer_init",      "aws_ring_buffer_init"),
    # ring_buffer → byte_cursor example (aws_byte_cursor_from_array)
    "aws_ring_buffer_acquire":             ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_ring_buffer_buf_belongs_to_pool": ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_ring_buffer_clean_up":            ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_ring_buffer_init":                ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
    "aws_ring_buffer_release":             ("func63_aws_byte_cursor_from_array","aws_byte_cursor_from_array"),
}


def _get_family_example_harness(func_name: str) -> tuple[str, str]:
    """
    Return (example_func_name, example_harness_code) for condition E.
    Looks up the same-family GT harness from dataset_condA.
    Returns ("", "") if no example is found.
    """
    entry = _FAMILY_EXAMPLES.get(func_name)
    if not entry:
        return ("", "")
    func_dir_name, example_func = entry
    gt_path = experiment_dir / "dataset_condA" / func_dir_name / "ground_truth_harness.c"
    if gt_path.exists():
        return (example_func, gt_path.read_text(encoding="utf-8"))
    return ("", "")


def _get_wrong_family_example_harness(func_name: str) -> tuple[str, str]:
    """
    Return (example_func_name, example_harness_code) for condition F.
    Looks up a DIFFERENT-family GT harness from dataset_condA.
    This is the ablation control for condition E: every function sees a real
    CBMC harness example, but from the wrong family (different predicates).
    Returns ("", "") if no example is found.
    """
    entry = _WRONG_FAMILY_EXAMPLES.get(func_name)
    if not entry:
        return ("", "")
    func_dir_name, example_func = entry
    gt_path = experiment_dir / "dataset_condA" / func_dir_name / "ground_truth_harness.c"
    if gt_path.exists():
        return (example_func, gt_path.read_text(encoding="utf-8"))
    return ("", "")

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

SYSTEM_PROMPT = "You are an expert in CBMC formal verification of C programs. Output only valid C code, with no explanations or text after the code."

# Read GT harnesses once
def get_gt_harness(func_name: str) -> str:
    p = PROOFDIR / "proofs" / func_name / f"{func_name}_harness.c"
    return p.read_text(encoding="utf-8") if p.exists() else ""

PROOFDIR = Path("/home/weiqi/aws-c-common/verification/cbmc")


def build_initial_prompt(func_dir: str, func_name: str) -> str:
    """Build the initial generation prompt for the active condition."""
    # Conditions K and Oracle build their prompts dynamically — no template file needed
    if ACTIVE_CONDITION == "K":
        return build_spec_first_prompt(func_dir, func_name)
    if ACTIVE_CONDITION == "Oracle":
        return build_oracle_initial_prompt(func_dir, func_name)

    active_dataset = CONDITION_DATASET[ACTIVE_CONDITION]
    prompt_file = CONDITION_PROMPT[ACTIVE_CONDITION]
    func_path = active_dataset / func_dir
    header = read_file(func_path / "header.h")
    impl = read_file(func_path / "implementation.c")
    template = read_file(PROMPTS_DIR / prompt_file)

    prompt = template
    prompt = prompt.replace("{HEADER_CONTENT}", header)
    prompt = prompt.replace("{IMPLEMENTATION_CONTENT}", impl)
    prompt = prompt.replace("{FUNCTION_NAME}", func_name)

    # Condition E: inject same-family GT harness as few-shot example
    if ACTIVE_CONDITION == "E":
        example_func, example_harness = _get_family_example_harness(func_name)
        if example_func:
            prompt = prompt.replace("{EXAMPLE_FUNC}", example_func)
            prompt = prompt.replace("{EXAMPLE_HARNESS}", example_harness)
        else:
            prompt = prompt.replace("{EXAMPLE_FUNC}", "(no example available)")
            prompt = prompt.replace("{EXAMPLE_HARNESS}", "/* No reference harness available for this function family. */")

    # Condition F: inject WRONG-family GT harness (ablation control for E)
    elif ACTIVE_CONDITION == "F":
        example_func, example_harness = _get_wrong_family_example_harness(func_name)
        if example_func:
            prompt = prompt.replace("{EXAMPLE_FUNC}", example_func)
            prompt = prompt.replace("{EXAMPLE_HARNESS}", example_harness)
        else:
            prompt = prompt.replace("{EXAMPLE_FUNC}", "(no example available)")
            prompt = prompt.replace("{EXAMPLE_HARNESS}", "/* No reference harness available. */")

    return prompt


def build_fix_compilation_prompt(
    harness_code: str,
    func_name: str,
    error_msg: str,
    iteration: int
) -> str:
    return f"""Your CBMC harness for `{func_name}` has compilation errors. Fix it.

## Your harness (has errors):
```c
{harness_code}
```

## Compilation error:
```
{error_msg}
```

## Key rules:
- `#include <aws/common/X.h>` with angle brackets and full path
  (e.g. `<aws/common/array_list.h>`, `<aws/common/byte_buf.h>`, `<aws/common/linked_list.h>`)
- Only include `<proof_helpers/make_common_data_structures.h>` — it provides everything
- Do NOT include `<proof_helpers/nondet.h>`, `<proof_helpers/pointer_utils.h>`, etc.
- For allocator: use `struct aws_allocator *allocator = aws_default_allocator();`
- Use stack-allocated structs (not malloc) for data structures under test

Output ONLY the corrected C code. No explanations. Iteration {iteration}."""


def build_fix_verification_prompt(
    harness_code: str,
    func_name: str,
    failed_lines: str,
    iteration: int
) -> str:
    return f"""Your CBMC harness for `{func_name}` compiles but VERIFICATION FAILED. Fix it.

## Your harness (verification fails):
```c
{harness_code}
```

## Failed CBMC checks:
```
{failed_lines[:1500]}
```

## How to fix:
- If assertion too strong: weaken or remove it, or add a `__CPROVER_assume` to constrain the input
- If precondition missing: add `__CPROVER_assume(condition)` before the function call
- If using raw `struct aws_allocator`: replace with `struct aws_allocator *allocator = aws_default_allocator()`
- Re-examine the function implementation to check what invariants must hold

Output ONLY the corrected C harness code. No explanations. Iteration {iteration}."""


def build_fix_verification_h_prompt(
    harness_code: str,
    func_name: str,
    failed_lines: str,
    cbmc_stdout: str,
    iteration: int
) -> str:
    """Condition H — strategy-neutral repair prompt.

    Gives the LLM only the violated assertion and counterexample trace.
    Provides NO instruction on whether to delete, weaken, add assumes, or refine.
    Tests whether active sacrifice is emergent (LLM-intrinsic) vs instructed.
    """
    return f"""Your CBMC harness for `{func_name}` has a verification failure.

## Your harness:
```c
{harness_code}
```

## CBMC output:
```
{cbmc_stdout[:2000]}
```

## Failed checks:
```
{failed_lines[:1000]}
```

Modify the harness so CBMC verification succeeds.
- Use `aws_default_allocator()` for `struct aws_allocator *` (NOT `can_fail_allocator`)
- Only use standard includes: `aws/common/*.h`, `proof_helpers/make_common_data_structures.h`, `assert.h`, `stdlib.h`, `stdint.h`

Output ONLY the corrected C harness code. Iteration {iteration}."""


def build_fix_unknown_prompt(
    harness_code: str,
    func_name: str,
    iteration: int,
    # Condition I: GT category label for the failed assertion (optional)
    gt_category_label: str = "",
    # Condition J: running deletion log (optional)
    deletion_log: list = None,
) -> str:
    # Condition M: accurate UNKNOWN diagnosis (unbounded inputs OR unreachable assertions)
    if ACTIVE_CONDITION == "M":
        unknown_explanation = f"""Your CBMC harness for `{func_name}` produced UNKNOWN result.

CBMC UNKNOWN has two possible causes — check BOTH before rewriting:
1. **Unbounded scalar inputs**: if you used `nondet_size_t()`, `nondet_uint32_t()` etc. without an immediately following `__CPROVER_assume` bound, the state space is infinite and CBMC cannot complete. FIX: add bounds like `__CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE)`.
2. **Unreachable assertions**: assert() calls exist but no code path reaches them. FIX: check that setup assumptions don't over-constrain the path to your assertions.

**Try fixing the bounding first** — this is the most common cause of UNKNOWN."""
    else:
        unknown_explanation = f"""Your CBMC harness for `{func_name}` produced UNKNOWN result (CBMC found no reachable assertions).
This means the harness likely has no assert() calls, or all code paths are unreachable."""

    base = f"""{unknown_explanation}

## Your harness:
```c
{harness_code}
```

Please rewrite the harness correctly, ensuring:
- There are `assert(condition)` calls to check postconditions
- The function `{func_name}` is actually called (not a different function)
- All code branches are reachable
- Use `aws_default_allocator()` for any `struct aws_allocator *` (NOT `can_fail_allocator`)
- Only use these includes: `aws/common/*.h`, `proof_helpers/make_common_data_structures.h`, `proof_helpers/nondet.h`, `proof_helpers/utils.h`, `assert.h`, `stdlib.h`, `stdint.h`, `stdbool.h`"""

    # Condition I: inject GT assertion category label
    if gt_category_label:
        base += f"""

## Assertion category hint (Condition I):
The assertion(s) causing UNKNOWN belong to category: **{gt_category_label}**
- validity_predicate: pointer non-null, return value range, error-code postconditions
- length_invariant: buffer length/capacity/offset relationships after the call
- frame_condition: unchanged memory — allocator, pointer identity, side-effect discipline
Do NOT remove assertions of this category — they are safety-relevant postconditions."""

    # Condition J: inject running deletion log
    if deletion_log:
        deleted_text = "\n".join(f"  - {a}" for a in deletion_log[-10:])
        base += f"""

## Assertions you have removed in prior iterations (Condition J):
{deleted_text}
These were removed in earlier iterations. If any were correct postconditions, consider restoring them
with a more precise predicate instead of deleting them permanently."""

    base += f"\n\nOutput ONLY the corrected C code. No explanations. Iteration {iteration}."
    return base


def build_spec_first_prompt(func_dir: str, func_name: str) -> str:
    """Condition K: spec-first phase. Ask LLM to write NL contract before generating harness."""
    active_dataset = CONDITION_DATASET["K"]
    func_path = active_dataset / func_dir
    header = read_file(func_path / "header.h")
    impl = read_file(func_path / "implementation.c")
    return f"""You are writing a CBMC proof harness for `{func_name}`.

## Function declaration:
```c
{header}
```

## Implementation:
```c
{impl}
```

**Step 1 — Write a formal contract FIRST** (before any code):

```
Preconditions: [list each __CPROVER_assume condition you will set up]
Postconditions (validity): [pointer non-null, return value, error codes]
Postconditions (length): [buffer length/capacity invariants after the call]
Postconditions (frame): [memory locations NOT modified by the function]
```

**Step 2 — Generate the CBMC harness** that verifies your contract using assert() statements.

Rules:
- Use `aws_default_allocator()` for any `struct aws_allocator *`
- Include only: `aws/common/*.h`, `proof_helpers/make_common_data_structures.h`, `assert.h`, `stdlib.h`, `stdint.h`, `stdbool.h`
- FORBIDDEN: `proof_helpers/proof_allocators.h`, `can_fail_allocator()`

Output the contract block first, then the complete C harness code."""


def build_oracle_initial_prompt(func_dir: str, func_name: str) -> str:
    """Oracle Setup condition: inject GT __CPROVER_assume preconditions into initial prompt."""
    active_dataset = CONDITION_DATASET["Oracle"]
    func_path = active_dataset / func_dir
    header = read_file(func_path / "header.h")
    impl = read_file(func_path / "implementation.c")

    # Load GT harness to extract its __CPROVER_assume lines
    gt_path = func_path / "ground_truth_harness.c"
    gt_assumes = ""
    if gt_path.exists():
        gt_code = gt_path.read_text()
        import re
        assumes = re.findall(r'__CPROVER_assume\([^;]+\);', gt_code)
        if assumes:
            gt_assumes = "\n".join(f"  {a}" for a in assumes)

    assume_block = f"""
## Ground-truth preconditions (use these exactly in your harness setup):
```c
{gt_assumes if gt_assumes else "// No GT preconditions available — generate your own"}
```
These are the structural validity assumptions an expert uses for `{func_name}`.
Copy them into your harness BEFORE the function call.
You only need to write the POSTCONDITION assert() statements — do NOT modify the setup above.
""" if gt_assumes else ""

    return f"""Write a CBMC proof harness for `{func_name}`.

## Function declaration:
```c
{header}
```

## Implementation:
```c
{impl}
```
{assume_block}
Write assert() postconditions that verify:
1. Return value / error code correctness (validity predicates)
2. Output buffer length/capacity invariants (length invariants)
3. Memory not modified beyond the function's contract (frame conditions)

Rules:
- Use `aws_default_allocator()` for any `struct aws_allocator *`
- Include only: `aws/common/*.h`, `proof_helpers/make_common_data_structures.h`, `assert.h`, `stdlib.h`, `stdint.h`, `stdbool.h`
- FORBIDDEN: `proof_helpers/proof_allocators.h`, `can_fail_allocator()`

Output ONLY complete C harness code."""


@dataclass
class IterationRecord:
    iteration: int
    harness_code: str
    cbmc_result: CBMCResult
    action_taken: str  # "initial", "fix_compile", "fix_verify", "done"


@dataclass
class FeedbackResult:
    func_dir: str
    func_name: str
    gt_result: CBMCResult
    iterations: list = field(default_factory=list)

    def final_result(self) -> Optional[CBMCResult]:
        if self.iterations:
            return self.iterations[-1].cbmc_result
        return None

    def converged(self) -> bool:
        r = self.final_result()
        return r is not None and r.verification_result == "SUCCESS"

    def first_pass_compile(self) -> bool:
        if self.iterations:
            return self.iterations[0].cbmc_result.compilation_ok
        return False

    def first_pass_verify(self) -> bool:
        if self.iterations:
            return self.iterations[0].cbmc_result.verification_result == "SUCCESS"
        return False

    def num_iterations(self) -> int:
        return len(self.iterations)

    def verification_equivalent(self) -> bool:
        """Does final result match GT result?"""
        final = self.final_result()
        if final is None:
            return False
        return final.verification_result == self.gt_result.verification_result

    def to_dict(self) -> dict:
        return {
            "func": self.func_name,
            "gt_verification": self.gt_result.verification_result,
            "first_pass_compile": self.first_pass_compile(),
            "first_pass_verify": self.first_pass_verify(),
            "converged": self.converged(),
            "verification_equivalent": self.verification_equivalent(),
            "num_iterations": self.num_iterations(),
            "iterations": [
                {
                    "iter": rec.iteration,
                    "action": rec.action_taken,
                    "compile_ok": rec.cbmc_result.compilation_ok,
                    "verify": rec.cbmc_result.verification_result,
                    "num_failed": rec.cbmc_result.num_failed,
                }
                for rec in self.iterations
            ]
        }


def run_feedback_loop(
    func_dir: str,
    func_name: str,
    max_iterations: int = 4,
    save_all: bool = True
) -> FeedbackResult:
    """Run the CBMC feedback loop for one function."""
    print(f"\n{'='*60}")
    print(f"Function: {func_name}  (max {max_iterations} iterations)")
    print(f"{'='*60}")

    result = FeedbackResult(func_dir=func_dir, func_name=func_name,
                            gt_result=run_gt(func_name))
    print(f"  GT baseline: {result.gt_result.verification_result}")

    model_suffix = _model_dir_suffix(ACTIVE_MODEL)
    output_dir = RESULTS_DIR / f"feedback_loop_{ACTIVE_CONDITION}{model_suffix}" / func_name
    output_dir.mkdir(parents=True, exist_ok=True)

    # Step 1: Initial generation
    print(f"\n  [Iter 1] Generating initial harness...")
    initial_prompt = build_initial_prompt(func_dir, func_name)
    harness_code = normalize_entry_point(
        extract_c_code(call_qwen(SYSTEM_PROMPT, initial_prompt)), func_name)

    harness_path = output_dir / "iter_1_harness.c"
    harness_path.write_text(harness_code, encoding="utf-8")

    cbmc_result = run_cbmc(func_name, harness_path)
    print(f"    Compile: {'OK' if cbmc_result.compilation_ok else 'FAIL'}")
    print(f"    Verify:  {cbmc_result.verification_result}  ({cbmc_result.num_failed}/{cbmc_result.num_checks} failed)")

    record = IterationRecord(
        iteration=1,
        harness_code=harness_code,
        cbmc_result=cbmc_result,
        action_taken="initial"
    )
    result.iterations.append(record)

    # Condition G: single-pass baseline — no feedback loop
    if ACTIVE_CONDITION == "G":
        print(f"  Condition G: single-pass, stopping after iter 1.")
        (output_dir / "summary.json").write_text(json.dumps(result.to_dict(), indent=2))
        return result

    # Condition J: track deletion log for running history
    deletion_log_j: list[str] = []

    # Feedback loop
    consecutive_unknown = 0
    for i in range(2, max_iterations + 1):
        if cbmc_result.verification_result == "SUCCESS":
            print(f"  Converged at iteration {i-1}!")
            break

        # Early exit: 3 consecutive UNKNOWN means the model cannot add assertions
        # under this prompt condition — further iterations add no research value.
        if cbmc_result.verification_result == "UNKNOWN":
            consecutive_unknown += 1
            if consecutive_unknown >= 3:
                print(f"  Early exit: {consecutive_unknown} consecutive UNKNOWN — "
                      f"model cannot add assertions under condition {ACTIVE_CONDITION}.")
                break
        else:
            consecutive_unknown = 0

        # Extract failed check lines for better LLM context
        failed_lines = "\n".join(
            l for l in cbmc_result.stdout.split("\n")
            if ": FAILED" in l or ": FAILURE" in l
        )

        # Determine fix type
        if not cbmc_result.compilation_ok:
            action = "fix_compile"
            print(f"\n  [Iter {i}] Fixing compilation error...")
            fix_prompt = build_fix_compilation_prompt(
                harness_code, func_name, cbmc_result.error_summary, i
            )
        elif cbmc_result.verification_result == "FAIL":
            action = "fix_verify"
            print(f"\n  [Iter {i}] Fixing verification failure...")
            print(f"    Failed checks: {failed_lines[:200]}")
            if ACTIVE_CONDITION == "H":
                fix_prompt = build_fix_verification_h_prompt(
                    harness_code, func_name, failed_lines, cbmc_result.stdout, i
                )
            else:
                fix_prompt = build_fix_verification_prompt(
                    harness_code, func_name, failed_lines, i
                )
        elif cbmc_result.verification_result == "UNKNOWN":
            action = "fix_unknown"
            print(f"\n  [Iter {i}] Fixing UNKNOWN (no reachable assertions)...")

            # Condition I: inject GT category label for the previous iteration's assertions
            gt_cat = ""
            if ACTIVE_CONDITION == "I":
                gt_cat = _guess_gt_category_for_harness(harness_code, func_name)

            # Condition J: pass running deletion log
            del_log = deletion_log_j if ACTIVE_CONDITION == "J" else []

            fix_prompt = build_fix_unknown_prompt(
                harness_code, func_name, i,
                gt_category_label=gt_cat,
                deletion_log=del_log,
            )
        else:
            print(f"  No fix possible for result: {cbmc_result.verification_result}")
            break

        harness_code = normalize_entry_point(
            extract_c_code(call_qwen(SYSTEM_PROMPT, fix_prompt)), func_name)
        harness_path = output_dir / f"iter_{i}_harness.c"
        harness_path.write_text(harness_code, encoding="utf-8")

        cbmc_result = run_cbmc(func_name, harness_path)
        print(f"    Compile: {'OK' if cbmc_result.compilation_ok else 'FAIL'}")
        print(f"    Verify:  {cbmc_result.verification_result}  ({cbmc_result.num_failed}/{cbmc_result.num_checks} failed)")

        record = IterationRecord(
            iteration=i,
            harness_code=harness_code,
            cbmc_result=cbmc_result,
            action_taken=action
        )
        result.iterations.append(record)

        # Condition J: update deletion log from this iteration's diff
        if ACTIVE_CONDITION == "J" and len(result.iterations) >= 2:
            import re
            prev_asserts = set(re.findall(r'assert\s*\(([^;]+)\)', result.iterations[-2].harness_code))
            curr_asserts = set(re.findall(r'assert\s*\(([^;]+)\)', harness_code))
            deleted_this_iter = prev_asserts - curr_asserts
            deletion_log_j.extend(sorted(deleted_this_iter))

    # Save summary
    summary = result.to_dict()
    (output_dir / "summary.json").write_text(json.dumps(summary, indent=2))

    final = result.final_result()
    if final:
        print(f"\n  Final: compile={'OK' if final.compilation_ok else 'FAIL'} "
              f"verify={final.verification_result} "
              f"equivalent={result.verification_equivalent()} "
              f"iters={result.num_iterations()}")

    return result


def print_summary_table(results: list):
    w = 85
    print(f"\n{'='*w}")
    print(f"{'FEEDBACK LOOP SUMMARY':^{w}}")
    print(f"{'='*w}")
    print(f"{'Function':<35} {'GT':^8} {'P1Cmp':^7} {'P1Vrfy':^7} {'Final':^8} {'Equiv':^7} {'Iters':^6}")
    print(f"{'-'*w}")
    for r in results:
        gt = r.gt_result.verification_result[:4]
        p1c = "OK" if r.first_pass_compile() else "FAIL"
        p1v = "OK" if r.first_pass_verify() else "FAIL"
        final = r.final_result().verification_result[:4] if r.final_result() else "N/A"
        equiv = "YES" if r.verification_equivalent() else "NO"
        iters = r.num_iterations()
        print(f"{r.func_name:<35} {gt:^8} {p1c:^7} {p1v:^7} {final:^8} {equiv:^7} {iters:^6}")
    print(f"{'='*w}")

    # Aggregate stats
    n = len(results)
    if n:
        p1c_rate = sum(1 for r in results if r.first_pass_compile()) / n
        p1v_rate = sum(1 for r in results if r.first_pass_verify()) / n
        conv_rate = sum(1 for r in results if r.converged()) / n
        equiv_rate = sum(1 for r in results if r.verification_equivalent()) / n
        avg_iters = sum(r.num_iterations() for r in results) / n
        print(f"\nAggregates (n={n}):")
        print(f"  First-pass compile rate:  {p1c_rate:.0%}")
        print(f"  First-pass verify rate:   {p1v_rate:.0%}")
        print(f"  Final convergence rate:   {conv_rate:.0%}")
        print(f"  Verification equivalence: {equiv_rate:.0%}")
        print(f"  Avg iterations:           {avg_iters:.1f}")


def main():
    global ACTIVE_CONDITION, ACTIVE_MODEL
    parser = argparse.ArgumentParser(description="CBMC feedback loop for harness refinement")
    parser.add_argument("--func", help="Function name (e.g. aws_array_list_back)")
    parser.add_argument("--all", action="store_true", help="Run all 30 functions")
    parser.add_argument("--max-iter", type=int, default=15, help="Max iterations (default: 15)")
    parser.add_argument("--save-json", action="store_true", help="Save aggregate results JSON")
    parser.add_argument("--condition",
                        choices=["original", "A", "B", "C", "D", "E", "F", "G", "H",
                                 "I", "J", "K", "Oracle", "M", "A_v3", "B_v3"],
                        default="original",
                        help=("Prompt condition: A=source+NL, B=source only, C=NL+CoT, D=no-NL+CoT, "
                              "E=same-family few-shot, F=wrong-family few-shot, "
                              "G=single-pass no-feedback, H=strategy-neutral repair"))
    parser.add_argument("--model", choices=["qwen", "claude", "openrouter"], default="qwen",
                        help="LLM backend: qwen (DashScope), claude (Anthropic), openrouter")
    args = parser.parse_args()

    ACTIVE_CONDITION = args.condition
    ACTIVE_MODEL = args.model
    _load_model_backend(args.model)
    print(f"Running with condition: {ACTIVE_CONDITION}, model: {ACTIVE_MODEL}")

    funcs = PILOT_FUNCTIONS if args.all else None
    if args.func:
        funcs = [(d, n) for d, n in PILOT_FUNCTIONS if n == args.func]
        if not funcs:
            # Try matching by func_name directly
            funcs = [(f"func__{args.func}", args.func)]

    if not funcs:
        parser.print_help()
        sys.exit(1)

    results = []
    for func_dir, func_name in funcs:
        # Skip if already completed (resume support)
        model_suffix = _model_dir_suffix(ACTIVE_MODEL)
        output_dir = RESULTS_DIR / f"feedback_loop_{ACTIVE_CONDITION}{model_suffix}" / func_name
        summary_path = output_dir / "summary.json"
        if summary_path.exists():
            import json as _json
            saved = _json.loads(summary_path.read_text())
            print(f"  SKIP (already done): {func_name}  [{saved.get('converged','?')} converged]")
            continue
        try:
            r = run_feedback_loop(func_dir, func_name, max_iterations=args.max_iter)
            results.append(r)
        except Exception as e:
            print(f"  ERROR for {func_name}: {e}")
            import traceback; traceback.print_exc()

    if len(results) > 1:
        print_summary_table(results)

    if args.save_json and results:
        EVAL_DIR.mkdir(parents=True, exist_ok=True)
        model_suffix = _model_dir_suffix(ACTIVE_MODEL)
        out = EVAL_DIR / f"feedback_loop_results_cond{ACTIVE_CONDITION}{model_suffix}.json"
        out.write_text(json.dumps([r.to_dict() for r in results], indent=2))
        print(f"\nSaved to: {out}")


if __name__ == "__main__":
    main()
