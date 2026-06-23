#!/usr/bin/env python3
"""
gf_audit.py -- Leave-one-out green-field audit.

gf_reference.py showed a synthesized (fidelity-gated, expert-free) reference
recovers ~99.5% of the expert GT-FAIL set. That measures the reference against
the *expert*. This script closes the loop WITHOUT any expert: it asks whether a
self-generated reference can audit a *peer* harness.

Protocol (pure green-field, no expert anywhere):
  For each pool harness h:
    1. h is the "harness under audit".
    2. h's silenced set S_h = { GT-FAIL-eligible mutant m : h has fidelity on m's
       func AND CBMC(h, m) = SUCCESS } -- the bugs h lets pass.
    3. Reference R_{-h} = union of the OTHER fidelity-passed harnesses.
    4. R_{-h} FLAGS m iff some member fails on m; each flag is a witnessed
       in-bound counterexample (not a vote). audit-recall_h = |flagged| / |S_h|.
  Aggregate across all hold-outs = "what fraction of a held-out harness's own
  silenced bugs does a peer-built reference surface", with NO ground truth used.

The heavy CBMC catch matrix is identical to gf_reference's, so we rebuild it once
(or load it from --matrix) and dump it to catch_matrix.json for reuse.

Usage:
  python3 gf_audit.py --exclude feedback_loop_A_v3,feedback_loop_A_gptoss120b_pin \
      --timeout 90 --workers 8 --out ../evaluation/gf_audit.json
  python3 gf_audit.py --matrix ../evaluation/catch_matrix.json   # skip CBMC
"""
import json, argparse, sys
from pathlib import Path
from collections import defaultdict
from concurrent.futures import ProcessPoolExecutor, as_completed

EXPERIMENT_DIR = Path("/root/experiment_aws_cbmc")
sys.path.insert(0, str(EXPERIMENT_DIR / "scripts"))

from gf_reference import (POOL_PREFIX, RESULTS_DIR, EVAL_DIR, MUTANTS_DIR,
                          pool_model, final_harness, load_gt_fail,
                          _fidelity_task, _catch_task)
from run_mutation_oracle_cbmc import get_mutated_source_idx


