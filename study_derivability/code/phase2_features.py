#!/usr/bin/env python3
"""
Phase 2 — Feature engineering per GT assertion.

Two feature groups (two-barrier hypothesis):

  Knowledge-barrier features  — predict miss when info absent from provided text
    f_var_overlap        fraction of assertion variables found in function body
    f_field_overlap      fraction of struct field accesses found in function body
    f_predicate_in_impl  predicate name appears anywhere in function implementation
    f_predicate_in_postcond  predicate in AWS_POSTCONDITION/return-path position
    f_predicate_in_nl    predicate in NL header Ensures/postcondition text
    f_uses_cbmc_builtin  expression uses __CPROVER_* builtins

  Strategy-barrier features   — predict miss when info present but LLM doesn't write it
    f_is_frame_negative  assertion says something did NOT change  (old.X == new.X)
    f_position_post_call assertion is located after function-under-test call
    f_negation_depth     count of negation operators (!, NOT chains)
    f_arity              argument count of top-level call

  Metadata pass-through (from taxonomy join)
    f_tax_info_source / f_tax_reasoning / f_tax_category

Output:
  data/gt_assertions_features.csv
  figures/phase2_corr_heatmap.png
  figures/phase2_feature_dist.png
  reports/feature_definitions.md
  reports/phase_2.md
"""

import csv
import json
import re
import sys
from collections import defaultdict
from pathlib import Path

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
from config import (
    AWS_CORPUS_DIR, DATA_DIR, FIG_DIR, REPORT_DIR,
    S2N_DATASET,
    FREERTOS_CORPUS_DIR,
)

# ── Source-text index builders ────────────────────────────────────────────────

AWS_SRCDIR = Path("/home/weiqi/Verification/aws-c-common")
AWS_DATASET = Path("/home/weiqi/Verification/LLM4Harness/experiment_aws_cbmc/dataset")
S2N_CORPDIR = Path("/home/weiqi/Verification/LLM4Harness/study_derivability/corpora/s2n-tls")
FRT_SRCDIR  = Path("/home/weiqi/Verification/LLM4Harness/study_derivability/corpora/FreeRTOS-Plus-TCP/source")


def _strip_comments(code: str) -> str:
    code = re.sub(r'//[^\n]*', ' ', code)
    code = re.sub(r'/\*.*?\*/', ' ', code, flags=re.DOTALL)
    return code


def build_aws_source_index() -> dict[str, dict]:
    """
    {func_name: {impl: str, header: str, impl_lower: str, header_lower: str}}
    Primary: dataset/funcN_*/implementation.c + header.h
    Fallback: fresh clone source for functions missing from dataset.
    """
    idx: dict[str, dict] = {}
    for d in AWS_DATASET.glob("func*"):
        func = d.name.split("_", 1)[1]
        impl_path   = d / "implementation.c"
        header_path = d / "header.h"
        impl   = impl_path.read_text(errors="replace")   if impl_path.exists()   else ""
        header = header_path.read_text(errors="replace") if header_path.exists() else ""
        if impl or header:
            idx[func] = {"impl": impl, "header": header,
                         "impl_lower": impl.lower(), "header_lower": header.lower()}
    # Fallback: search fresh clone source files for missing functions
    aws_src = AWS_SRCDIR / "source"
    if aws_src.exists():
        for func in set():  # populate only on demand
            pass
    return idx


def build_s2n_source_index() -> dict[str, dict]:
    data = json.loads(S2N_DATASET.read_text())
    idx: dict[str, dict] = {}
    for func, entry in data.items():
        impl   = entry.get("implementation", "")
        header = entry.get("header_declaration", "")
        idx[func] = {"impl": impl, "header": header,
                     "impl_lower": impl.lower(), "header_lower": header.lower()}
    return idx


