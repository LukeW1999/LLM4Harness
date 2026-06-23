#!/usr/bin/env python3
"""
gf_reference.py — Green-field auditing: synthesize a fail-side reference from a
POOL of independently generated (green-field, no-expert) harnesses, gate it for
fidelity, and measure how much of the expert fault-detection it recovers.

Core idea (keeps the certificate formal, NOT a vote):
  A candidate harness H certifies mutant m as a real fault iff
     (1) CBMC(H, original) = SUCCESS          [fidelity gate]
     (2) CBMC(H, m)        = FAIL             [distinguishes m from original]
  Both => m is a sound, witnessed in-bound counterexample, exactly like the
  expert oracle, regardless of where H came from. The synthesized reference
  R = union of fidelity-passed candidates; R catches m iff ANY member catches it.

Measuring stick: the expert GT-FAIL set (the (func,mutant) pairs the expert
harness kills, from the evaluation oracle JSONs). reference-recall = fraction of
that set R also kills. We also draw recall vs pool size K and compare
single-model vs cross-model pools.

Usage:
  python3 gf_reference.py --pool feedback_loop_A_claude,feedback_loop_A_claude_r2,... \
      --timeout 90 --workers 8 --out ../evaluation/gf_reference.json
  # default pool = all condition-A result dirs found.
"""
import json, argparse, random, sys, itertools
from pathlib import Path
from collections import defaultdict
from concurrent.futures import ProcessPoolExecutor, as_completed

EXPERIMENT_DIR = Path("/root/experiment_aws_cbmc")
RESULTS_DIR    = EXPERIMENT_DIR / "results"
EVAL_DIR       = EXPERIMENT_DIR / "evaluation"
MUTANTS_DIR    = EXPERIMENT_DIR / "mutants"
sys.path.insert(0, str(EXPERIMENT_DIR / "scripts"))

from cbmc_runner import run_cbmc, FUNC_CONFIGS
from run_mutation_oracle_cbmc import run_cbmc_on_mutant, get_mutated_source_idx

POOL_PREFIX = "feedback_loop_A_"


def pool_model(dirname: str) -> str:
    """feedback_loop_A_claude_r3 -> 'claude'; feedback_loop_A_gptoss120b -> 'gptoss120b'."""
    s = dirname[len(POOL_PREFIX):] if dirname.startswith(POOL_PREFIX) else dirname
    parts = s.split("_")
    if parts and parts[-1].startswith("r") and parts[-1][1:].isdigit():
        parts = parts[:-1]
    return "_".join(parts)


def final_harness(pool: str, func: str):
    d = RESULTS_DIR / pool / func
    hs = sorted(d.glob("iter_*_harness.c"))
    return hs[-1] if hs else None


def load_gt_fail():
    """Canonical GT-FAIL set across all oracle JSONs; gt is the expert harness,
    consistent across LLM conditions. Returns set of (func,mutant)."""
    gt = {}
    conflicts = 0
    for f in sorted(EVAL_DIR.glob("mutation_oracle_cbmc_*.json")):
        try:
            d = json.load(open(f))
        except Exception:
            continue
        for r in d.get("results", []):
            key = (r["func"], r["mutant"])
            v = r["gt"]
            if key in gt and gt[key] != v:
                # keep FAIL if any file says FAIL (expert catches it somewhere)
                if "FAIL" in (gt[key], v):
                    conflicts += 1
                    gt[key] = "FAIL"
            else:
                gt[key] = v
    fail = {k for k, v in gt.items() if v == "FAIL"}
    return fail, len(gt), conflicts


# ── parallel workers (module-level for pickling) ──────────────────────────────
def _fidelity_task(args):
    pool, func, hpath, timeout = args
    try:
        r = run_cbmc(func, Path(hpath), timeout=timeout)
        ok = (r.verification_result == "SUCCESS")
    except Exception as e:
        ok = False
    return (pool, func, ok)


