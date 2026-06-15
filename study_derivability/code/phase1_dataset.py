#!/usr/bin/env python3
"""
Phase 1 — Build the assertion-level GT dataset.

Strategy per corpus:
  aws-c-common : CBMC --show-properties --xml-ui on each GT harness
                 (exact match with existing cross_verify taxonomy)
  s2n-tls      : regex extraction from gt_harness strings in JSON
  FreeRTOS     : regex extraction from .c harness files

Output:
  data/gt_assertions.csv              all GT assertions, all three corpora
  data/gt_assertions_joined.csv       ↑ joined with 191-row taxonomy labels
  reports/phase_1.md                  Phase 1 report
"""

import csv
import json
import re
import subprocess
import sys
import xml.etree.ElementTree as ET
from collections import defaultdict
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
sys.path.insert(0, str(Path(__file__).parent.parent.parent /
                       "experiment_aws_cbmc" / "scripts"))

from config import (
    AWS_CORPUS_DIR, AWS_RESULTS, AWS_EVAL, TAXONOMY_CSV,
    S2N_DATASET, S2N_CORPUS_DIR,
    FREERTOS_CORPUS_DIR, FREERTOS_SHORTLIST,
    DATA_DIR, REPORT_DIR,
)

# ── Normalization (mirrors cross_verify.py normalize_prop) ────────────────────

def norm(s: str) -> str:
    s = s.lower().strip()
    s = s.replace('->', '.')
    s = re.sub(r'\s+', ' ', s)
    s = re.sub(r'^assertion\s*', '', s)   # strip CBMC description prefix
    s = re.sub(r'^assert\s+', '', s)
    # X_old → old_X
    s = re.sub(r'\b([a-z][a-z0-9]*)_old\b', r'old_\1', s)
    # _is_valid(&identifier) → _is_valid(&_arg_)
    s = re.sub(r'(?<=_is_valid\()&[a-z]\w*', '&_arg_', s)
    # fuzzy: strip simple local variable prefix (non-old_ prefix before .)
    s = re.sub(r'\b(?!old_)[a-z][a-z0-9_]*\.', '_v_.', s)
    return s.strip()


# ── Regex extraction for non-aws corpora ─────────────────────────────────────

def _strip_comments(code: str) -> str:
    code = re.sub(r'//[^\n]*', ' ', code)
    code = re.sub(r'/\*.*?\*/', ' ', code, flags=re.DOTALL)
    return code


def _extract_call_args(code: str, func_name: str) -> list[tuple[int, str]]:
    """
    Extract argument strings from calls to `func_name(...)` in code.
    Returns list of (approx_line, raw_arg_str).
    Handles nested parentheses correctly.
    """
    results = []
    pattern = re.compile(r'\b' + re.escape(func_name) + r'\s*\(', re.MULTILINE)
    for m in pattern.finditer(code):
        start = m.end()   # position right after the opening '('
        depth = 1
        i = start
        while i < len(code) and depth > 0:
            if code[i] == '(':
                depth += 1
            elif code[i] == ')':
                depth -= 1
            i += 1
        if depth == 0:
            arg = code[start:i - 1].strip()
            line = code[:m.start()].count('\n') + 1
            results.append((line, arg))
    return results


def extract_from_source(code: str) -> list[dict]:
    """
    Extract assert() and __CPROVER_assume() from C source text.
    Returns list of {line, kind, expr_raw, expr_norm}.
    """
    clean = _strip_comments(code)
    rows = []
    for (line, arg) in _extract_call_args(clean, 'assert'):
        # Exclude assert-like macros with two args (e.g. static_assert)
        if arg and ',' not in arg.split('(')[0]:
            rows.append({
                'line': line, 'kind': 'ASSERT',
                'expr_raw': arg, 'expr_norm': norm(arg),
            })
    for (line, arg) in _extract_call_args(clean, '__CPROVER_assume'):
        rows.append({
            'line': line, 'kind': 'ASSUME',
            'expr_raw': arg, 'expr_norm': norm(arg),
        })
    for (line, arg) in _extract_call_args(clean, '__CPROVER_assert'):
        rows.append({
            'line': line, 'kind': 'ASSERT',
            'expr_raw': arg.split(',')[0].strip() if ',' in arg else arg,
            'expr_norm': norm(arg.split(',')[0].strip() if ',' in arg else arg),
        })
    # Deduplicate by (kind, expr_norm), keep first occurrence
    seen: set[tuple] = set()
    deduped = []
    for r in rows:
        key = (r['kind'], r['expr_norm'])
        if key not in seen:
            seen.add(key)
            deduped.append(r)
    return deduped


