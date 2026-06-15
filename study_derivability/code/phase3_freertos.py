#!/usr/bin/env python3
"""
Phase 3 — Generate LLM harnesses for FreeRTOS-Plus-TCP.

Conditions: A (code only) and E (code + same-module example).
Models: Claude Sonnet, Qwen2.5-Coder-32B.

Reads API keys from .env (Claude_API / Qwen_API),
falling back to ANTHROPIC_API_KEY / DASHSCOPE_API_KEY.

Output per function per condition per model:
  data/freertos_results/{cond}_{model}/{func}/iter_1_harness.c
  data/freertos_results/{cond}_{model}/{func}/summary.json

After generation: computes recall vs GT using the same norm() as Phase 1.
"""

import csv
import json
import os
import re
import sys
import time
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
from config import (
    FREERTOS_CORPUS_DIR, FREERTOS_SHORTLIST, DATA_DIR, REPORT_DIR,
)

# ── Load .env ─────────────────────────────────────────────────────────────────

def load_env() -> dict[str, str]:
    env: dict[str, str] = {}
    # Load from all candidate .env files (later files can override)
    candidates = [
        Path(__file__).parent.parent.parent / "experiment_aws_cbmc" / ".env",  # original experiments
        Path(__file__).parent.parent.parent / ".env",                           # repo root
        Path(__file__).parent.parent / ".env",                                  # study_derivability/
    ]
    for dotenv in candidates:
        if dotenv.exists():
            for line in dotenv.read_text().splitlines():
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    k, _, v = line.partition('=')
                    env[k.strip()] = v.strip()
    return env


ENV = load_env()

CLAUDE_KEY = ENV.get("ANTHROPIC_API_KEY") or ENV.get("Claude_API") or os.environ.get("ANTHROPIC_API_KEY", "")
QWEN_KEY   = ENV.get("Qwen_API")   or ENV.get("DASHSCOPE_API_KEY")  or os.environ.get("DASHSCOPE_API_KEY", "")

# ── FreeRTOS harness source index ─────────────────────────────────────────────

FRT_SRC = FREERTOS_CORPUS_DIR.parent / "source"

def _load_freertos_harnesses() -> dict[str, dict]:
    """Return {func: {path, gt_code, module}} for all FreeRTOS harnesses with ≥1 assertion."""
    gt_rows = list(csv.DictReader((DATA_DIR / "gt_assertions.csv").open()))
    funcs_with_asserts = {r["function"] for r in gt_rows
                          if r["corpus"] == "freertos" and r["kind"] == "ASSERT"}

    seen: set[str] = set()
    out: dict[str, dict] = {}
    for h in sorted(FREERTOS_CORPUS_DIR.rglob("*_harness.c")):
        func = h.stem.removesuffix("_harness")
        if func in seen or func not in funcs_with_asserts:
            continue
        seen.add(func)
        module = h.parts[list(h.parts).index("proofs") + 1]
        out[func] = {
            "path": h,
            "gt_code": h.read_text(errors="replace"),
            "module": module,
        }
    return out


def find_same_module_example(func: str, module: str,
                              all_harnesses: dict[str, dict]) -> str | None:
    """Return gt_code of a different function in the same module."""
    for other_func, info in all_harnesses.items():
        if other_func != func and info["module"] == module:
            return info["gt_code"]
    return None


# ── Prompt builders ───────────────────────────────────────────────────────────

def _harness_header_snippet(gt_code: str, max_lines: int = 15) -> str:
    """Extract the first max_lines of non-blank harness code for context."""
    lines = [l for l in gt_code.splitlines() if l.strip()]
    return "\n".join(lines[:max_lines])


def build_prompt_A(func: str, gt_code: str) -> str:
    """Condition A: harness code only, ask LLM to write a new one."""
    # Extract includes and key setup from GT (without assertions = spoiler-free)
    return f"""You are a formal verification expert. Write a CBMC proof harness for the FreeRTOS-Plus-TCP function `{func}`.

A CBMC harness must:
1. Allocate nondeterministic inputs using `nondet_*()` helpers
2. Constrain inputs with `__CPROVER_assume()`
3. Call the function under test
4. Assert postconditions with `assert()`

Here is the ground-truth harness structure (first lines only, for context):
```c
{_harness_header_snippet(gt_code)}
...
```

Write a complete harness function `{func}_harness()` that covers the function's postconditions.
Output only the C code, no explanation."""