def build_catch_matrix(pools, gt_fail, gt_funcs, timeout, workers):
    """Returns (catch, fidelity) where catch[(func,mut)][pool]=bool over fidelity-passed
    candidates, fidelity[func][pool]=bool. Mirrors gf_reference stages 1-2 exactly."""
    idx_cache = {f: get_mutated_source_idx(f) for f in gt_funcs}

    # Stage 1: fidelity gate
    fid_tasks = []
    for pool in pools:
        for func in gt_funcs:
            hp = final_harness(pool, func)
            if hp is not None:
                fid_tasks.append((pool, func, str(hp), timeout))
    print(f"[fidelity] {len(fid_tasks)} candidate harnesses to verify on original")
    fidelity = defaultdict(dict)
    with ProcessPoolExecutor(max_workers=workers) as ex:
        for fut in as_completed([ex.submit(_fidelity_task, t) for t in fid_tasks]):
            pool, func, ok = fut.result()
            fidelity[func][pool] = ok
    n_pass = sum(1 for f in fidelity for p in fidelity[f] if fidelity[f][p])
    print(f"[fidelity] {n_pass}/{len(fid_tasks)} pass")

    # Stage 2: catch matrix
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
            catch_tasks.append((pool, func, mutant, str(mutant_c), str(hp), idx, timeout))
    print(f"[catch] {len(catch_tasks)} (candidate x GT-FAIL-mutant) CBMC runs")
    catch = defaultdict(dict)
    done = 0
    with ProcessPoolExecutor(max_workers=workers) as ex:
        for fut in as_completed([ex.submit(_catch_task, t) for t in catch_tasks]):
            pool, func, mutant, killed = fut.result()
            catch[(func, mutant)][pool] = killed
            done += 1
            if done % 200 == 0:
                print(f"   catch {done}/{len(catch_tasks)}")
    return catch, fidelity, n_pass, len(fid_tasks), len(catch_tasks)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--pool", default="")
    ap.add_argument("--exclude", default="")
    ap.add_argument("--timeout", type=int, default=90)
    ap.add_argument("--workers", type=int, default=8)
    ap.add_argument("--matrix", default="", help="load a dumped catch_matrix.json instead of re-running CBMC")
    ap.add_argument("--out", default=str(EVAL_DIR / "gf_audit.json"))
    args = ap.parse_args()

    if args.pool:
        pools = [p.strip() for p in args.pool.split(",") if p.strip()]
    else:
        excl = {x.strip() for x in args.exclude.split(",") if x.strip()}
        pools = sorted(d.name for d in RESULTS_DIR.glob(POOL_PREFIX + "*")
                       if d.is_dir() and d.name not in excl)
    print(f"[pool] {len(pools)} generators: {pools}")

    gt_fail, n_total, conflicts = load_gt_fail()
    gt_funcs = sorted({f for f, _ in gt_fail})
    print(f"[gt] {len(gt_fail)} GT-FAIL over {len(gt_funcs)} funcs")

    if args.matrix and Path(args.matrix).exists():
        raw = json.load(open(args.matrix))
        catch = {tuple(k.split("\x1f")): v for k, v in raw["catch"].items()}
        print(f"[matrix] loaded {len(catch)} catch rows from {args.matrix}")
        fid_pass = raw.get("fidelity_pass"); fid_total = raw.get("fidelity_total")
        catch_runs = raw.get("catch_runs")
    else:
        catch, fidelity, fid_pass, fid_total, catch_runs = build_catch_matrix(
            pools, gt_fail, gt_funcs, args.timeout, args.workers)
        # dump matrix for reuse (join key with unit separator)
        dump = {"catch": {"\x1f".join(k): v for k, v in catch.items()},
                "pools": pools, "fidelity_pass": fid_pass,
                "fidelity_total": fid_total, "catch_runs": catch_runs}
        mpath = EVAL_DIR / "catch_matrix.json"
        mpath.write_text(json.dumps(dump))
        print(f"[matrix] dumped -> {mpath}")

    # ---- Leave-one-out audit -------------------------------------------------
    # caught_by[key] = set of pools that kill this mutant; present[key] = pools with fidelity
    per_h = {}
    agg_sil = 0
    agg_flagged = 0
    residual_global = set()   # GT-FAIL pairs no harness at all catches
    for key in gt_fail:
        row = catch.get(key, {})
        if row and not any(row.values()):
            residual_global.add(key)

    for h in pools:
        S_h = [key for key in gt_fail
               if h in catch.get(key, {}) and catch[key][h] is False]
        flagged = 0
        unflag = []
        for key in S_h:
            row = catch.get(key, {})
            if any(row[p] for p in row if p != h):
                flagged += 1
            else:
                unflag.append(list(key))
        per_h[h] = {"model": pool_model(h),
                    "silenced": len(S_h),
                    "flagged_by_peers": flagged,
                    "audit_recall": (flagged / len(S_h) if S_h else None),
                    "unflagged": unflag}
        agg_sil += len(S_h)
        agg_flagged += flagged
        rr = per_h[h]["audit_recall"]
        rr_s = f"{100*rr:.1f}%" if rr is not None else "n/a"
        print(f"  hold-out {h:<36} model={per_h[h]['model']:<18} "
              f"silenced={len(S_h):>3} flagged-by-peers={flagged:>3} ({rr_s})")

    micro = agg_flagged / agg_sil if agg_sil else None
    macro_vals = [v["audit_recall"] for v in per_h.values() if v["audit_recall"] is not None]
    macro = sum(macro_vals) / len(macro_vals) if macro_vals else None

    print(f"\n=== LEAVE-ONE-OUT GREEN-FIELD AUDIT (no expert) ===")
    print(f"   total held-out silenced bugs : {agg_sil}")
    print(f"   flagged by peer-built reference: {agg_flagged}  (micro {100*micro:.1f}%)")
    print(f"   macro mean over {len(macro_vals)} hold-outs : {100*macro:.1f}%")
    print(f"   global residual (no harness catches): {len(residual_global)}  {sorted(residual_global)}")

    out = {
        "pools": pools,
        "gt_fail_total": len(gt_fail),
        "per_holdout": per_h,
        "aggregate": {"silenced": agg_sil, "flagged": agg_flagged,
                      "micro_audit_recall": micro, "macro_audit_recall": macro,
                      "n_holdouts_with_silences": len(macro_vals)},
        "global_residual": sorted(list(k) for k in residual_global),
        "fidelity_pass": fid_pass, "fidelity_total": fid_total, "catch_runs": catch_runs,
    }
    Path(args.out).write_text(json.dumps(out, indent=2))
    print(f"\n[out] {args.out}")


if __name__ == "__main__":
    main()