# ── CBMC extraction for aws-c-common ─────────────────────────────────────────

try:
    from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS, PROOFDIR, SRCDIR
    CBMC_CONFIGS_AVAILABLE = True
except ImportError:
    CBMC_CONFIGS_AVAILABLE = False
    FUNC_CONFIGS = {}


def extract_cbmc_properties(func_name: str, harness_path: Path,
                             timeout: int = 60) -> list[dict]:
    """
    Run CBMC --show-properties on harness and return harness-originated properties.
    Returns list of {line, kind, expr_raw, expr_norm}.
    """
    if not CBMC_CONFIGS_AVAILABLE or func_name not in FUNC_CONFIGS:
        return []  # fall back to regex for this function

    cfg = FUNC_CONFIGS[func_name]
    all_sources = (cfg["proof_sources"] + [harness_path] + cfg["project_sources"])
    cmd = (
        ["cbmc"] + COMMON_FLAGS + cfg["defines"] +
        ["--show-properties", "--xml-ui"] +
        ["--function", f"{func_name}_harness"] +
        [str(s) for s in all_sources]
    )

    try:
        proc = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except (subprocess.TimeoutExpired, FileNotFoundError):
        return []

    if "PARSING ERROR" in proc.stdout or "fatal error" in proc.stderr:
        return []

    results = []
    harness_stem = harness_path.stem.lower()
    try:
        m = re.search(r'<cprover>.*?</cprover>', proc.stdout, re.DOTALL)
        if not m:
            return []
        root = ET.fromstring(m.group(0))
        for prop in root.findall("property"):
            desc_el  = prop.find("description")
            loc_el   = prop.find("location")
            file_attr = loc_el.get("file", "") if loc_el is not None else ""
            # Only keep properties from the harness file itself
            if "harness" not in file_attr.lower():
                continue
            desc = desc_el.text.strip() if desc_el is not None and desc_el.text else ""
            cls  = prop.get("class", "")
            line = int(loc_el.get("line", 0)) if loc_el is not None else 0
            kind = "ASSERT" if "assertion" in cls.lower() else "ASSUME"
            results.append({
                'line': line, 'kind': kind,
                'expr_raw': desc, 'expr_norm': norm(desc),
            })
    except ET.ParseError:
        pass

    # Deduplicate by (kind, expr_norm)
    seen: set[tuple] = set()
    deduped = []
    for r in results:
        key = (r['kind'], r['expr_norm'])
        if key not in seen:
            seen.add(key)
            deduped.append(r)
    return deduped


# ── Corpus-level harness enumeration ─────────────────────────────────────────

def aws_harnesses() -> list[dict]:
    """Return [{func, file_path}] for the PI's 84-function aws-c-common set."""
    result_dir = AWS_RESULTS / "feedback_loop_A_claude"
    pi_funcs = sorted(d.name for d in result_dir.iterdir() if d.is_dir()) \
        if result_dir.exists() else []

    clone_index: dict[str, Path] = {}
    for h in AWS_CORPUS_DIR.rglob("*_harness.c"):
        func = h.stem.removesuffix("_harness")
        clone_index[func] = h

    out = []
    for func in pi_funcs:
        path = clone_index.get(func)
        if path:
            out.append({"func": func, "file_path": path})
        else:
            print(f"  WARNING: no clone harness for {func}")
    return out


def s2n_harnesses() -> list[dict]:
    """Return [{func, gt_code}] from JSON dataset."""
    data = json.loads(S2N_DATASET.read_text())
    return [{"func": k, "gt_code": v["gt_harness"], "file_path": "s2n_stuffer_dataset.json"}
            for k, v in data.items()]


