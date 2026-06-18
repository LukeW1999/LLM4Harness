#!/usr/bin/env python3
"""esbmc_oracle.py - ESBMC differential mutation oracle (second verifier).

Mirror of run_mutation_oracle_cbmc.py but using ESBMC in strict-contract mode
(ESBMC_STRICT=1 by default), so silencing can be compared CBMC vs ESBMC for a
verifier-independence claim.

For each mutant m of function f, substitute the mutated source and run:
  GT : esbmc(H_GT,  m)  -> FAIL means the expert catches the bug
  LLM: esbmc(H_LLM, m)  -> SUCCESS means the LLM harness lets it pass
  silenced = GT==FAIL and LLM==SUCCESS

Usage:
  python3 esbmc_oracle.py --funcs aws_byte_buf_cat aws_byte_buf_eq --cond feedback_loop_A_claude
"""
import json, subprocess, argparse, os, sys
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
from collections import defaultdict

SCRIPT_DIR = Path(__file__).parent
sys.path.insert(0, str(SCRIPT_DIR))
os.environ.setdefault("ESBMC_STRICT", "1")   # strict contracts by default
import esbmc_runner as E

REPO        = SCRIPT_DIR.parent
MUTANTS_DIR = REPO / "mutants"
RESULTS_DIR = REPO / "results"
EVAL_DIR    = REPO / "evaluation"
GT_PROOFS   = E.PROOFDIR / "proofs"
TIMEOUT     = int(os.environ.get("ESBMC_TIMEOUT", "120"))


def get_mutated_source_idx(func):
    meta = MUTANTS_DIR / func / "metadata.json"
    if not meta.exists():
        return None
    base = Path(json.loads(meta.read_text()).get("source_file", "")).name
    cfg = E.FUNC_CONFIGS.get(func, {})
    for i, s in enumerate(cfg.get("project_sources", [])):
        if Path(s).name == base:
            return i
    return None


def get_final_harness(d):
    hs = sorted(d.glob("iter_*_harness.c"))
    return hs[-1] if hs else None


def _run(func, mutant_c, harness_c, timeout, idx):
    cfg = E.FUNC_CONFIGS.get(func)
    if cfg is None:
        return "COMPILE_ERROR"
    proof = E._filter_sources(cfg["proof_sources"])
    proj  = list(cfg["project_sources"]); proj[idx] = Path(mutant_c)
    proj  = E._filter_sources(proj)
    all_src = proof + [Path(harness_c)] + proj
    cmd = ([str(E.ESBMC_BIN)] + E.ESBMC_BASE_FLAGS + cfg["defines"] + cfg["unwind"]
           + ["--function", f"{func}_harness"] + [str(s) for s in all_src])
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    c = p.stderr + p.stdout
    if any(b in c for b in ("PARSING ERROR", "CONVERSION ERROR", "Segmentation fault")):
        return "COMPILE_ERROR"
    if "VERIFICATION SUCCESSFUL" in c:
        return "SUCCESS"
    if "VERIFICATION FAILED" in c:
        return "FAIL"
    return "UNKNOWN"


def process_mutant(task):
    func, mname, mc, gt, llm, to, idx = task
    g = _run(func, mc, gt,  to, idx)
    l = _run(func, mc, llm, to, idx)
    return {"func": func, "mutant": mname, "gt": g, "llm": l,
            "silenced": g == "FAIL" and l == "SUCCESS"}


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cond", default="feedback_loop_A_claude")
    ap.add_argument("--funcs", nargs="+", required=True)
    ap.add_argument("--workers", type=int, default=int(os.environ.get("ESBMC_WORKERS", "4")))
    ap.add_argument("--out", default="esbmc_oracle_smoke.json")
    a = ap.parse_args()

    tasks = []
    for func in a.funcs:
        idx = get_mutated_source_idx(func)
        if idx is None:
            print("skip (no mutated idx):", func); continue
        gt = GT_PROOFS / func / f"{func}_harness.c"
        if not gt.exists():
            print("skip (no GT harness):", func); continue
        ldir = RESULTS_DIR / a.cond / func
        llm = get_final_harness(ldir) if ldir.exists() else None
        if not llm:
            print("skip (no LLM harness):", func); continue
        for mc in sorted((MUTANTS_DIR / func).glob("mutant_*.c")):
            tasks.append((func, mc.stem, mc, gt, llm, TIMEOUT, idx))

    print(f"{len(tasks)} mutant-tasks over {len(a.funcs)} funcs | workers={a.workers} "
          f"strict={os.environ.get('ESBMC_STRICT')} memlimit={os.environ.get('ESBMC_MEMLIMIT')}")
    results = []
    with ProcessPoolExecutor(max_workers=a.workers) as pool:
        futs = {pool.submit(process_mutant, t): t for t in tasks}
        done = 0
        for f in as_completed(futs):
            results.append(f.result()); done += 1
            if done % 50 == 0:
                print(f"  {done}/{len(tasks)}"); sys.stdout.flush()

    by = defaultdict(lambda: {"gtfail": 0, "silenced": 0, "n": 0})
    for r in results:
        b = by[r["func"]]; b["n"] += 1
        if r["gt"] == "FAIL":   b["gtfail"] += 1
        if r["silenced"]:       b["silenced"] += 1
    print(f"\n{'func':<40}{'n':>4}{'GT-FAIL':>9}{'silenced':>10}")
    tgf = tsil = 0
    for fn in sorted(by):
        b = by[fn]; tgf += b["gtfail"]; tsil += b["silenced"]
        print(f"  {fn:<38}{b['n']:>4}{b['gtfail']:>9}{b['silenced']:>10}")
    print(f"  TOTAL GT-FAIL={tgf} silenced={tsil} "
          f"Sil/GT={100*tsil/tgf if tgf else 0:.1f}%")
    out = EVAL_DIR / a.out
    json.dump({"cond": a.cond, "strict": os.environ.get("ESBMC_STRICT"), "results": results},
              open(out, "w"), indent=2)
    print("saved", out)


if __name__ == "__main__":
    main()
