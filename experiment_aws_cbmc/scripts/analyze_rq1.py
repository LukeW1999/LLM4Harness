#!/usr/bin/env python3
"""
analyze_rq1.py — Comprehensive RQ1 analysis: conformance pressure, assertion sacrifice,
wrong-function rates, and per-condition verdict distribution.

Usage:
    python3 scripts/analyze_rq1.py [--csv output.csv] [--full]

Conditions covered (primary LLM gptoss120b, baseline qwen2.5, replication):
  A/qwen2.5, A/claude, A/gptoss120b, G/gptoss120b, H/gptoss120b,
  B/qwen2.5, B/claude, C/qwen2.5, D/qwen2.5, E/qwen2.5, E/claude, F/claude
"""

import json
import re
import csv
import argparse
from pathlib import Path
from collections import Counter, defaultdict

SCRIPT_DIR = Path(__file__).parent
RESULTS = SCRIPT_DIR.parent / "results"


# ── helpers ────────────────────────────────────────────────────────────────

def count_asserts(code: str) -> int:
    if not code:
        return 0
    return code.count("__CPROVER_assert") + code.count("assert(")


def calls_target_function(harness_code: str, func_name: str) -> bool:
    return func_name in harness_code


def load_condition(cond_name: str) -> dict:
    d = RESULTS / f"feedback_loop_{cond_name}"
    if not d.exists():
        return {}
    data = {}
    for func_dir in d.iterdir():
        if not func_dir.is_dir():
            continue
        sfile = func_dir / "summary.json"
        if not sfile.exists():
            continue
        s = json.loads(sfile.read_text())
        il_path = func_dir / "iteration_log.json"
        il = []
        if il_path.exists():
            raw = il_path.read_text().strip()
            if raw and raw not in ("[]", "null"):
                try:
                    il = json.loads(raw)
                except json.JSONDecodeError:
                    pass
        harnesses = {}
        for f in func_dir.iterdir():
            m = re.match(r"iter_(\d+)_harness\.c", f.name)
            if m:
                harnesses[int(m.group(1))] = f.read_text()
        data[func_dir.name] = {
            "summary": s,
            "iteration_log": il,
            "harnesses": harnesses,
        }
    return data