def build_prompt_E(func: str, gt_code: str, example_code: str) -> str:
    """Condition E: same-module ground-truth example included."""
    return f"""You are a formal verification expert. Write a CBMC proof harness for the FreeRTOS-Plus-TCP function `{func}`.

Here is a complete example harness from the same module to show the style:
```c
{example_code}
```

Now write a complete harness function `{func}_harness()` following the same pattern.
Output only the C code, no explanation."""


# ── LLM callers ──────────────────────────────────────────────────────────────

def call_claude(prompt: str, model: str = "claude-sonnet-4-6") -> str:
    import anthropic
    client = anthropic.Anthropic(api_key=CLAUDE_KEY)
    msg = client.messages.create(
        model=model,
        max_tokens=2048,
        messages=[{"role": "user", "content": prompt}],
    )
    return msg.content[0].text


def call_qwen(prompt: str,
              model: str = "qwen2.5-coder-32b-instruct") -> str:
    import dashscope
    from dashscope import Generation
    dashscope.api_key = QWEN_KEY
    resp = Generation.call(
        model=model,
        messages=[{"role": "user", "content": prompt}],
        max_tokens=2048,
        result_format="message",
    )
    return resp.output.choices[0].message.content


def extract_c_code(llm_output: str) -> str:
    """Strip markdown fences from LLM output."""
    m = re.search(r'```(?:c|cpp)?\n(.*?)```', llm_output, re.DOTALL)
    if m:
        return m.group(1).strip()
    return llm_output.strip()


# ── Recall computation ────────────────────────────────────────────────────────

def norm(s: str) -> str:
    s = s.lower().strip().replace('->', '.')
    s = re.sub(r'\s+', ' ', s)
    s = re.sub(r'^assertion\s*', '', s)
    s = re.sub(r'\b([a-z][a-z0-9]*)_old\b', r'old_\1', s)
    s = re.sub(r'(?<=_is_valid\()&[a-z]\w*', '&_arg_', s)
    s = re.sub(r'\b(?!old_)[a-z][a-z0-9_]*\.', '_v_.', s)
    eq = re.match(r'^(.+?)\s*==\s*(.+)$', s)
    if eq:
        l, r = eq.group(1).strip(), eq.group(2).strip()
        if l > r:
            s = f'{r} == {l}'
    return s.strip()


_ASSERT_RE = re.compile(r'\b(?:assert|__CPROVER_assert)\s*\(', re.DOTALL)

def _first_arg(args_str: str) -> str:
    """Extract the first argument from a possibly multi-arg string like 'expr, "msg"'."""
    depth = 0
    in_str = False
    for i, ch in enumerate(args_str):
        if ch == '"' and (i == 0 or args_str[i-1] != '\\'):
            in_str = not in_str
        if in_str:
            continue
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        elif ch == ',' and depth == 0:
            return args_str[:i].strip()
    return args_str.strip()


def extract_asserts(code: str) -> set[str]:
    clean = re.sub(r'//[^\n]*', '', code)
    clean = re.sub(r'/\*.*?\*/', '', clean, flags=re.DOTALL)
    results: set[str] = set()
    for m in _ASSERT_RE.finditer(clean):
        start = m.end()
        depth, i = 1, start
        while i < len(clean) and depth > 0:
            if clean[i] == '(':   depth += 1
            elif clean[i] == ')': depth -= 1
            i += 1
        full_arg = clean[start:i-1]
        expr = _first_arg(full_arg)
        results.add(norm(expr))
    return results


def compute_recall(gt_code: str, llm_code: str) -> dict:
    gt_norms  = extract_asserts(gt_code)
    llm_norms = extract_asserts(llm_code)
    if not gt_norms:
        return {"gt_count": 0, "matched": 0, "recall": None}
    matched = gt_norms & llm_norms
    return {
        "gt_count": len(gt_norms),
        "matched":  len(matched),
        "recall":   round(len(matched) / len(gt_norms), 3),
        "unmatched_gt": sorted(gt_norms - llm_norms),
    }


