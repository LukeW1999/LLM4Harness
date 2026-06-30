#!/usr/bin/env python3
"""M1 check: re-run silenced cases' LLM harness with --unwinding-assertions ON.
A SUCCESS->FAIL flip means the original SUCCESS was an under-unwinding artifact,
not genuine silencing. If verdicts stay SUCCESS, the bounds were sufficient."""
import json, subprocess, sys
from pathlib import Path
from concurrent.futures import ProcessPoolExecutor, as_completed
import run_mutation_oracle_cbmc as R

def run_with_ua(func, mutant_c, harness_c, idx, timeout=R.TIMEOUT):
    cfg = R.FUNC_CONFIGS.get(func)
    if cfg is None: return "NO_CFG"
    proof_sources   = [str(p) for p in cfg.get("proof_sources", [])]
    project_sources = [str(p) for p in cfg.get("project_sources", [])]
    project_sources[idx] = str(mutant_c)
    obj_bits = cfg.get("object_bits", 8)
    if obj_bits != 8:
        base = [f"-I{R.SRCDIR}/include", f"-I{R.PROOFDIR}/include", "-DCBMC",
                f"-DCBMC_OBJECT_BITS={obj_bits}",
                "-DCBMC_MAX_OBJECT_SIZE=(SIZE_MAX>>(CBMC_OBJECT_BITS+1))",
                "--object-bits", str(obj_bits)]
        defines = [d for d in cfg.get("defines", []) if "CBMC_OBJECT_BITS" not in d]
    else:
        base = list(R.COMMON_FLAGS); defines = list(cfg.get("defines", []))
    remove_flags = []
    for fn in cfg.get("remove_function_body", []): remove_flags += ["--remove-function-body", fn]
    cmd = (["cbmc"] + base + defines + ["--unwinding-assertions"] +
           list(cfg.get("unwind", [])) + list(cfg.get("unwindset", []) or []) +
           remove_flags + ["--function", f"{func}_harness"] +
           proof_sources + [str(harness_c)] + project_sources)
    try:
        p = subprocess.run(cmd, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    out = p.stdout
    bad = ("PARSING ERROR","CONVERSION ERROR","Invalid User Input","fatal error","compilation terminated")
    if any(b in (out+p.stderr) for b in bad): return "COMPILE_ERROR"
    if "VERIFICATION SUCCESSFUL" in out: return "SUCCESS"
    if "VERIFICATION FAILED" in out:
        return "FAIL(unwind)" if "unwinding assertion" in out else "FAIL(other)"
    return "UNKNOWN"

def task(t):
    func, mut, mc, harness, idx = t
    return (func, mut, run_with_ua(func, mc, harness, idx))

def main(dataset):
    oracle = R.EVAL_DIR / f"mutation_oracle_cbmc_{dataset}.json"
    data = json.loads(oracle.read_text())
    res = data.get("results", data)
    sil = [r for r in res if r.get("silenced")]
    tasks = []
    for r in sil:
        func = r["func"]; mut = r["mutant"]
        idx = R.get_mutated_source_idx(func)
        if idx is None: continue
        mc = R.MUTANTS_DIR / func / f"{mut}.c"
        harness = R.get_final_harness(R.RESULTS_DIR / dataset / func)
        if harness is None or not mc.exists(): continue
        tasks.append((func, mut, mc, harness, idx))
    print(f"[{dataset}] silenced={len(sil)} runnable={len(tasks)}")
    from collections import Counter
    verd = Counter()
    with ProcessPoolExecutor(max_workers=6) as ex:
        for fut in as_completed([ex.submit(task, t) for t in tasks]):
            f, m, v = fut.result(); verd[v] += 1
    print(f"  verdicts with --unwinding-assertions ON: {dict(verd)}")
    flips = verd.get("FAIL(unwind)", 0)
    print(f"  SUCCESS (bounds sufficient, silencing genuine): {verd.get('SUCCESS',0)}")
    print(f"  FAIL(unwind) (under-unwinding artifact): {flips}")
    print(f"  other (FAIL-other/timeout/CE): {sum(v for k,v in verd.items() if k not in ('SUCCESS','FAIL(unwind)'))}")

if __name__ == "__main__":
    for ds in sys.argv[1:]:
        main(ds)
