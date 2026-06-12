#!/usr/bin/env python3
"""
build_iteration_log.py
======================
Post-hoc assertion diff logger.

Reads saved iter_N_harness.c files for each function and computes a
per-transition assertion diff, producing iteration_log.json:

    [
      {
        "from_iter": 1,
        "to_iter": 2,
        "action": "delete",          # add | delete | weaken | keep
        "assert_text": "buf->len == 0",
        "triggered_violation": true, # true if CBMC reported a FAIL on iter N
        "context": "fix_verify"      # from summary.json action field
      },
      ...
    ]

"weaken" is detected when an assertion present in iter N is absent in iter N+1
AND a substring-similar assertion (same LHS, different RHS) appears — indicating
the predicate was loosened rather than fully removed.

"triggered_violation" is set to true when:
  - summary.iterations[N-1].verify == "FAIL" (iter N had a CBMC failure)
  AND
  - the assertion was present in iter N (could have been the violated one)

NOTE: We cannot know *which* assertion CBMC violated without re-running CBMC with
--show-properties. The triggered_violation flag is therefore a conservative
estimate: true whenever an assertion was present during a failed iteration and
was subsequently removed or weakened. This is consistent with the research
design's requirement that attribution be validated by a 30-function human-annotated
subsample with inter-rater κ₂ ≥ 0.8.

Usage:
    python build_iteration_log.py --condition A_claude
    python build_iteration_log.py --all
    python build_iteration_log.py --func aws_byte_buf_init --condition A_claude
    python build_iteration_log.py --input-dir /path/to/func/dir   # for testing
"""

import re
import sys
import json
import argparse
from pathlib import Path
from dataclasses import dataclass, asdict
from typing import Optional

SCRIPT_DIR = Path(__file__).parent
EXPERIMENT_DIR = SCRIPT_DIR.parent
RESULTS_DIR = EXPERIMENT_DIR / "results"

# ── Assert extraction ─────────────────────────────────────────────────────────

# Matches: assert(expr);  — handles multi-line if needed (greedy within parens)
_ASSERT_RE = re.compile(
    r'\bassert\s*\(([^;]{1,300})\)\s*;',
    re.DOTALL
)

def extract_asserts(code: str) -> list[str]:
    """
    Extract all assert() expressions from C code.
    Returns normalised (whitespace-collapsed, lowercased) assertion texts.
    """
    raw = _ASSERT_RE.findall(code)
    return [_normalise(r) for r in raw]


def _normalise(expr: str) -> str:
    """Normalise an assertion expression for comparison."""
    s = expr.strip()
    # collapse whitespace
    s = re.sub(r'\s+', ' ', s)
    # lowercase
    s = s.lower()
    # normalise pointer dereference spacing
    s = s.replace('-> ', '->').replace(' ->', '->')
    s = s.replace('. ', '.').replace(' .', '.')
    return s


# ── Weaken detection ──────────────────────────────────────────────────────────

def _lhs_of(expr: str) -> str:
    """
    Extract the left-hand side of a simple comparison assertion.
    e.g. "buf->len == 0"  →  "buf->len"
         "buf->capacity <= 1"  →  "buf->capacity"
    Returns the full expression if no comparison found.
    """
    m = re.match(r'^(.+?)\s*(==|!=|<=|>=|<|>)\s*.+$', expr)
    return m.group(1).strip() if m else expr


def classify_transition(prev_asserts: list[str], next_asserts: list[str]) -> list[dict]:
    """
    Given the assert sets of two consecutive iterations, return a list of
    change records:
      {action: add|delete|weaken|keep, assert_text: str,
       prev_text: str|None, next_text: str|None}

    Weaken: assertion in prev with same LHS but different RHS (comparison
    operator or literal changed), absent from next as-is but a different
    version present.
    """
    prev_set = set(prev_asserts)
    next_set = set(next_asserts)

    events = []

    # 1. Unchanged assertions
    kept = prev_set & next_set

    # 2. Removed from prev
    removed = prev_set - next_set

    # 3. Added in next
    added = next_set - prev_set

    # 4. Detect weakening: removed assertion that shares LHS with an added one
    weakened_prev = set()
    weakened_next = set()
    for r in list(removed):
        r_lhs = _lhs_of(r)
        for a in list(added):
            a_lhs = _lhs_of(a)
            if r_lhs == a_lhs and r_lhs != r:  # shared LHS, different assertion
                events.append({
                    "action": "weaken",
                    "assert_text": r,        # the original (stronger) form
                    "new_text": a,           # the weakened replacement
                })
                weakened_prev.add(r)
                weakened_next.add(a)
                break

    # 5. Pure deletions (removed but not explained by weakening)
    for r in removed - weakened_prev:
        events.append({"action": "delete", "assert_text": r, "new_text": None})

    # 6. Pure additions
    for a in added - weakened_next:
        events.append({"action": "add", "assert_text": a, "new_text": None})

    return events


# ── Triggered violation heuristic ────────────────────────────────────────────

def _iter_failed(summary: dict, iter_n: int) -> bool:
    """Return True if iteration iter_n had a CBMC FAIL result."""
    for rec in summary.get("iterations", []):
        if rec["iter"] == iter_n:
            return rec.get("verify") == "FAIL"
    return False


# ── Main per-function builder ─────────────────────────────────────────────────