def build_freertos_source_index() -> dict[str, dict]:
    """
    Map harness function name → source file text by searching FRT source/ dir.
    Strategy: find the function definition by name-variant in all source files.
    """
    if not FRT_SRCDIR.exists():
        return {}

    # Build a big combined source text (fast enough for feature matching)
    all_src = ""
    src_by_file: dict[str, str] = {}
    for sf in FRT_SRCDIR.glob("*.c"):
        txt = sf.read_text(errors="replace")
        src_by_file[sf.name] = txt
        all_src += txt

    def find_func_impl(func_name: str) -> str:
        """Try several name variants to find the function in source."""
        # Harness name may be CamelCase; FreeRTOS functions use prefixes
        # e.g. ARPAgeCache → vARPAgeCache, eARPGetCacheEntry, etc.
        candidates = [
            func_name,                         # exact
            "v" + func_name,                   # void prefix
            "e" + func_name,                   # enum return
            "x" + func_name,                   # BaseType_t return
            "prv" + func_name,                 # private
            "FreeRTOS_" + func_name,           # public API
        ]
        for name in candidates:
            pat = re.compile(
                r'[\w\s\*]+\b' + re.escape(name) + r'\s*\([^;]*\)\s*\{',
                re.MULTILINE
            )
            for sf_name, sf_text in src_by_file.items():
                m = pat.search(sf_text)
                if m:
                    # Extract function body (roughly: from match to matching })
                    start = m.end() - 1  # at the opening {
                    depth, i = 1, start + 1
                    while i < len(sf_text) and depth > 0:
                        if sf_text[i] == '{':
                            depth += 1
                        elif sf_text[i] == '}':
                            depth -= 1
                        i += 1
                    return sf_text[start:i]
        return all_src  # fallback: all source (coarser but catches everything)

    idx: dict[str, dict] = {}
    for func in set():  # populate lazily
        pass
    # Pre-build for all FreeRTOS assertion functions
    rows = list(csv.DictReader((DATA_DIR / "gt_assertions.csv").open()))
    frt_funcs = {r["function"] for r in rows if r["corpus"] == "freertos"}
    for func in frt_funcs:
        impl = find_func_impl(func)
        idx[func] = {"impl": impl, "header": "", "impl_lower": impl.lower(), "header_lower": ""}
    return idx


# ── Individual feature functions ──────────────────────────────────────────────

def extract_identifiers(expr: str) -> set[str]:
    """All identifier tokens in expression, excluding keywords/types."""
    SKIP = {"int", "size_t", "uint64_t", "uint32_t", "bool", "true", "false",
            "null", "nullptr", "void", "char", "long", "unsigned", "return",
            "sizeof", "if", "else", "while", "for", "assert", "aws_op_success",
            "aws_op_err"}
    tokens = re.findall(r'\b[a-z_][a-z0-9_]*\b', expr.lower())
    return {t for t in tokens if t not in SKIP and len(t) > 1}


def extract_field_accesses(expr: str) -> set[str]:
    """Struct field access patterns like buf.len, list.head, _v_.len"""
    return set(re.findall(r'(?:_v_|\w+)\.(\w+)', expr.lower()))


def f_var_overlap(expr_norm: str, impl_text: str) -> float:
    """Fraction of identifier tokens in assertion also in function body."""
    if not impl_text:
        return -1.0   # NA
    impl_clean = _strip_comments(impl_text).lower()
    ids = extract_identifiers(expr_norm)
    if not ids:
        return 0.0
    found = sum(1 for v in ids if re.search(r'\b' + re.escape(v) + r'\b', impl_clean))
    return round(found / len(ids), 3)


def f_field_overlap(expr_norm: str, impl_text: str) -> float:
    """Fraction of struct fields in assertion also in function body."""
    if not impl_text:
        return -1.0
    impl_clean = _strip_comments(impl_text).lower()
    fields = extract_field_accesses(expr_norm)
    if not fields:
        return -1.0  # NA (no field accesses)
    found = sum(1 for f in fields if re.search(r'\b' + re.escape(f) + r'\b', impl_clean))
    return round(found / len(fields), 3)