def analyze_condition(cond_label: str, data: dict) -> dict:
    """Return a dict of metrics for one condition."""
    if not data:
        return {"label": cond_label, "n": 0}

    verdicts = Counter()
    iter_counts = []
    assert_iter1 = []
    assert_final = []
    n_zero_iter1 = 0
    n_wrong_func_iter1 = 0
    n_wrong_func_final = 0

    # Iteration log action breakdown
    actions_total = Counter()          # action → count
    actions_by_ctx = defaultdict(Counter)   # action → ctx → count
    actions_triggered = Counter()      # action → n_triggered_by_violation

    # Active sacrifice: UNKNOWN-driven deletions/weakens
    sacrifice_delete_unknown = 0
    sacrifice_weaken_unknown = 0
    # Correction: violation-driven deletions/weakens
    correct_delete_violation = 0
    correct_weaken_violation = 0

    # Per-function: did feedback change verdict?
    pass_after_sacrifice = 0   # final=SUCCESS AND had UNKNOWN-driven deletion
    still_unknown = 0          # final=UNKNOWN despite UNKNOWN-driven deletions

    for fname, d in data.items():
        s = d["summary"]
        iters = s.get("iterations", [])
        final_v = iters[-1]["verify"] if iters else "NONE"
        verdicts[final_v] += 1

        n_iters = s.get("num_iterations", 1)
        iter_counts.append(n_iters)

        h1 = d["harnesses"].get(1, "")
        hf = d["harnesses"].get(n_iters, "")
        n1 = count_asserts(h1)
        nf = count_asserts(hf)
        assert_iter1.append(n1)
        assert_final.append(nf)

        if n1 == 0:
            n_zero_iter1 += 1
        if h1 and not calls_target_function(h1, fname):
            n_wrong_func_iter1 += 1
        if hf and not calls_target_function(hf, fname):
            n_wrong_func_final += 1

        # Iteration log
        had_unknown_deletion = False
        for entry in d["iteration_log"]:
            action = entry.get("action", "?")
            ctx = entry.get("context", "?")
            triggered = entry.get("triggered_violation", False)

            actions_total[action] += 1
            actions_by_ctx[action][ctx] += 1
            if triggered:
                actions_triggered[action] += 1

            if action == "delete" and ctx == "fix_unknown":
                sacrifice_delete_unknown += 1
                had_unknown_deletion = True
            elif action == "weaken" and ctx == "fix_unknown":
                sacrifice_weaken_unknown += 1
                had_unknown_deletion = True
            elif action == "delete" and triggered:
                correct_delete_violation += 1
            elif action == "weaken" and triggered:
                correct_weaken_violation += 1

        if had_unknown_deletion:
            if final_v == "SUCCESS":
                pass_after_sacrifice += 1
            elif final_v == "UNKNOWN":
                still_unknown += 1

    n = sum(verdicts.values())
    avg1 = sum(assert_iter1) / len(assert_iter1) if assert_iter1 else 0
    avgf = sum(assert_final) / len(assert_final) if assert_final else 0
    avg_iter = sum(iter_counts) / len(iter_counts) if iter_counts else 0
    total_actions = sum(actions_total.values())

    sacrifice_total = sacrifice_delete_unknown + sacrifice_weaken_unknown
    correction_total = correct_delete_violation + correct_weaken_violation

    return {
        "label": cond_label,
        "n": n,
        "n_success": verdicts.get("SUCCESS", 0),
        "n_unknown": verdicts.get("UNKNOWN", 0),
        "n_fail": verdicts.get("FAILURE", 0),
        "n_compile_err": verdicts.get("COMPILE_ERROR", 0),
        "pass_pct": verdicts.get("SUCCESS", 0) / n * 100 if n else 0,
        "unknown_pct": verdicts.get("UNKNOWN", 0) / n * 100 if n else 0,
        "avg_iters": avg_iter,
        "avg_assert_iter1": avg1,
        "avg_assert_final": avgf,
        "assert_delta": avgf - avg1,
        "n_zero_assert_iter1": n_zero_iter1,
        "wrong_func_iter1": n_wrong_func_iter1,
        "wrong_func_iter1_pct": n_wrong_func_iter1 / n * 100 if n else 0,
        "wrong_func_final": n_wrong_func_final,
        "wrong_func_final_pct": n_wrong_func_final / n * 100 if n else 0,
        # iteration log
        "total_actions": total_actions,
        "n_delete": actions_total.get("delete", 0),
        "n_add": actions_total.get("add", 0),
        "n_weaken": actions_total.get("weaken", 0),
        "delete_pct": actions_total.get("delete", 0) / total_actions * 100 if total_actions else 0,
        # conformance pressure breakdown
        "sacrifice_delete_unknown": sacrifice_delete_unknown,
        "sacrifice_weaken_unknown": sacrifice_weaken_unknown,
        "sacrifice_total": sacrifice_total,
        "correction_delete_violation": correct_delete_violation,
        "correction_weaken_violation": correct_weaken_violation,
        "correction_total": correction_total,
        "sacrifice_ratio": sacrifice_total / (sacrifice_total + correction_total) if (sacrifice_total + correction_total) > 0 else 0,
        "pass_after_sacrifice": pass_after_sacrifice,
        "still_unknown_after_sacrifice": still_unknown,
    }


# ── main ───────────────────────────────────────────────────────────────────

CONDITIONS = [
    ("A/qwen2.5",        "A"),
    ("A/claude",         "A_claude"),
    ("A/gptoss120b",     "A_gptoss120b"),
    ("G/gptoss120b",     "G_gptoss120b"),
    ("H/gptoss120b",     "H_gptoss120b"),
    ("B/qwen2.5",        "B"),
    ("B/claude",         "B_claude"),
    ("C/qwen2.5",        "C"),
    ("D/qwen2.5",        "D"),
    ("E/qwen2.5",        "E"),
    ("E/claude",         "E_claude"),
    ("F/claude",         "F_claude"),
    # Replication (broken — for documentation)
    ("A/deepseekv4flash", "A_deepseekv4flash"),
    ("G/deepseekv4flash", "G_deepseekv4flash"),
    ("H/deepseekv4flash", "H_deepseekv4flash"),
    # Replication (Llama 3.3 70B)
    ("A/llama3370b",      "A_llama3370binstruct"),
    ("G/llama3370b",      "G_llama3370binstruct"),
    ("H/llama3370b",      "H_llama3370binstruct"),
]


