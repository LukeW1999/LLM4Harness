#!/usr/bin/env python3
"""Cross-version consistency check: re-run condition A mutation oracle on CBMC 6.8
with a config matched to the paper's 5.95.1 defaults (no automatic checks, no
unwinding assertions), reusing the ALREADY-GENERATED harnesses (no LLM calls).
Compare per-mutant verdicts to the stored 5.95.1 results."""
import sys, json, subprocess, time
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed

HERE = Path(__file__).resolve().parent
EXP = HERE.parent
sys.path.insert(0, str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS  # noqa
import run_mutation_oracle_cbmc as rmo  # get_mutated_source_idx, get_final_harness, GT_PROOFS_DIR

MATCH = ["--no-standard-checks", "--no-unwinding-assertions"]  # emulate 5.95.1 defaults
RES = EXP / "results/feedback_loop_A_gptoss120b"
MUT = EXP / "mutants"
STORE = json.load(open(EXP / "evaluation/mutation_oracle_cbmc_feedback_loop_A_gptoss120b.json"))["results"]

def cbmc_verdict(func, mutant_c, harness, idx):
    cfg = FUNC_CONFIGS.get(func)
    if cfg is None:
        return "NOCFG"
    ps = [str(p) for p in cfg["project_sources"]]
    if idx is None or idx >= len(ps):
        return "NOIDX"
    ps[idx] = str(mutant_c)
    cmd = (["cbmc"] + list(COMMON_FLAGS) + list(cfg.get("defines", [])) +
           list(cfg["unwind"]) + list(cfg.get("unwindset") or []) + MATCH +
           ["--function", f"{func}_harness"] +
           [str(p) for p in cfg["proof_sources"]] + [str(harness)] + ps)
    try:
        o = subprocess.run(cmd, capture_output=True, text=True, timeout=200).stdout
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    if "VERIFICATION SUCCESSFUL" in o: return "SUCCESS"
    if "VERIFICATION FAILED" in o: return "FAIL"
    return "UNKNOWN"

def work(rec):
    func, mut = rec["func"], rec["mutant"]
    idx = rmo.get_mutated_source_idx(func)
    gt_h = rmo.GT_PROOFS_DIR / func / f"{func}_harness.c"
    llm_h = rmo.get_final_harness(RES / func)
    mutant_c = MUT / func / f"{mut}.c"
    if not (mutant_c.exists() and gt_h.exists() and llm_h):
        return (func, mut, "MISSING", "MISSING", rec)
    gt68 = cbmc_verdict(func, mutant_c, gt_h, idx)
    llm68 = cbmc_verdict(func, mutant_c, llm_h, idx)
    return (func, mut, gt68, llm68, rec)

def main():
    t0 = time.time()
    out = []
    with ProcessPoolExecutor(max_workers=16) as ex:
        futs = [ex.submit(work, r) for r in STORE]
        done = 0
        for f in as_completed(futs):
            out.append(f.result()); done += 1
            if done % 100 == 0:
                print(f"  {done}/{len(STORE)} done ({time.time()-t0:.0f}s)", flush=True)
    # compare
    gtfail_595 = sum(1 for r in STORE if r["gt"] == "FAIL")
    sil_595 = sum(1 for r in STORE if r.get("silenced"))
    gtfail_68 = sum(1 for (_,_,g,_,_) in out if g == "FAIL")
    sil_68 = sum(1 for (_,_,g,l,_) in out if g == "FAIL" and l == "SUCCESS")
    # silenced-set agreement (on the union of both silenced sets)
    def key(x): return (x[0], x[1])
    sil595_set = {(r["func"], r["mutant"]) for r in STORE if r.get("silenced")}
    sil68_set = {(f, m) for (f, m, g, l, _) in out if g == "FAIL" and l == "SUCCESS"}
    both = sil595_set & sil68_set
    only595 = sil595_set - sil68_set
    only68 = sil68_set - sil595_set
    summary = {
        "cbmc": "6.8.0 (matched config: --no-standard-checks --no-unwinding-assertions)",
        "condition": "A_gptoss120b",
        "gtfail_5951": gtfail_595, "gtfail_68": gtfail_68,
        "silenced_5951": sil_595, "silenced_68": sil_68,
        "silenced_agree_both": len(both),
        "silenced_only_5951": sorted(only595), "silenced_only_68": sorted(only68),
        "elapsed_s": round(time.time() - t0),
    }
    json.dump({"summary": summary,
               "verdicts": [{"func": f, "mutant": m, "gt68": g, "llm68": l,
                             "gt595": r["gt"], "llm595": r["llm"], "sil595": bool(r.get("silenced"))}
                            for (f, m, g, l, r) in out]},
              open(EXP / "evaluation/consistency_68_A_gptoss120b.json", "w"), indent=1)
    print("\n=== CONSISTENCY SUMMARY (condition A) ===")
    for k, v in summary.items():
        if isinstance(v, list):
            print(f"  {k}: {len(v)}  {v[:8]}")
        else:
            print(f"  {k}: {v}")

if __name__ == "__main__":
    main()
