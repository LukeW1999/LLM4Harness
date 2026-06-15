#!/usr/bin/env python3
"""C1 (ARS review): behavioural, rename-immune KG check.

For each silenced mutant (GT=FAIL & final-LLM=SUCCESS), run CBMC on EVERY LLM
iteration harness. If ANY iteration catches the mutant (FAIL), a catching
assertion WAS written at some point (behavioural sacrifice candidate). If NO
iteration ever catches it, KG is confirmed behaviourally -- the catching
assertion was never written in ANY form (rename-immune), not merely "not written
exactly". This is symmetric to the AOC assumption-relaxation re-run.

Usage: python3 behavioural_kg.py feedback_loop_A_gptoss120b feedback_loop_A_claude ...
"""
import sys, json, importlib.util as iu
from pathlib import Path
from concurrent.futures import ThreadPoolExecutor

ROOT = Path("/root/experiment_aws_cbmc")
spec = iu.spec_from_file_location("orc", str(ROOT / "scripts/run_mutation_oracle_cbmc.py"))
O = iu.module_from_spec(spec); spec.loader.exec_module(O)

TIMEOUT = getattr(O, "TIMEOUT", 180)
MUT = O.MUTANTS_DIR
EVAL = O.EVAL_DIR
RESULTS = ROOT / "results"

def iter_harnesses(cond, func):
    d = RESULTS / cond / func
    return sorted(d.glob("iter_*_harness.c")) if d.exists() else []

def check_one(cond, func, mutant):
    """A LEGITIMATE catch by an iteration = the iteration harness PASSES on the
    original source (valid, non-degenerate harness) AND FAILS on the mutant
    (genuinely discriminates the bug). An early harness that fails on the
    original too is just over-strong/broken, not a real catch -> not counted.
    Return (func, mutant, ever_caught:bool, n_iters, verdicts)."""
    try:
        idx = O.get_mutated_source_idx(func)
        orig_src = Path(O.FUNC_CONFIGS[func]["project_sources"][idx])
    except Exception:
        return (func, mutant, None, 0, "no_src_idx")
    mutant_c = MUT / func / f"{mutant}.c"
    if not mutant_c.exists():
        return (func, mutant, None, 0, "no_mutant_file")
    hs = iter_harnesses(cond, func)
    if not hs:
        return (func, mutant, None, 0, "no_harness")
    verdicts = []
    ever = False
    for h in hs:
        v_mut = O.run_cbmc_on_mutant(func, mutant_c, h, TIMEOUT, idx)
        if v_mut in ("FAIL", "SAT"):
            v_orig = O.run_cbmc_on_mutant(func, orig_src, h, TIMEOUT, idx)
            legit = v_orig in ("SUCCESS", "UNSAT")
            verdicts.append(f"{v_mut}/orig:{v_orig}")
            if legit:
                ever = True
        else:
            verdicts.append(v_mut)
    return (func, mutant, ever, len(hs), ",".join(verdicts))

def run_condition(cond):
    op = EVAL / f"mutation_oracle_cbmc_{cond}.json"
    data = json.loads(op.read_text())["results"]
    sil = [(r["func"], r["mutant"]) for r in data if r.get("silenced")]
    tasks = []
    with ThreadPoolExecutor(max_workers=8) as ex:
        futs = [ex.submit(check_one, cond, f, m) for f, m in sil]
        rows = [fu.result() for fu in futs]
    decided = [r for r in rows if r[2] is not None]
    ever = [r for r in decided if r[2]]
    bkg = [r for r in decided if not r[2]]
    undecided = [r for r in rows if r[2] is None]
    out = {
        "condition": cond,
        "silenced_total": len(sil),
        "decided": len(decided),
        "behavioural_KG": len(bkg),
        "ever_caught_some_iter": len(ever),
        "undecided": len(undecided),
        "behavioural_KG_pct_of_decided": round(100*len(bkg)/len(decided), 1) if decided else None,
        "ever_caught_cases": [{"func": r[0], "mutant": r[1], "verdicts": r[4]} for r in ever],
        "undecided_reasons": [{"func": r[0], "mutant": r[1], "why": r[4]} for r in undecided],
    }
    outp = EVAL / f"behavioural_kg_{cond}.json"
    outp.write_text(json.dumps(out, indent=2))
    print(f"{cond}: silenced={len(sil)} decided={len(decided)} "
          f"behavioural_KG={len(bkg)} ({out['behavioural_KG_pct_of_decided']}%) "
          f"ever_caught={len(ever)} undecided={len(undecided)} -> {outp.name}")
    return out

if __name__ == "__main__":
    conds = sys.argv[1:] or ["feedback_loop_A_gptoss120b"]
    print("BEHAVIOURAL_KG_START")
    for c in conds:
        try:
            run_condition(c)
        except Exception as e:
            print(f"{c}: ERROR {e}")
    print("BEHAVIOURAL_KG_DONE")