def freertos_harnesses() -> list[dict]:
    """Return [{func, file_path}] for all unique FreeRTOS harnesses."""
    seen: set[str] = set()
    out = []
    for h in sorted(FREERTOS_CORPUS_DIR.rglob("*_harness.c")):
        func = h.stem.removesuffix("_harness")
        if func not in seen:
            seen.add(func)
            out.append({"func": func, "file_path": h})
    return out


# ── Main extraction loop ──────────────────────────────────────────────────────

def extract_corpus(corpus: str, harnesses: list[dict],
                   use_cbmc: bool) -> list[dict]:
    rows = []
    n_cbmc, n_regex, n_empty = 0, 0, 0
    for i, h in enumerate(harnesses, 1):
        func = h["func"]

        if use_cbmc:
            props = extract_cbmc_properties(func, h["file_path"])
            if props:
                n_cbmc += 1
                method = "cbmc"
            else:
                # fallback: regex on source file
                code = h["file_path"].read_text(errors="replace") \
                    if "file_path" in h and isinstance(h["file_path"], Path) else ""
                props = extract_from_source(code)
                n_regex += 1
                method = "regex_fallback"
        else:
            code = h.get("gt_code") or \
                (h["file_path"].read_text(errors="replace")
                 if isinstance(h.get("file_path"), Path) else "")
            props = extract_from_source(code)
            n_regex += 1
            method = "regex"

        if not props:
            n_empty += 1

        for p in props:
            rows.append({
                "id":         f"{corpus[:3].upper()}_{func}_{p['line']}_{p['kind'][0]}",
                "corpus":     corpus,
                "function":   func,
                "file_path":  str(h.get("file_path", "")),
                "line":       p["line"],
                "kind":       p["kind"],
                "expr_raw":   p["expr_raw"],
                "expr_norm":  p["expr_norm"],
                "method":     method,
            })

        if i % 20 == 0 or i == len(harnesses):
            print(f"  [{corpus}] {i}/{len(harnesses)} done")

    print(f"  [{corpus}] cbmc={n_cbmc} regex={n_regex} empty={n_empty}")
    return rows


# ── Unique ID assignment ──────────────────────────────────────────────────────

def assign_ids(rows: list[dict]) -> list[dict]:
    for i, r in enumerate(rows, 1):
        r["id"] = f"A{i:04d}"
    return rows


# ── Taxonomy join ─────────────────────────────────────────────────────────────

def load_taxonomy() -> list[dict]:
    return list(csv.DictReader(TAXONOMY_CSV.open()))


def join_taxonomy(assertion_rows: list[dict],
                  taxonomy: list[dict]) -> list[dict]:
    """
    Join assertion_rows with taxonomy on (function, expr_norm).
    taxonomy.property_text is normalised with the same norm() function.
    Returns assertion_rows augmented with taxonomy columns (or None).
    """
    # Index taxonomy by (func, norm)
    tax_index: dict[tuple, dict] = {}
    for t in taxonomy:
        key = (t["func"], norm(t["property_text"]))
        tax_index[key] = t

    joined, matched, unmatched = [], 0, 0
    for r in assertion_rows:
        key = (r["function"], r["expr_norm"])
        t = tax_index.get(key)
        if t:
            matched += 1
            r["tax_id"]          = t["id"]
            r["tax_info_source"] = t["info_source"]
            r["tax_reasoning"]   = t["reasoning"]
            r["tax_category"]    = t["category"]
        else:
            unmatched += 1
            r["tax_id"]          = ""
            r["tax_info_source"] = ""
            r["tax_reasoning"]   = ""
            r["tax_category"]    = ""
        joined.append(r)

    print(f"  Taxonomy join: {matched} matched, {unmatched} unmatched "
          f"(of {len(taxonomy)} taxonomy rows, {len(assertion_rows)} assertions)")
    return joined, matched


# ── Summary stats ─────────────────────────────────────────────────────────────