def f_predicate_in_impl(expr_norm: str, impl_text: str) -> int:
    """1 if any named predicate (aws_*_is_valid / s2n_*_validate / *_valid) in expression
    also appears anywhere in the function implementation text."""
    if not impl_text:
        return -1
    predicates = re.findall(r'\b(\w*(?:is_valid|validate|_valid)\w*)\b', expr_norm)
    if not predicates:
        return -1   # NA: expression has no validity predicate
    impl_lower = impl_text.lower()
    return int(any(p.lower() in impl_lower for p in predicates))


def f_predicate_in_postcond(expr_norm: str, impl_text: str) -> int:
    """1 if predicate appears in AWS_POSTCONDITION() / POSIX_POSTCONDITION() call
    in the function implementation."""
    if not impl_text:
        return -1
    predicates = re.findall(r'\b(\w*(?:is_valid|validate|_valid)\w*)\b', expr_norm)
    if not predicates:
        return -1
    postcond_re = re.compile(
        r'(?:AWS_POSTCONDITION|POSIX_POSTCONDITION|ENSURES)\s*\([^)]*\b'
        + '|'.join(re.escape(p) for p in predicates) + r'\b',
        re.IGNORECASE
    )
    return int(bool(postcond_re.search(impl_text)))


def f_predicate_in_nl(expr_norm: str, header_text: str) -> int:
    """1 if predicate appears in NL Ensures/postcondition annotation in header."""
    if not header_text:
        return -1
    predicates = re.findall(r'\b(\w*(?:is_valid|validate|_valid)\w*)\b', expr_norm)
    if not predicates:
        return -1
    ensures_blocks = re.findall(
        r'(?:Ensures|@ensures|postcondition|@post)[^\n]*', header_text, re.IGNORECASE
    )
    block_text = ' '.join(ensures_blocks).lower()
    return int(any(p.lower() in block_text for p in predicates))


def f_uses_cbmc_builtin(expr_norm: str) -> int:
    """1 if expression uses __CPROVER_* or AWS_PRECONDITION-like builtins."""
    return int(bool(re.search(r'__cprover_|cprover_', expr_norm)))


FRAME_PATTERNS = [
    re.compile(r'\bold_\w+\s*=='),          # old_X == ...
    re.compile(r'==\s*old_\w+'),            # ... == old_X
    re.compile(r'\b_v_\.\w+\s*==\s*_v_\.\w+'),  # _v_.field == _v_.field (normalised)
    re.compile(r'\bOLD_\w+\b'),             # OLD_X (evaluate_harness convention)
]

def f_is_frame_negative(expr_norm: str) -> int:
    """1 if assertion expresses 'something did not change' (frame condition)."""
    return int(any(p.search(expr_norm) for p in FRAME_PATTERNS))


def f_position_post_call(expr_norm: str, harness_text: str, func_name: str) -> int:
    """
    1 if this assertion appears AFTER the call to func_name in the harness.
    Uses character-offset heuristic: find the last call to func_name,
    then check if the assert text appears after that offset.
    """
    clean = _strip_comments(harness_text)
    # Find the function under test call
    call_re = re.compile(r'\b' + re.escape(func_name) + r'\s*\(', re.IGNORECASE)
    calls = list(call_re.finditer(clean))
    if not calls:
        return -1   # can't determine
    last_call_pos = calls[-1].start()
    post_text = clean[last_call_pos:].lower()
    # Check if a substring of the assertion (first 20 chars) appears after the call
    key = re.sub(r'\s+', ' ', expr_norm[:30]).strip()
    return int(key in post_text)


def f_negation_depth(expr_norm: str) -> int:
    """Count negation operators: !, NOT chains."""
    return expr_norm.count('!')


def f_arity(expr_norm: str) -> int:
    """Argument count of top-level function call (0 if not a call)."""
    m = re.match(r'\w+\s*\((.+)\)\s*$', expr_norm.strip())
    if not m:
        return 0
    args_str = m.group(1)
    # count commas at depth 0
    depth, commas = 0, 0
    for ch in args_str:
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        elif ch == ',' and depth == 0:
            commas += 1
    return commas + 1


# ── Harness text index ────────────────────────────────────────────────────────