def main():
    parser = argparse.ArgumentParser(description="RQ1 analysis")
    parser.add_argument("--csv", help="Write CSV to this path")
    parser.add_argument("--full", action="store_true", help="Print full detail table")
    args = parser.parse_args()

    results = []
    for label, cname in CONDITIONS:
        data = load_condition(cname)
        r = analyze_condition(label, data)
        results.append(r)

    # ── Table 1: Verdict + assertion distribution ──────────────────────────
    print("\n" + "=" * 110)
    print("TABLE 1: Verdict Distribution + Assertion Change")
    print("=" * 110)
    hdr = f"{'Condition':<22} {'N':>4} {'PASS%':>6} {'UNK%':>6} {'FAIL%':>5} {'AvgItr':>7} {'As_1':>6} {'As_F':>6} {'ΔAs':>6} {'0-ass':>6} {'WrongFn%':>9}"
    print(hdr)
    print("-" * 110)
    for r in results:
        if r["n"] == 0:
            continue
        print(f"{r['label']:<22} {r['n']:>4} {r['pass_pct']:>6.1f} {r['unknown_pct']:>6.1f} "
              f"{r['n_fail']/r['n']*100:>5.1f} {r['avg_iters']:>7.2f} "
              f"{r['avg_assert_iter1']:>6.1f} {r['avg_assert_final']:>6.1f} "
              f"{r['assert_delta']:>+6.1f} {r['n_zero_assert_iter1']:>6} "
              f"{r['wrong_func_iter1_pct']:>9.1f}")

    # ── Table 2: Conformance pressure ──────────────────────────────────────
    print("\n" + "=" * 110)
    print("TABLE 2: Conformance Pressure (Iteration Log Actions)")
    print("=" * 110)
    hdr2 = f"{'Condition':<22} {'TotalActs':>10} {'Del%':>6} {'Add%':>6} {'Wkn%':>6} {'SacDel':>7} {'SacWkn':>7} {'CorrDel':>8} {'CorrWkn':>8} {'SacRatio%':>10} {'PassAfterSac':>13}"
    print(hdr2)
    print("-" * 110)
    for r in results:
        if r["n"] == 0 or r["total_actions"] == 0:
            print(f"{r['label']:<22} {'(no iterations)':>20}")
            continue
        ta = r["total_actions"]
        add_pct = r["n_add"] / ta * 100 if ta else 0
        wkn_pct = r["n_weaken"] / ta * 100 if ta else 0
        print(f"{r['label']:<22} {ta:>10} {r['delete_pct']:>6.1f} {add_pct:>6.1f} {wkn_pct:>6.1f} "
              f"{r['sacrifice_delete_unknown']:>7} {r['sacrifice_weaken_unknown']:>7} "
              f"{r['correction_delete_violation']:>8} {r['correction_weaken_violation']:>8} "
              f"{r['sacrifice_ratio']*100:>10.1f} {r['pass_after_sacrifice']:>13}")

    # ── Table 3: Wrong-function rate ───────────────────────────────────────
    print("\n" + "=" * 80)
    print("TABLE 3: Wrong-Function Rate (first harness vs final harness)")
    print("=" * 80)
    hdr3 = f"{'Condition':<22} {'N':>4} {'WrongFn_Iter1':>14} {'WrongFn_Final':>14} {'FixedByFeedback':>16}"
    print(hdr3)
    print("-" * 80)
    for r in results:
        if r["n"] == 0:
            continue
        fixed = r["wrong_func_iter1"] - r["wrong_func_final"]
        print(f"{r['label']:<22} {r['n']:>4} {r['wrong_func_iter1']:>8} ({r['wrong_func_iter1_pct']:>4.1f}%) "
              f"{r['wrong_func_final']:>8} ({r['wrong_func_final_pct']:>4.1f}%) "
              f"{fixed:>16}")

    # ── Key summary numbers ────────────────────────────────────────────────
    print("\n" + "=" * 80)
    print("KEY FINDINGS SUMMARY")
    print("=" * 80)

    # G vs A for gptoss120b
    r_A = next((r for r in results if r["label"] == "A/gptoss120b"), None)
    r_G = next((r for r in results if r["label"] == "G/gptoss120b"), None)
    r_H = next((r for r in results if r["label"] == "H/gptoss120b"), None)
    r_A_claude = next((r for r in results if r["label"] == "A/claude"), None)

    if r_G and r_A:
        pass_gain = r_A["pass_pct"] - r_G["pass_pct"]
        assert_cost = r_A["assert_delta"]
        print(f"\n[G vs A — gptoss120b]")
        print(f"  PASS rate: G={r_G['pass_pct']:.1f}% → A={r_A['pass_pct']:.1f}% (+{pass_gain:.1f}pp)")
        print(f"  Assertion change: G={r_G['assert_delta']:+.1f} → A={r_A['assert_delta']:+.1f}")
        print(f"  Trade-off: +{pass_gain:.1f}pp PASS at cost of {abs(assert_cost):.1f} assertions/function avg")

    if r_H and r_A:
        print(f"\n[H vs A — strategy-neutral vs strategy-guided repair]")
        print(f"  PASS rate: A={r_A['pass_pct']:.1f}% vs H={r_H['pass_pct']:.1f}% (diff={r_H['pass_pct']-r_A['pass_pct']:.1f}pp)")
        print(f"  Weaken actions: A={r_A['n_weaken']} vs H={r_H['n_weaken']}")
        print(f"  Delete actions: A={r_A['n_delete']} vs H={r_H['n_delete']}")
        print(f"  → Strategy-neutral H leads to MORE weakening ({r_H['n_weaken']} vs {r_A['n_weaken']}) but similar PASS rate")

    if r_A_claude:
        print(f"\n[Claude A — highest PASS rate, highest assertion sacrifice]")
        print(f"  PASS rate: {r_A_claude['pass_pct']:.1f}%, assertion delta={r_A_claude['assert_delta']:+.1f}")
        print(f"  Sacrifice ratio: {r_A_claude['sacrifice_ratio']*100:.1f}% of deletions/weakens are UNKNOWN-driven")

    # Overall sacrifice ratio across all conditions
    all_sac = sum(r["sacrifice_total"] for r in results if r["n"] > 0)
    all_corr = sum(r["correction_total"] for r in results if r["n"] > 0)
    all_del = sum(r["n_delete"] for r in results if r["n"] > 0)
    all_wkn = sum(r["n_weaken"] for r in results if r["n"] > 0)
    all_acts = sum(r["total_actions"] for r in results if r["n"] > 0)
    print(f"\n[Overall (all conditions):]")
    print(f"  Total iteration actions: {all_acts}")
    print(f"  Deletions: {all_del} ({all_del/all_acts*100:.1f}%)")
    print(f"  Additions: {sum(r['n_add'] for r in results if r['n']>0)} ({sum(r['n_add'] for r in results if r['n']>0)/all_acts*100:.1f}%)")
    print(f"  Weakens: {all_wkn} ({all_wkn/all_acts*100:.1f}%)")
    print(f"  UNKNOWN-driven (sacrifice) del+wkn: {all_sac} ({all_sac/(all_sac+all_corr)*100:.1f}% of all del+wkn)")
    print(f"  Violation-driven (correction) del+wkn: {all_corr} ({all_corr/(all_sac+all_corr)*100:.1f}% of all del+wkn)")

    # ── CSV output ─────────────────────────────────────────────────────────
    if args.csv:
        keys = [k for k in results[0].keys() if k != "label"]
        with open(args.csv, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=["label"] + keys)
            w.writeheader()
            for r in results:
                if r["n"] > 0:
                    w.writerow(r)
        print(f"\nCSV written to: {args.csv}")


if __name__ == "__main__":
    main()
