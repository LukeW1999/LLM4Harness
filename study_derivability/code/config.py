"""
Centralized data paths for the Derivability study.
All other scripts import from here.
"""
from pathlib import Path

# ── Repository root ────────────────────────────────────────────────────────────
REPO_ROOT = Path(__file__).parent.parent.parent   # LLM4Harness/

# ── study_derivability root ────────────────────────────────────────────────────
STUDY_ROOT = REPO_ROOT / "study_derivability"
DATA_DIR   = STUDY_ROOT / "data"
FIG_DIR    = STUDY_ROOT / "figures"
REPORT_DIR = STUDY_ROOT / "reports"
CODE_DIR   = STUDY_ROOT / "code"

# ── Cloned corpora (sparse checkouts) ─────────────────────────────────────────
CORPORA_DIR        = STUDY_ROOT / "corpora"
AWS_CORPUS_DIR     = CORPORA_DIR / "aws-c-common" / "verification" / "cbmc" / "proofs"
S2N_CORPUS_DIR     = CORPORA_DIR / "s2n-tls" / "tests" / "cbmc" / "proofs"
FREERTOS_CORPUS_DIR = CORPORA_DIR / "FreeRTOS-Plus-TCP" / "test" / "cbmc" / "proofs"

# ── PI's existing experiment data (aws-c-common) ───────────────────────────────
AWS_EXP        = REPO_ROOT / "experiment_aws_cbmc"
AWS_DATASET    = AWS_EXP / "dataset"           # func{N}_{name}/ dirs — 76/101 have GT harnesses
# NOTE: use AWS_CORPUS_DIR (fresh clone) for GT harnesses; all 84 PI functions present there
AWS_RESULTS    = AWS_EXP / "results"           # feedback_loop_{cond}_{model}/ dirs
AWS_EVAL       = AWS_EXP / "evaluation"        # cross_verify_results_cond*.json
AWS_ANNOTATION = AWS_EXP / "annotation"        # annotated_missed_properties.csv

# Key annotation file (191 missed assertions, condA_claude)
TAXONOMY_CSV = AWS_ANNOTATION / "annotated_missed_properties.csv"

# Per-function assertion-level match files (cross_verify JSONs)
# Keys: (condition, model) → path
CROSS_VERIFY_FILES = {
    ("A", "claude"): AWS_EVAL / "cross_verify_results_condA_claude.json",
    ("B", "claude"): AWS_EVAL / "cross_verify_results_condB_claude.json",
    ("C", "claude"): AWS_EVAL / "cross_verify_results_condC_claude.json",
    ("D", "claude"): AWS_EVAL / "cross_verify_results_condD_claude.json",
    ("E", "claude"): AWS_EVAL / "iter_recall_feedback_loop_E_claude.json",  # best-iter
    ("F", "claude"): AWS_EVAL / "iter_recall_feedback_loop_F_claude.json",
    ("A", "qwen"):   AWS_EVAL / "cross_verify_results_condA.json",
    ("B", "qwen"):   AWS_EVAL / "cross_verify_results_condB.json",
    ("E", "qwen"):   AWS_EVAL / "iter_recall_feedback_loop_E.json",
}

# Result directories per (condition, model)
AWS_RESULT_DIRS = {
    ("A", "claude"): AWS_RESULTS / "feedback_loop_A_claude",
    ("B", "claude"): AWS_RESULTS / "feedback_loop_B_claude",
    ("C", "claude"): AWS_RESULTS / "feedback_loop_C_claude",
    ("D", "claude"): AWS_RESULTS / "feedback_loop_D_claude",
    ("E", "claude"): AWS_RESULTS / "feedback_loop_E_claude",
    ("F", "claude"): AWS_RESULTS / "feedback_loop_F_claude",
    ("A", "qwen"):   AWS_RESULTS / "feedback_loop_A",
    ("B", "qwen"):   AWS_RESULTS / "feedback_loop_B",
    ("C", "qwen"):   AWS_RESULTS / "feedback_loop_C",
    ("D", "qwen"):   AWS_RESULTS / "feedback_loop_D",
    ("E", "qwen"):   AWS_RESULTS / "feedback_loop_E",
}

# ── PI's existing experiment data (s2n-tls) ────────────────────────────────────
S2N_EXP     = REPO_ROOT / "experiment_s2n"
S2N_DATASET = S2N_EXP / "dataset" / "s2n_stuffer_dataset.json"
S2N_RESULTS = S2N_EXP / "results"
S2N_EVAL    = S2N_EXP / "evaluation"

S2N_RECALL_FILES = {
    ("A", "qwen"): S2N_EVAL / "s2n_recall_condA.json",
    ("E", "qwen"): S2N_EVAL / "s2n_recall_condE.json",
}
S2N_RESULT_DIRS = {
    ("A", "qwen"): S2N_RESULTS / "s2n_condA",
    ("E", "qwen"): S2N_RESULTS / "s2n_condE",
}

# ── FreeRTOS (no LLM runs yet) ────────────────────────────────────────────────
FREERTOS_SHORTLIST = STUDY_ROOT / "freertos_shortlist.md"
FREERTOS_RESULTS   = STUDY_ROOT / "data" / "freertos_results"  # to be created in Phase 3