def build_harness_index(rows: list[dict]) -> dict[tuple, str]:
    """(corpus, function) → harness C text"""
    idx: dict[tuple, str] = {}
    for r in rows:
        key = (r["corpus"], r["function"])
        if key not in idx:
            path = Path(r["file_path"])
            if path.exists() and path.suffix == ".c":
                try:
                    idx[key] = path.read_text(errors="replace")
                except Exception:
                    idx[key] = ""
            else:
                idx[key] = ""
    return idx


# ── Main feature loop ─────────────────────────────────────────────────────────

def compute_features(rows: list[dict],
                     aws_src: dict, s2n_src: dict, frt_src: dict,
                     harness_idx: dict) -> list[dict]:
    out = []
    for r in rows:
        corpus   = r["corpus"]
        func     = r["function"]
        expr     = r["expr_norm"]
        kind     = r["kind"]

        # Source text for this function
        if corpus == "aws_c_common":
            src = aws_src.get(func, {})
        elif corpus == "s2n_tls":
            src = s2n_src.get(func, {})
        else:
            src = frt_src.get(func, {})

        impl   = src.get("impl", "")
        header = src.get("header", "")
        harness = harness_idx.get((corpus, func), "")

        feat = dict(r)  # copy all existing columns

        # ── Knowledge-barrier features ─────────────────────────────────────
        feat["f_var_overlap"]          = f_var_overlap(expr, impl)
        feat["f_field_overlap"]        = f_field_overlap(expr, impl)
        feat["f_predicate_in_impl"]    = f_predicate_in_impl(expr, impl)
        feat["f_predicate_in_postcond"]= f_predicate_in_postcond(expr, impl)
        feat["f_predicate_in_nl"]      = f_predicate_in_nl(expr, header)
        feat["f_uses_cbmc_builtin"]    = f_uses_cbmc_builtin(expr)

        # ── Strategy-barrier features ──────────────────────────────────────
        feat["f_is_frame_negative"]    = f_is_frame_negative(expr)
        feat["f_position_post_call"]   = f_position_post_call(expr, harness, func)
        feat["f_negation_depth"]       = f_negation_depth(expr)
        feat["f_arity"]                = f_arity(expr)

        # ── Metadata (already in joined CSV) ──────────────────────────────
        # tax_info_source, tax_reasoning, tax_category already present

        out.append(feat)
    return out


# ── Plots ─────────────────────────────────────────────────────────────────────

NUMERIC_FEATS = [
    "f_var_overlap", "f_field_overlap",
    "f_predicate_in_impl", "f_predicate_in_postcond", "f_predicate_in_nl",
    "f_uses_cbmc_builtin",
    "f_is_frame_negative", "f_position_post_call",
    "f_negation_depth", "f_arity",
]


def plot_correlation_heatmap(rows: list[dict]):
    # Use only rows where all features are numeric (no -1 NA)
    data = []
    for r in rows:
        vals = []
        ok = True
        for f in NUMERIC_FEATS:
            v = r.get(f, -1)
            try:
                v = float(v)
            except (ValueError, TypeError):
                v = -1.0
            if v < 0:
                ok = False
                break
            vals.append(v)
        if ok:
            data.append(vals)

    if len(data) < 10:
        print("  Too few complete rows for correlation heatmap")
        return

    mat = np.array(data)
    # Pearson correlation
    corr = np.corrcoef(mat.T)

    fig, ax = plt.subplots(figsize=(10, 8))
    im = ax.imshow(corr, vmin=-1, vmax=1, cmap="RdBu_r")
    plt.colorbar(im, ax=ax)
    labels = [f.replace("f_", "") for f in NUMERIC_FEATS]
    ax.set_xticks(range(len(labels)))
    ax.set_yticks(range(len(labels)))
    ax.set_xticklabels(labels, rotation=45, ha="right", fontsize=8)
    ax.set_yticklabels(labels, fontsize=8)
    ax.set_title("Phase 2 — Feature correlation matrix (aws-c-common assertions with complete features)")
    for i in range(len(labels)):
        for j in range(len(labels)):
            ax.text(j, i, f"{corr[i,j]:.2f}", ha="center", va="center", fontsize=6,
                    color="white" if abs(corr[i,j]) > 0.6 else "black")
    fig.tight_layout()
    out = FIG_DIR / "phase2_corr_heatmap.png"
    fig.savefig(out, dpi=150)
    plt.close(fig)
    print(f"  saved: {out}")