def corpus_stats(rows: list[dict]) -> dict:
    funcs   = {r["function"] for r in rows}
    asserts = [r for r in rows if r["kind"] == "ASSERT"]
    assumes = [r for r in rows if r["kind"] == "ASSUME"]
    return {
        "n_functions":  len(funcs),
        "n_assertions": len(asserts),
        "n_assumes":    len(assumes),
        "total":        len(rows),
        "mean_per_func": round(len(asserts) / max(len(funcs), 1), 1),
    }


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    DATA_DIR.mkdir(exist_ok=True)
    REPORT_DIR.mkdir(exist_ok=True)

    all_rows: list[dict] = []

    # ── aws-c-common ──────────────────────────────────────────────────────────
    print("── aws-c-common (CBMC + regex fallback) ──")
    aws_h = aws_harnesses()
    aws_rows = extract_corpus("aws_c_common", aws_h, use_cbmc=True)
    print(f"  extracted {len(aws_rows)} property instances from {len(aws_h)} harnesses")
    all_rows.extend(aws_rows)

    # ── s2n-tls ───────────────────────────────────────────────────────────────
    print("── s2n-tls (regex) ──")
    s2n_h = s2n_harnesses()
    s2n_rows = extract_corpus("s2n_tls", s2n_h, use_cbmc=False)
    print(f"  extracted {len(s2n_rows)} property instances from {len(s2n_h)} harnesses")
    all_rows.extend(s2n_rows)

    # ── FreeRTOS-Plus-TCP ─────────────────────────────────────────────────────
    print("── FreeRTOS-Plus-TCP (regex) ──")
    frt_h = freertos_harnesses()
    frt_rows = extract_corpus("freertos", frt_h, use_cbmc=False)
    print(f"  extracted {len(frt_rows)} property instances from {len(frt_h)} harnesses")
    all_rows.extend(frt_rows)

    # Assign sequential IDs
    all_rows = assign_ids(all_rows)

    # Save raw CSV
    raw_path = DATA_DIR / "gt_assertions.csv"
    fieldnames = ["id", "corpus", "function", "file_path", "line",
                  "kind", "expr_raw", "expr_norm", "method"]
    with raw_path.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=fieldnames)
        w.writeheader()
        w.writerows(all_rows)
    print(f"\n  saved: {raw_path} ({len(all_rows)} rows)")

    # Taxonomy join (aws-c-common only)
    print("\n── Taxonomy join ──")
    taxonomy = load_taxonomy()
    aws_only  = [r for r in all_rows if r["corpus"] == "aws_c_common"]
    non_aws   = [r for r in all_rows if r["corpus"] != "aws_c_common"]

    aws_joined, n_matched = join_taxonomy(aws_only, taxonomy)
    join_rate = n_matched / len(taxonomy) if taxonomy else 0
    print(f"  Join rate: {n_matched}/{len(taxonomy)} = {join_rate:.1%}")

    # Combine and save joined CSV
    joined_all = aws_joined + non_aws   # non-aws rows have empty tax fields
    for r in non_aws:
        r.update({"tax_id": "", "tax_info_source": "", "tax_reasoning": "", "tax_category": ""})

    joined_path = DATA_DIR / "gt_assertions_joined.csv"
    joined_fields = fieldnames + ["tax_id", "tax_info_source", "tax_reasoning", "tax_category"]
    with joined_path.open("w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=joined_fields)
        w.writeheader()
        w.writerows(joined_all)
    print(f"  saved: {joined_path}")

    # Per-corpus stats
    print("\n── Summary ──")
    for corpus in ["aws_c_common", "s2n_tls", "freertos"]:
        rows = [r for r in all_rows if r["corpus"] == corpus]
        s = corpus_stats(rows)
        print(f"  {corpus:20s}  funcs={s['n_functions']:3}  "
              f"asserts={s['n_assertions']:4}  assumes={s['n_assumes']:4}  "
              f"mean_asserts/func={s['mean_per_func']}")

    # Check unmatched taxonomy rows
    matched_tax_ids = {r["tax_id"] for r in aws_joined if r["tax_id"]}
    unmatched_tax = [t for t in taxonomy if t["id"] not in matched_tax_ids]
    if unmatched_tax:
        print(f"\n  Unmatched taxonomy rows ({len(unmatched_tax)}):")
        for t in unmatched_tax[:10]:
            print(f"    [{t['category']}] {t['func']}: {t['property_text'][:60]}")
        if len(unmatched_tax) > 10:
            print(f"    ... and {len(unmatched_tax) - 10} more")

    # Write report
    report = build_report(all_rows, taxonomy, n_matched, join_rate, unmatched_tax)
    rpath = REPORT_DIR / "phase_1.md"
    rpath.write_text(report)
    print(f"\n  report: {rpath}")


def build_report(all_rows, taxonomy, n_matched, join_rate, unmatched_tax) -> str:
    stats = {}
    for corpus in ["aws_c_common", "s2n_tls", "freertos"]:
        rows = [r for r in all_rows if r["corpus"] == corpus]
        stats[corpus] = corpus_stats(rows)

    unmatched_examples = "\n".join(
        f"  [{t['category']:15s}] {t['func']}: {t['property_text'][:60]}"
        for t in unmatched_tax[:15]
    )

    return f"""# Phase 1 Report

## What I did

- Extracted GT assertions using CBMC --show-properties for aws-c-common (with
  regex fallback for any function missing from FUNC_CONFIGS).
- Extracted GT assertions using regex for s2n-tls (from JSON) and FreeRTOS (from .c files).
- Applied uniform normalization mirroring cross_verify.py's normalize_prop().
- Joined aws-c-common assertions with the 191-row taxonomy on (function, expr_norm).

## Key numbers

### Assertion counts per corpus

| Corpus             | Functions | Assertions | Assumes | Mean assert/func |
|--------------------|:---------:|:----------:|:-------:|:----------------:|
| aws-c-common       | {stats['aws_c_common']['n_functions']:>9} | {stats['aws_c_common']['n_assertions']:>10} | {stats['aws_c_common']['n_assumes']:>7} | {stats['aws_c_common']['mean_per_func']:>16} |
| s2n-tls            | {stats['s2n_tls']['n_functions']:>9} | {stats['s2n_tls']['n_assertions']:>10} | {stats['s2n_tls']['n_assumes']:>7} | {stats['s2n_tls']['mean_per_func']:>16} |
| FreeRTOS-Plus-TCP  | {stats['freertos']['n_functions']:>9} | {stats['freertos']['n_assertions']:>10} | {stats['freertos']['n_assumes']:>7} | {stats['freertos']['mean_per_func']:>16} |
| **Total**          | {stats['aws_c_common']['n_functions']+stats['s2n_tls']['n_functions']+stats['freertos']['n_functions']:>9} | {stats['aws_c_common']['n_assertions']+stats['s2n_tls']['n_assertions']+stats['freertos']['n_assertions']:>10} | | |

### Taxonomy join (aws-c-common only)

- Taxonomy rows: {len(taxonomy)}
- Matched: {n_matched} ({join_rate:.1%})
- Unmatched: {len(unmatched_tax)}

## Interesting observations

1. **Join rate** [check]: {join_rate:.1%} of the 191 taxonomy rows match. Unmatched rows
   are listed below — likely due to normalization differences or CBMC vs regex extraction.

2. **FreeRTOS assertion density** [likely real]: FreeRTOS harnesses are significantly
   larger and may have more complex assertion patterns including protocol-specific checks.

3. **Normalization consistency** [important]: Phase 2 features must use the same norm()
   function defined here to ensure correct join with the taxonomy recall labels from Phase 0.

## Unmatched taxonomy rows (first {min(15, len(unmatched_tax))})

```
{unmatched_examples if unmatched_tax else '(none — all matched)'}
```

## Questions for the PI

1. Are the assertion counts reasonable? (Expected: ~400–600 total assertions)
2. Should assumes be included in the Phase 4 regression, or assertions only?
3. For FreeRTOS: confirm all 79 harnesses should be used (or apply the 40-function shortlist)?

## What I propose to do next

Begin Phase 2 feature engineering on `gt_assertions_joined.csv`.
"""


if __name__ == "__main__":
    main()