@dataclass
class LogEntry:
    from_iter: int
    to_iter: int
    action: str            # add | delete | weaken | keep
    assert_text: str       # the assertion text (normalised)
    new_text: Optional[str]  # for "weaken": the replacement text
    triggered_violation: bool
    context: str           # from summary.json action field of to_iter


def build_log_for_func(func_dir: Path) -> list[dict]:
    """
    Build the iteration log for a single function directory.
    Returns a list of LogEntry dicts (JSON-serialisable).
    """
    summary_path = func_dir / "summary.json"
    if not summary_path.exists():
        return []

    summary = json.loads(summary_path.read_text())
    iterations = summary.get("iterations", [])
    if not iterations:
        return []

    # Collect harness files in order
    harness_files = sorted(
        func_dir.glob("iter_*.c"),
        key=lambda p: int(re.search(r'iter_(\d+)', p.name).group(1))
    )
    if len(harness_files) < 2:
        return []  # Only one iteration — nothing to diff

    # Build iteration number → action mapping from summary
    iter_action = {rec["iter"]: rec.get("action", "unknown") for rec in iterations}

    entries = []
    for i in range(len(harness_files) - 1):
        prev_path = harness_files[i]
        next_path = harness_files[i + 1]

        prev_iter = int(re.search(r'iter_(\d+)', prev_path.name).group(1))
        next_iter = int(re.search(r'iter_(\d+)', next_path.name).group(1))

        prev_code = prev_path.read_text(encoding="utf-8", errors="replace")
        next_code = next_path.read_text(encoding="utf-8", errors="replace")

        prev_asserts = extract_asserts(prev_code)
        next_asserts = extract_asserts(next_code)

        # Was prev_iter a failed CBMC run?
        prev_failed = _iter_failed(summary, prev_iter)

        changes = classify_transition(prev_asserts, next_asserts)
        context = iter_action.get(next_iter, "unknown")

        for ch in changes:
            action = ch["action"]
            # triggered_violation: conservative — true if prev iter failed AND
            # the assertion was present (and potentially the one that failed)
            triggered = prev_failed and action in ("delete", "weaken")

            entry = LogEntry(
                from_iter=prev_iter,
                to_iter=next_iter,
                action=action,
                assert_text=ch["assert_text"],
                new_text=ch.get("new_text"),
                triggered_violation=triggered,
                context=context,
            )
            entries.append(asdict(entry))

    return entries


def build_and_save(func_dir: Path, overwrite: bool = False) -> Optional[Path]:
    """Build iteration log for func_dir and save to iteration_log.json."""
    out_path = func_dir / "iteration_log.json"
    if out_path.exists() and not overwrite:
        return out_path

    log = build_log_for_func(func_dir)
    out_path.write_text(json.dumps(log, indent=2))
    return out_path


# ── Sacrifice-then-recover detection ─────────────────────────────────────────

def find_sacrifice_then_recover(log: list[dict]) -> list[dict]:
    """
    Detect assertions that were deleted (or weakened) in transition A→B
    and then re-added in a later transition B→C.
    Returns a list of recovery events.
    """
    recoveries = []
    deleted: dict[str, int] = {}  # assert_text → to_iter when deleted

    for entry in log:
        if entry["action"] in ("delete", "weaken"):
            deleted[entry["assert_text"]] = entry["to_iter"]
        elif entry["action"] == "add":
            if entry["assert_text"] in deleted:
                sacrificed_at = deleted.pop(entry["assert_text"])
                recoveries.append({
                    "assert_text": entry["assert_text"],
                    "sacrificed_at_iter": sacrificed_at,
                    "recovered_at_iter": entry["to_iter"],
                })

    return recoveries


# ── CLI ───────────────────────────────────────────────────────────────────────

def _all_conditions() -> list[str]:
    return [
        d.name.replace("feedback_loop_", "")
        for d in RESULTS_DIR.iterdir()
        if d.is_dir() and d.name.startswith("feedback_loop_")
    ]


def main():
    parser = argparse.ArgumentParser(description="Build per-function iteration logs.")
    parser.add_argument("--condition", help="Condition name, e.g. A_claude, E")
    parser.add_argument("--func", help="Single function name")
    parser.add_argument("--all", action="store_true", help="Process all conditions")
    parser.add_argument("--overwrite", action="store_true")
    parser.add_argument("--input-dir", type=Path, help="Direct path to a func dir (for testing)")
    args = parser.parse_args()

    # Direct path mode (for testing)
    if args.input_dir:
        log = build_log_for_func(args.input_dir)
        out = args.input_dir / "iteration_log.json"
        out.write_text(json.dumps(log, indent=2))
        print(f"Written {len(log)} entries to {out}")
        return

    conditions = _all_conditions() if args.all else [args.condition]
    if not conditions or conditions == [None]:
        parser.error("Specify --condition, --all, or --input-dir")

    total_written = 0
    for cond in conditions:
        cond_dir = RESULTS_DIR / f"feedback_loop_{cond}"
        if not cond_dir.exists():
            print(f"  Skipping {cond} — directory not found")
            continue

        func_dirs = [d for d in cond_dir.iterdir() if d.is_dir()]
        if args.func:
            func_dirs = [d for d in func_dirs if d.name == args.func]

        written = 0
        for func_dir in sorted(func_dirs):
            out = build_and_save(func_dir, overwrite=args.overwrite)
            if out:
                written += 1

        print(f"  {cond}: wrote {written} iteration_log.json files")
        total_written += written

    print(f"\nTotal: {total_written} files written")


if __name__ == "__main__":
    main()