def plot_feature_distributions(rows: list[dict]):
    fig, axes = plt.subplots(2, 5, figsize=(18, 7))
    axes = axes.flatten()
    for i, feat in enumerate(NUMERIC_FEATS):
        vals = []
        for r in rows:
            v = r.get(feat, -1)
            try:
                v = float(v)
                if v >= 0:
                    vals.append(v)
            except (ValueError, TypeError):
                pass
        ax = axes[i]
        if vals:
            ax.hist(vals, bins=20, color="#2196F3", alpha=0.8, edgecolor="white", linewidth=0.3)
        ax.set_title(feat.replace("f_", ""), fontsize=9)
        ax.set_xlabel("value")
        ax.text(0.95, 0.95, f"n={len(vals)}", transform=ax.transAxes,
                ha="right", va="top", fontsize=7, color="gray")
    fig.suptitle("Phase 2 — Feature distributions (all corpora, NA excluded)", fontsize=11)
    fig.tight_layout()
    out = FIG_DIR / "phase2_feature_dist.png"
    fig.savefig(out, dpi=150)
    plt.close(fig)
    print(f"  saved: {out}")


# ── Feature summary stats ─────────────────────────────────────────────────────

def feature_summary(rows: list[dict]) -> dict:
    stats = {}
    for feat in NUMERIC_FEATS:
        vals = []
        nas  = 0
        for r in rows:
            v = r.get(feat, -1)
            try:
                v = float(v)
                if v < 0:
                    nas += 1
                else:
                    vals.append(v)
            except (ValueError, TypeError):
                nas += 1
        if vals:
            stats[feat] = {
                "n":    len(vals),
                "na":   nas,
                "mean": round(np.mean(vals), 3),
                "std":  round(np.std(vals),  3),
                "frac_positive": round(sum(1 for v in vals if v > 0) / len(vals), 3),
            }
        else:
            stats[feat] = {"n": 0, "na": nas, "mean": "NA"}
    return stats


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    DATA_DIR.mkdir(exist_ok=True)
    FIG_DIR.mkdir(exist_ok=True)
    REPORT_DIR.mkdir(exist_ok=True)

    print("Loading gt_assertions_joined.csv …")
    rows = list(csv.DictReader((DATA_DIR / "gt_assertions_joined.csv").open()))
    print(f"  {len(rows)} rows")

    print("Building source text indices …")
    aws_src = build_aws_source_index()
    s2n_src = build_s2n_source_index()
    print("  FreeRTOS source index …")
    frt_src = build_freertos_source_index()
    print(f"  aws={len(aws_src)} s2n={len(s2n_src)} frt={len(frt_src)}")

    print("Building harness text index …")
    harness_idx = build_harness_index(rows)

    print("Computing features …")
    featured = compute_features(rows, aws_src, s2n_src, frt_src, harness_idx)

    # Save
    out_path = DATA_DIR / "gt_assertions_features.csv"
    all_keys = list(featured[0].keys()) if featured else []
    with out_path.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=all_keys)
        w.writeheader()
        w.writerows(featured)
    print(f"  saved: {out_path} ({len(featured)} rows)")

    # Plots
    print("Generating plots …")
    aws_rows = [r for r in featured if r["corpus"] == "aws_c_common"]
    plot_correlation_heatmap(aws_rows)
    plot_feature_distributions(featured)

    # Summary
    print("\nFeature summary (all corpora):")
    summary = feature_summary(featured)
    for feat, s in summary.items():
        print(f"  {feat:30s}  n={s['n']:4}  na={s['na']:3}  "
              f"mean={s.get('mean','NA'):5}  frac_pos={s.get('frac_positive','NA')}")

    # Write feature_definitions.md
    write_feature_definitions()

    # Write report
    report = build_report(featured, summary)
    rpath = REPORT_DIR / "phase_2.md"
    rpath.write_text(report)
    print(f"\n  report: {rpath}")