# ── Generation loop ───────────────────────────────────────────────────────────

RESULT_ROOT = DATA_DIR / "freertos_results"

def run_generation(func: str, cond: str, model_name: str,
                   prompt: str, gt_code: str,
                   dry_run: bool = False) -> dict:
    out_dir = RESULT_ROOT / f"{cond}_{model_name}" / func
    out_dir.mkdir(parents=True, exist_ok=True)
    harness_path = out_dir / "iter_1_harness.c"
    summary_path = out_dir / "summary.json"

    if summary_path.exists():
        return json.loads(summary_path.read_text())

    if dry_run:
        print(f"  [dry_run] would call {model_name} for {func} cond={cond}")
        return {}

    try:
        if model_name == "claude":
            raw = call_claude(prompt)
        else:
            raw = call_qwen(prompt)
        llm_code = extract_c_code(raw)
    except Exception as e:
        llm_code = f"// ERROR: {e}"
        print(f"  ERROR calling {model_name} for {func}: {e}")

    harness_path.write_text(llm_code)
    recall = compute_recall(gt_code, llm_code)
    summary = {
        "func": func, "cond": cond, "model": model_name,
        **recall, "harness_path": str(harness_path),
    }
    summary_path.write_text(json.dumps(summary, indent=2))
    return summary


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--dry-run",  action="store_true", help="print prompts, don't call APIs")
    parser.add_argument("--models",   default="claude,qwen", help="comma-separated: claude,qwen")
    parser.add_argument("--conds",    default="A,E",          help="comma-separated: A,E")
    parser.add_argument("--func",     default=None,           help="single function to test")
    parser.add_argument("--limit",    type=int, default=None, help="max functions to run")
    args = parser.parse_args()

    models = [m.strip() for m in args.models.split(",")]
    conds  = [c.strip() for c in args.conds.split(",")]

    # Verify API keys
    if "claude" in models and not CLAUDE_KEY:
        print("ERROR: Claude_API not set in .env"); sys.exit(1)
    if "qwen" in models and not QWEN_KEY:
        print("ERROR: Qwen_API not set in .env"); sys.exit(1)

    print("Loading FreeRTOS harnesses …")
    all_harnesses = _load_freertos_harnesses()
    print(f"  {len(all_harnesses)} functions with ≥1 assertion")

    funcs = [args.func] if args.func else sorted(all_harnesses)
    if args.limit:
        funcs = funcs[:args.limit]

    results: list[dict] = []
    total = len(funcs) * len(conds) * len(models)
    done  = 0

    for func in funcs:
        info = all_harnesses[func]
        gt_code = info["gt_code"]
        module  = info["module"]
        example = find_same_module_example(func, module, all_harnesses)

        for cond in conds:
            for model in models:
                done += 1
                print(f"  [{done}/{total}] {func} cond={cond} model={model}")

                if cond == "A":
                    prompt = build_prompt_A(func, gt_code)
                elif cond == "E":
                    if example is None:
                        print(f"    no same-module example, skipping E")
                        continue
                    prompt = build_prompt_E(func, gt_code, example)
                else:
                    continue

                summary = run_generation(func, cond, model, prompt,
                                         gt_code, dry_run=args.dry_run)
                if summary:
                    results.append(summary)

                if not args.dry_run:
                    time.sleep(0.5)   # rate-limit courtesy

    # Save aggregate results
    if results:
        out_path = DATA_DIR / "freertos_generation_results.json"
        out_path.write_text(json.dumps(results, indent=2))
        print(f"\nSaved {len(results)} results → {out_path}")

        # Print recall table
        print("\n── Recall summary ──")
        for cond in conds:
            for model in models:
                sub = [r for r in results
                       if r.get("cond") == cond and r.get("model") == model
                       and r.get("recall") is not None]
                if sub:
                    mean = sum(r["recall"] for r in sub) / len(sub)
                    print(f"  {cond}_{model:6s}  n={len(sub):2}  mean_recall={mean:.3f}")


if __name__ == "__main__":
    main()