def _catch_task(args):
    pool, func, mutant, mutant_c, hpath, idx, timeout = args
    try:
        v = run_cbmc_on_mutant(func, Path(mutant_c), Path(hpath), timeout, idx)
    except Exception:
        v = "ERROR"
    return (pool, func, mutant, v == "FAIL")


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pool", default="", help="comma list of result dir names; default=all condition-A dirs")
    ap.add_argument("--exclude", default="", help="comma list of dir names to exclude from default pool")
    ap.add_argument("--timeout", type=int, default=90)
    ap.add_argument("--workers", type=int, default=8)
    ap.add_argument("--boot", type=int, default=200, help="bootstrap reps for recall-vs-K")
    ap.add_argument("--out", default=str(EVAL_DIR / "gf_reference.json"))
    args = ap.parse_args()

    if args.pool:
        pools = [p.strip() for p in args.pool.split(",") if p.strip()]
    else:
        excl = {x.strip() for x in args.exclude.split(",") if x.strip()}
        pools = sorted(d.name for d in RESULTS_DIR.glob(POOL_PREFIX + "*")
                       if d.is_dir() and d.name not in excl)
    print(f"[pool] {len(pools)} candidate generators:")
    for p in pools:
        print(f"   {p}  (model={pool_model(p)})")

    gt_fail, n_total, conflicts = load_gt_fail()
    print(f"[gt] {len(gt_fail)} GT-FAIL (func,mutant) of {n_total} total; gt conflicts merged={conflicts}")
    gt_funcs = sorted({f for f, _ in gt_fail})
    print(f"[gt] spread over {len(gt_funcs)} functions")

    # precompute mutated-source idx per func
    idx_cache = {}
    for f in gt_funcs:
        idx_cache[f] = get_mutated_source_idx(f)

    # ── Stage 1: fidelity gate ────────────────────────────────────────────────
    fid_tasks = []
    for pool in pools:
        for func in gt_funcs:
            hp = final_harness(pool, func)
            if hp is not None:
                fid_tasks.append((pool, func, str(hp), args.timeout))
    print(f"[fidelity] {len(fid_tasks)} (pool,func) candidate harnesses to verify on original")
    fidelity = defaultdict(dict)   # fidelity[func][pool] = True/False
    done = 0
    with ProcessPoolExecutor(max_workers=args.workers) as ex:
        for fut in as_completed([ex.submit(_fidelity_task, t) for t in fid_tasks]):
            pool, func, ok = fut.result()
            fidelity[func][pool] = ok
            done += 1
            if done % 100 == 0:
                print(f"   fidelity {done}/{len(fid_tasks)}")
    n_pass = sum(1 for f in fidelity for p in fidelity[f] if fidelity[f][p])
    print(f"[fidelity] {n_pass}/{len(fid_tasks)} candidates pass the gate (verify SUCCESS on original)")

    # ── Stage 2: catch matrix (the only heavy CBMC step) ──────────────────────
    catch_tasks = []
    for (func, mutant) in sorted(gt_fail):
        idx = idx_cache.get(func)
        if idx is None:
            continue
        mutant_c = MUTANTS_DIR / func / f"{mutant}.c"
        if not mutant_c.exists():
            continue
        for pool in pools:
            if not fidelity.get(func, {}).get(pool):
                continue
            hp = final_harness(pool, func)
            if hp is None:
                continue
            catch_tasks.append((pool, func, mutant, str(mutant_c), str(hp), idx, args.timeout))
    print(f"[catch] {len(catch_tasks)} (candidate x GT-FAIL-mutant) CBMC runs")
    # catch[(func,mutant)][pool] = True if that candidate kills the mutant
    catch = defaultdict(dict)
    done = 0
    with ProcessPoolExecutor(max_workers=args.workers) as ex:
        for fut in as_completed([ex.submit(_catch_task, t) for t in catch_tasks]):
            pool, func, mutant, killed = fut.result()
            catch[(func, mutant)][pool] = killed
            done += 1
            if done % 200 == 0:
                print(f"   catch {done}/{len(catch_tasks)}")

    # ── Derive metrics from the catch matrix (no more CBMC) ────────────────────
    def recall_for(pool_subset):
        subset = set(pool_subset)
        caught = 0
        measurable = 0
        for key in gt_fail:
            row = catch.get(key, {})
            present = [p for p in subset if p in row]
            if not present:
                continue
            measurable += 1
            if any(row[p] for p in present):
                caught += 1
        return caught, measurable

    full_caught, full_meas = recall_for(pools)
    print(f"\n=== REFERENCE RECALL (full pool, K={len(pools)}) ===")
    print(f"   {full_caught}/{full_meas} GT-FAIL mutants recovered "
          f"= {100*full_caught/max(full_meas,1):.1f}%")

    # single-model pools
    by_model = defaultdict(list)
    for p in pools:
        by_model[pool_model(p)].append(p)
    per_model = {}
    for m, ps in sorted(by_model.items()):
        c, meas = recall_for(ps)
        per_model[m] = {"pools": ps, "caught": c, "measurable": meas,
                        "recall": (c / meas if meas else None)}
        print(f"   single-model [{m}] K={len(ps)}: {c}/{meas} = "
              f"{100*c/max(meas,1):.1f}%")

    # recall vs K (bootstrap random subsets of the full pool)
    random.seed(0)
    rk = {}
    for K in range(1, len(pools) + 1):
        vals = []
        reps = 1 if K == len(pools) else args.boot
        for _ in range(reps):
            sub = random.sample(pools, K)
            c, meas = recall_for(sub)
            if meas:
                vals.append(c / meas)
        if vals:
            vals.sort()
            mean = sum(vals) / len(vals)
            lo = vals[int(0.025 * len(vals))]
            hi = vals[min(len(vals) - 1, int(0.975 * len(vals)))]
            rk[K] = {"mean": mean, "lo": lo, "hi": hi, "reps": len(vals)}
            print(f"   recall@K={K:2d}: {100*mean:.1f}%  [{100*lo:.1f},{100*hi:.1f}]")

    # cross-model vs single-model at matched K: one seed per distinct model
    models = sorted(by_model.keys())
    cross = None
    if len(models) >= 2:
        Kc = len(models)
        vals = []
        for _ in range(args.boot):
            sub = [random.choice(by_model[m]) for m in models]
            c, meas = recall_for(sub)
            if meas:
                vals.append(c / meas)
        if vals:
            cross = {"K": Kc, "mean": sum(vals) / len(vals), "reps": len(vals)}
            # matched single-model: best single model using Kc of its seeds (or all it has)
            sm_vals = []
            for _ in range(args.boot):
                m = random.choice(models)
                ps = by_model[m]
                sub = random.sample(ps, min(Kc, len(ps)))
                c, meas = recall_for(sub)
                if meas:
                    sm_vals.append(c / meas)
            sm_mean = sum(sm_vals) / len(sm_vals) if sm_vals else None
            print(f"\n=== DIVERSITY (matched K={Kc}) ===")
            print(f"   cross-model pool : {100*cross['mean']:.1f}%")
            if sm_mean is not None:
                print(f"   single-model pool: {100*sm_mean:.1f}%")
            cross["single_model_matched_mean"] = sm_mean

    out = {
        "pools": pools,
        "models": {m: ps for m, ps in by_model.items()},
        "gt_fail_total": len(gt_fail),
        "full_pool": {"K": len(pools), "caught": full_caught,
                      "measurable": full_meas,
                      "recall": full_caught / max(full_meas, 1)},
        "per_model": per_model,
        "recall_vs_k": rk,
        "cross_vs_single": cross,
        "fidelity_pass": n_pass,
        "fidelity_total": len(fid_tasks),
        "catch_runs": len(catch_tasks),
    }
    Path(args.out).write_text(json.dumps(out, indent=2))
    print(f"\n[out] {args.out}")


if __name__ == "__main__":
    main()