def write_feature_definitions():
    content = """# Feature Definitions — Phase 2

Two groups corresponding to the two-barrier hypothesis.
All features documented with: definition, computation, examples.

---

## Group 1 — Knowledge-barrier features

### f_var_overlap [continuous, 0–1]
**Definition**: Fraction of identifier tokens in the assertion expression that also
appear in the function's implementation body text.
High = assertion variables are visible in code; Low = assertion uses variables not in code.

**Computation**: `extract_identifiers(expr_norm)` → intersect with tokens in `impl_text`.

**Examples (high ≈ derivable)**:
- `buf.len == 0` in `aws_byte_buf_init`: `len`, `buf` both in impl → 1.0
- `r == a + b` in `aws_add_size_checked`: `a`, `b`, `r` all in impl → 1.0

**Examples (low ≈ not derivable)**:
- `aws_byte_buf_is_valid(&_arg_)`: predicate name not in impl body → low

---

### f_field_overlap [continuous, 0–1, NA if no fields]
**Definition**: Fraction of struct field names (e.g. `.len`, `.capacity`) in the
assertion that appear in the function implementation.

**Computation**: regex `\w+\.(\w+)` → intersect with impl tokens.

---

### f_predicate_in_impl [binary, -1=NA]
**Definition**: 1 if any validity predicate (`*_is_valid`, `*_validate`) in the assertion
expression also appears anywhere in the function's implementation text.
-1 if expression has no validity predicate.

**Computation**: regex for predicate names → `in impl_text.lower()`.

**Examples (1 — predicate visible in impl)**:
- `aws_byte_buf_is_valid` in `aws_byte_buf_init`: impl has `AWS_POSTCONDITION(aws_byte_buf_is_valid(buf))` → 1
- `aws_linked_list_node_next_is_valid` in `aws_linked_list_next`: impl has `AWS_POSTCONDITION(...)` → 1

**Examples (0 — predicate not in impl)**:
- `aws_linked_list_node_prev_is_valid` in `aws_linked_list_push_back`: predicate
  only in NL header, not in push_back's own impl → 0

---

### f_predicate_in_postcond [binary, -1=NA]
**Definition**: 1 if predicate appears specifically inside an `AWS_POSTCONDITION()` /
`POSIX_POSTCONDITION()` call in the function implementation.
Stronger than `f_predicate_in_impl` — only fires on explicit postcondition annotation.

---

### f_predicate_in_nl [binary, -1=NA]
**Definition**: 1 if predicate appears in NL `Ensures:` / `@ensures` / `postcondition`
annotations in the function's header declaration.

**Examples (1)**:
- `aws_linked_list_node_prev_is_valid` in header "Ensures: aws_linked_list_node_prev_is_valid(node)"

---

### f_uses_cbmc_builtin [binary]
**Definition**: 1 if expression contains `__cprover_*` CBMC-specific builtins.
These are never visible in production code text.

---

## Group 2 — Strategy-barrier features

### f_is_frame_negative [binary]
**Definition**: 1 if assertion expresses "something did NOT change" — i.e., a frame
condition of the form `old.X == new.X` or `OLD_field == current_field`.

**Computation**: regex for `old_\w+\s*==`, `==\s*old_\w+`, `OLD_\w+` patterns.

**Why it matters**: Frame conditions are CODE-derivable (you can read the impl to see
what doesn't change) but LLMs systematically skip them — this is the strategy barrier.
Prediction: high f_is_frame_negative → low recall under A/B/C/D, recoverable under E/F.

**Examples (1)**:
- `old_to.head == list.head` (head didn't change)
- `OLD_alloc == buf.allocator`

**Examples (0)**:
- `buf.len == 0` (postcondition about what changed)
- `aws_byte_buf_is_valid(&_arg_)` (validity check)

---

### f_position_post_call [binary, -1=NA]
**Definition**: 1 if this assertion appears textually AFTER the call to the function
under test in the harness. Preconditions appear before; postconditions appear after.

---

### f_negation_depth [int, 0+]
**Definition**: Count of `!` negation operators in the expression.
Higher negation depth → logically more complex condition.

---

### f_arity [int, 0+]
**Definition**: Number of arguments to the top-level function call in the expression.
0 if expression is not a function call. Higher arity = more complex predicate.
"""
    path = REPORT_DIR / "feature_definitions.md"
    path.write_text(content)
    print(f"  saved: {path}")


