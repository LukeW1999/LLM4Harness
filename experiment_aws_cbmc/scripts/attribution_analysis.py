#!/usr/bin/env python3
"""
attribution_analysis.py -- For each silenced mutant, classify as:
  knowledge_gap: GT-catching assertion never appeared in ANY LLM iteration
  sacrifice:     GT-catching assertion appeared in >=1 LLM iteration but not final

Usage:
    python3 attribution_analysis.py --dataset feedback_loop_A_gptoss120b
"""

import json, re, argparse
from pathlib import Path
from collections import defaultdict

SCRIPT_DIR     = Path(__file__).parent
EXPERIMENT_DIR = Path("/root/experiment_aws_cbmc")
EVAL_DIR       = EXPERIMENT_DIR / "evaluation"
RESULTS_DIR    = EXPERIMENT_DIR / "results"
GT_PROOFS_DIR  = Path("/root/aws-c-common/verification/cbmc/proofs")

OVERLAP_THRESHOLD = 0.45  # Jaccard similarity to count as "appeared"


def tokenize(s):
    s = re.sub(r'assert\s*\(|\)', ' ', s)
    tokens = re.findall(r'[a-zA-Z_][a-zA-Z0-9_]*|[0-9]+', s)
    stopwords = {'assert', 'CPROVER_assert', 'int', 'size_t', 'return',
                 'if', 'else', 'true', 'false', 'NULL', 'void'}
    return set(t for t in tokens if t not in stopwords and len(t) > 1)


def jaccard(a, b):
    if not a and not b:
        return 1.0
    if not a or not b:
        return 0.0
    return len(a & b) / len(a | b)


def extract_asserts(path):
    if not path or not path.exists():
        return []
    text = path.read_text(errors='replace')
    asserts = []
    for line in text.splitlines():
        s = line.strip()
        if 'assert(' in s or '__CPROVER_assert(' in s:
            if re.search(r'assert\s*\(\s*(false|0)\s*[,)]', s):
                continue
            asserts.append(s)
    return asserts


def extract_assumes(path):
    """Extract __CPROVER_assume(...) and ASSUME_VALID_MEMORY* macro calls."""
    if not path or not path.exists():
        return []
    text = path.read_text(errors='replace')
    assumes = []
    for line in text.splitlines():
        s = line.strip()
        if '__CPROVER_assume(' in s or 'ASSUME_VALID_MEMORY' in s or 'ensure_' in s:
            if not s.startswith('//') and not s.startswith('*'):
                assumes.append(s)
    return assumes


def detect_aoc(llm_dataset, func, llm_iters):
    """
    Detect Assumption Over-Constraint (AOC) pattern:
    Returns True if LLM final harness has explicit size/value bounds
    (__CPROVER_assume(x <= N)) while GT harness uses ASSUME_VALID_MEMORY macros.

    AOC = LLM narrows state space via explicit bounds to achieve CBMC convergence.
    """
    # Get GT harness assumes
    gt_path = GT_PROOFS_DIR / func / f"{func}_harness.c"
    gt_assumes = extract_assumes(gt_path)
    gt_text = gt_path.read_text(errors='replace') if gt_path.exists() else ""

    # GT uses proof-helper macros (not raw bounds)
    gt_uses_macro = any('ASSUME_VALID_MEMORY' in a or 'ensure_' in a for a in gt_assumes)

    # Get LLM final harness assumes
    if not llm_iters:
        return False, []
    max_iter = max(llm_iters.keys())
    llm_func_dir = RESULTS_DIR / llm_dataset / func
    final_harness_path = llm_func_dir / f"iter_{max_iter}_harness.c"
    llm_assumes = extract_assumes(final_harness_path)

    # LLM has explicit size bounds
    bound_pattern = re.compile(r'__CPROVER_assume\s*\(.+<=\s*\w+|__CPROVER_assume\s*\(.+<\s*[A-Z_]+\)')
    llm_explicit_bounds = [a for a in llm_assumes if bound_pattern.search(a)]

    # Also check for malloc-as-concrete-alloc (LLM uses malloc, GT uses symbolic)
    llm_text = final_harness_path.read_text(errors='replace') if final_harness_path.exists() else ""
    llm_uses_malloc = 'malloc(' in llm_text or 'bounded_malloc(' in llm_text
    gt_uses_malloc = 'malloc(' in gt_text or 'bounded_malloc(' in gt_text

    is_aoc = (
        (gt_uses_macro and llm_explicit_bounds) or
        (gt_uses_macro and llm_uses_malloc and not gt_uses_malloc)
    )

    evidence = llm_explicit_bounds[:3]
    if gt_uses_macro and llm_uses_malloc and not gt_uses_malloc:
        evidence.append("[malloc-vs-ASSUME_VALID_MEMORY]")

    return is_aoc, evidence


def get_gt_asserts(func):
    p = GT_PROOFS_DIR / func / f"{func}_harness.c"
    return extract_asserts(p)


def get_llm_iter_asserts(dataset, func):
    func_dir = RESULTS_DIR / dataset / func
    iters = {}
    if func_dir.exists():
        for p in sorted(func_dir.glob("iter_*_harness.c")):
            m = re.search(r'iter_(\d+)_harness', p.name)
            if m:
                iters[int(m.group(1))] = extract_asserts(p)
    return iters


