#!/usr/bin/env python3
"""Renamed-function cloze CONTROL (memorisation test, Lucas's note, paper l.482).

Same cloze as run_cloze_test.py, but the function name and the harness's local
variable names are renamed to neutral tokens IN THE PROMPT only, so the model
cannot recognise `aws_byte_buf_cat` and retrieve a memorised assertion. The
model's fill (in renamed terms) is mapped back to the original identifiers and
scored by CBMC on the ORIGINAL source + mutants (no renamed source needed).

First-order control: the `aws_byte_buf` struct type, its fields, and the
proof-helper calls are kept (renaming them cascades into the helper library),
so the model still sees the domain. It defeats name-based retrieval, not
domain-pattern association. POC map is byte_buf_cat-specific.
"""
import sys, os, re, json, argparse
sys.path.insert(0, "/root/experiment_aws_cbmc/scripts")
import importlib.util as u
_s = u.spec_from_file_location("clz", "/root/experiment_aws_cbmc/scripts/run_cloze_test.py")
C = u.module_from_spec(_s); _s.loader.exec_module(C)   # reuse blank_harness/cbmc_check/func_source/get_call

# byte_buf_cat rename map (longest identifiers first so \b rules don't overlap)
FWD = [
    ("aws_byte_buf_cat", "fn_under_test"),
    ("number_of_args", "n_args"),
    ("old_buffer1", "old_p1"), ("old_buffer2", "old_p2"), ("old_buffer3", "old_p3"),
    ("old_dest", "old_out"),
    ("buffer1", "p1"), ("buffer2", "p2"), ("buffer3", "p3"),
    ("dest", "out"),
]

def apply_map(text, pairs):
    for a, b in pairs:
        text = re.sub(rf"\b{re.escape(a)}\b", b, text)
    return text

def rename_fwd(text):  # original identifiers -> neutral
    return apply_map(text, FWD)

def rename_rev(text):  # neutral -> original (for the model's fill)
    return apply_map(text, [(b, a) for a, b in reversed(FWD)])


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cond", required=True)
    ap.add_argument("--model", default="openrouter")
    ap.add_argument("--func", default="aws_byte_buf_cat")
    ap.add_argument("--timeout", type=int, default=90)
    a = ap.parse_args()
    CALL = C.get_call(a.model)

    op = C.EVAL / f"mutation_oracle_cbmc_feedback_loop_{a.cond}.json"
    sil = [r for r in json.load(open(op))["results"] if r.get("silenced") and r["func"] == a.func]
    muts = [r["mutant"] for r in sil]

    gts = C.A.get_gt_asserts(a.func)
    its = C.A.get_llm_iter_asserts(f"feedback_loop_{a.cond}", a.func)
    allllm = set(C.norm(x) for asl in its.values() for x in asl) if its else set()
    missed = [g for g in gts if C.norm(g) not in allllm]

    gt_path = __import__("pathlib").Path(str(C.A.GT_PROOFS_DIR)) / a.func / f"{a.func}_harness.c"
    gt_text = gt_path.read_text(errors="replace")
    gt_renamed = rename_fwd(gt_text)
    src_excerpt_renamed = rename_fwd(C.func_source(a.func))

    results = []
    for ga in missed:
        blanked_orig, ok0 = C.blank_harness(gt_text, ga)                    # for scoring (original ids)
        blanked_ren, ok1 = C.blank_harness(gt_renamed, rename_fwd(ga))      # for the prompt (neutral ids)
        if not (ok0 and ok1):
            results.append({"assert": ga.strip(), "status": "BLANK_FAIL"}); continue
        prompt = f"""Below is an expert CBMC proof harness for `fn_under_test` with EXACTLY ONE assert() statement removed (marked /* CLOZE */). Based on the function's semantics, write the single missing assertion.

Function source (excerpt):
```c
{src_excerpt_renamed}
```

Harness with one assertion blanked:
```c
{blanked_ren}
```
Reply with ONLY the missing assert statement."""
        try:
            resp = CALL(C.SYSP, prompt, temperature=0.0)
        except Exception as e:
            results.append({"assert": ga.strip(), "status": f"API_ERR:{e}"}); continue
        m = re.search(r"assert\s*\(.*?\)\s*;", resp, re.S)
        filled_ren = m.group(0) if m else resp.strip()[:200]
        filled_orig = rename_rev(filled_ren)                                # map back to real ids
        exact = C.norm(filled_orig) == C.norm(ga)
        filled_text = blanked_orig.replace(
            "/* CLOZE: exactly one assertion is missing here -- fill it in */", filled_orig)
        cb = C.cbmc_check(a.func, filled_text, muts, a.timeout)
        results.append({"assert": ga.strip(), "filled_renamed": filled_ren.strip(),
                        "filled_mapped": filled_orig.strip(), "exact": exact, "cbmc": cb, "status": "OK"})
        cstr = f"orig={cb['orig']} catches={cb['catches']}/{cb['n_mut']}" if cb else "cbmc=NA"
        print(f"[{a.func}|RENAMED] exact={exact} {cstr}\n   model saw vars: p1/p2/p3/out\n"
              f"   filled(neutral): {filled_ren.strip()}\n   mapped->orig:    {filled_orig.strip()}", flush=True)

    n = [r for r in results if r["status"] == "OK"]
    rec = sum(1 for r in n if r.get("cbmc") and r["cbmc"]["catches"] > 0 and r["cbmc"]["orig"] in ("SUCCESS", "UNKNOWN"))
    ex = sum(1 for r in n if r["exact"])
    print(f"\n===== RENAMED CLOZE {a.cond}/{a.func} model={a.model}: "
          f"n={len(n)} exact={ex} recovered={rec} =====")
    json.dump(results, open(C.EVAL / f"cloze_rename_{a.cond}_{a.func}_{a.model}.json", "w"), indent=1)


if __name__ == "__main__":
    main()