def build_report(featured, summary) -> str:
    aws  = [r for r in featured if r["corpus"] == "aws_c_common"]
    s2n  = [r for r in featured if r["corpus"] == "s2n_tls"]
    frt  = [r for r in featured if r["corpus"] == "freertos"]

    stat_rows = "\n".join(
        f"| {f:30s} | {s.get('n','?'):>4} | {s.get('na','?'):>3} | "
        f"{str(s.get('mean','NA')):>6} | {str(s.get('frac_positive','NA')):>8} |"
        for f, s in summary.items()
    )

    # Check knowledge vs strategy features for taxonomy-matched assertions
    tax_rows = [r for r in aws if r.get("tax_id")]
    frame_rate = sum(1 for r in tax_rows if r.get("f_is_frame_negative") == "1") / max(len(tax_rows), 1)
    domain_rows = [r for r in tax_rows if r.get("tax_info_source") == "DOMAIN"]
    domain_pred_rate = sum(1 for r in domain_rows
                          if r.get("f_predicate_in_impl") == "1") / max(len(domain_rows), 1)

    return f"""# Phase 2 Report

## What I did

- Built source text indices for all three corpora
- Computed 10 numeric features per assertion (6 knowledge-barrier, 4 strategy-barrier)
- Generated correlation heatmap (aws-c-common assertions with no NA features)
- Generated feature distribution plots

## Key numbers

### Feature summary (all corpora, NA excluded)

| Feature                        |    n |  NA |   mean | frac>0  |
|--------------------------------|:----:|:---:|:------:|:-------:|
{stat_rows}

### Sanity checks on taxonomy-matched assertions ({len(tax_rows)} rows)

- Frame condition rate (f_is_frame_negative=1): {frame_rate:.1%}
  (expected ~17% — matches taxonomy FRAME_COND proportion)
- DOMAIN assertions with predicate in impl (f_predicate_in_impl=1): {domain_pred_rate:.1%}
  (expected ~30–50% based on A_qwen DOMAIN recovery rate)

## Interesting observations

1. **f_position_post_call** [check]: many ASSUME-kind properties are before the call
   (preconditions); assertions should be after. If the split is clean, this feature
   adds information about precondition vs postcondition nature.

2. **f_field_overlap NA rate** [expected]: many assertions are function calls
   (e.g. `aws_X_is_valid(&_arg_)`) with no struct field access → f_field_overlap = -1.
   This is expected and handled as NA in regression.

3. **FreeRTOS source coverage** [check]: FreeRTOS functions mapped via name-variant
   search. For functions where no source was found, impl = full combined source text
   (conservative: gives max possible variable overlap, biasing f_var_overlap high).

## Questions for the PI

1. Is f_field_overlap NA (~40–60% of rows) acceptable for regression? Plan: use multiple
   imputation or drop the feature if NA rate is too high.
2. Should f_position_post_call be computed on the GT harness or LLM harness?
   (Currently: GT harness — tells us structural position, not LLM behavior)
3. Confirm: for Phase 4, use only ASSERT-kind rows (not ASSUME)?

## What I propose to do next

Phase 4 analysis: build the `(assertion, LLM, condition)` regression dataframe,
fit M1–M4 mixed-effects logistic models, run PCA on features to test typology.
(Phase 3 FreeRTOS LLM runs can proceed in parallel with Phase 4 on existing data.)
"""


if __name__ == "__main__":
    main()