def classify_gt_assert(gt_assert, llm_iters):
    if not llm_iters:
        return False, False
    gt_tok = tokenize(gt_assert)
    if not gt_tok:
        return False, False
    max_iter = max(llm_iters.keys())
    appeared_any = False
    appeared_final = False
    for iter_num, asserts in llm_iters.items():
        for llm_a in asserts:
            if jaccard(gt_tok, tokenize(llm_a)) >= OVERLAP_THRESHOLD:
                appeared_any = True
                if iter_num == max_iter:
                    appeared_final = True
    return appeared_any, appeared_final


def analyze(dataset):
    # Oracle files use cbmc_ prefix; LLM results dir uses base dataset name
    llm_dataset = dataset.removeprefix("cbmc_")
    oracle_path = EVAL_DIR / f"mutation_oracle_cbmc_{llm_dataset}.json"
    if not oracle_path.exists():
        print(f"Oracle file not found: {oracle_path}")
        return

    data = json.loads(oracle_path.read_text())
    results = data.get("results", [])
    silenced = [r for r in results if r.get("silenced")]

    if not silenced:
        print(f"No silenced mutants in {oracle_path.name}")
        return

    print(f"\n{'='*60}")
    print(f"  Attribution Analysis: {dataset}")
    print(f"  Total results: {len(results)} | Silenced: {len(silenced)}")
    print(f"{'='*60}")

    by_func = defaultdict(list)
    for r in silenced:
        by_func[r["func"]].append(r)

    summary = {"knowledge_gap": 0, "sacrifice": 0, "aoc": 0, "unknown": 0}
    func_summary = {}

    for func, mutants in sorted(by_func.items()):
        gt_asserts = get_gt_asserts(func)
        llm_iters  = get_llm_iter_asserts(llm_dataset, func)
        n_iters    = len(llm_iters)

        gt_attr = {}
        for ga in gt_asserts:
            any_, final_ = classify_gt_assert(ga, llm_iters)
            gt_attr[ga] = (any_, final_)

        kg_asserts    = [ga for ga, (a, f) in gt_attr.items() if not a]
        sac_asserts   = [ga for ga, (a, f) in gt_attr.items() if a and not f]
        present_final = [ga for ga, (a, f) in gt_attr.items() if f]

        n_sil = len(mutants)
        if sac_asserts:
            attribution = "SACRIFICE-CANDIDATE"
            summary["sacrifice"] += n_sil
        elif kg_asserts:
            attribution = "KNOWLEDGE-GAP"
            summary["knowledge_gap"] += n_sil
        else:
            # All GT assertions present — check for AOC pattern
            is_aoc, aoc_evidence = detect_aoc(llm_dataset, func, llm_iters)
            if is_aoc:
                attribution = "ASSUMPTION-OVER-CONSTRAINT"
                summary["aoc"] += n_sil
            else:
                attribution = "UNKNOWN (structural)"
                summary["unknown"] += n_sil

        func_summary[func] = {
            "silenced": n_sil,
            "n_iters": n_iters,
            "gt_asserts": len(gt_asserts),
            "kg_asserts": len(kg_asserts),
            "sac_asserts": len(sac_asserts),
            "present_final": len(present_final),
            "attribution": attribution,
            # Assertion text lists for cloze test and downstream analysis
            "kg_assertion_texts": kg_asserts,
            "sac_assertion_texts": sac_asserts,
        }

        print(f"\n  {func} ({n_sil} silenced, {n_iters} LLM iters)")
        print(f"    GT asserts total       : {len(gt_asserts)}")
        print(f"    KG (never in LLM)      : {len(kg_asserts)}")
        print(f"    SAC (appeared, deleted): {len(sac_asserts)}")
        print(f"    Present in final iter  : {len(present_final)}")
        print(f"    Attribution -> {attribution}")
        for ga in kg_asserts[:3]:
            print(f"      [KG] {ga[:88]}")
        for ga in sac_asserts[:2]:
            print(f"      [SAC] {ga[:88]}")
        if attribution == "ASSUMPTION-OVER-CONSTRAINT":
            for ev in aoc_evidence[:2]:
                print(f"      [AOC] {str(ev)[:88]}")

    total_sil = len(silenced)
    print(f"\n{'='*60}")
    print(f"  ATTRIBUTION SUMMARY ({total_sil} silenced mutants total)")
    print(f"{'='*60}")
    print(f"  Knowledge-gap-attributed  : {summary['knowledge_gap']}"
          f" ({100*summary['knowledge_gap']/max(1,total_sil):.1f}%)")
    print(f"  Sacrifice-attributed      : {summary['sacrifice']}"
          f" ({100*summary['sacrifice']/max(1,total_sil):.1f}%)")
    print(f"  Assumption-over-constraint: {summary['aoc']}"
          f" ({100*summary['aoc']/max(1,total_sil):.1f}%)")
    print(f"  Unknown (structural)      : {summary['unknown']}"
          f" ({100*summary['unknown']/max(1,total_sil):.1f}%)")

    out_path = EVAL_DIR / f"attribution_{llm_dataset}.json"
    out_path.write_text(json.dumps({
        "dataset": dataset,
        "total_silenced": total_sil,
        "summary": summary,
        "per_function": func_summary,
    }, indent=2))
    print(f"\n  Saved -> {out_path}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--dataset", default="feedback_loop_A_gptoss120b",
                        help="Base dataset name (cbmc_ prefix optional)")
    args = parser.parse_args()
    analyze(args.dataset)
