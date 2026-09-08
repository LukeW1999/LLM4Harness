#!/usr/bin/env python3
"""
reachability_full_640.py — how far into the postcondition region does the harness get?

reachability_probe.py answers one question: can the FIRST assertion after the last
call site run. That is enough to call a harness dead, but not enough to call it
live, because a harness can reach its first postcondition and still have every
later one cut off by a truncated loop or a contradictory branch. The never-written
label assumes there was somewhere for the missing assertion to go, so we have to
know whether the tail of the harness is reachable, not just its head.

Three probes per group, each an `assert(0)` in the final LLM harness:

  head   before the first assertion following the last call site (the old probe)
  tail   before the LAST assertion in the harness
  end    immediately before the closing brace of the harness body

FAIL means the probe fires, so that point is reachable. SUCCESS means it never
fires, so that point is dead. A group is fully live only when `end` fires.

Usage:  CBMC640=... python3 scripts/reachability_full_640.py [--cond C]
"""
import argparse, collections, json, os, re, sys, time
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import reachability_probe as P  # noqa: E402

EXP = HERE.parent

def _workers():
    return int(os.environ.get("CBMC_WORKERS") or max(1, (os.cpu_count() or 8) - 1))

def positions(src, func):
    """(head, tail, end) insertion indices, or None where the harness has no such point."""
    lines = src.splitlines(keepends=True)
    call = re.compile(rf"^\s*(?:[\w\s\*]+=\s*)?{re.escape(func)}\s*\(")
    calls = [i for i, ln in enumerate(lines) if call.match(ln)]
    if not calls:
        return lines, {}
    asserts = [i for i, ln in enumerate(lines)
               if i > calls[-1] and re.match(r"\s*(?:__CPROVER_)?assert\s*\(", ln)]
    closes = [i for i, ln in enumerate(lines) if re.match(r"^\}", ln)]
    at = {}
    if asserts:
        at["head"] = asserts[0]
        at["tail"] = asserts[-1]
    if closes:
        at["end"] = closes[-1]
    return lines, at

def work(t):
    cond, func, n, src = t
    lines, at = positions(src, func)
    out = {"cond": cond, "func": func, "n": n}
    if not at:
        out["head"] = out["tail"] = out["end"] = "NOCALLSITE"
        return out
    for name, i in at.items():
        text = "".join(lines[:i] + [P.PROBE] + lines[i:])
        out[name] = P.run(func, text)
    return out

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cond")
    ap.add_argument("--out", default=str(EXP / "evaluation/reachability_full_640.json"))
    args = ap.parse_args()

    tasks = []
    for cond, func, n in P.silenced_groups():
        if args.cond and cond != args.cond:
            continue
        h = P.final_harness(cond, func)
        if h:
            tasks.append((cond, func, n, h.read_text(errors="replace")))

    # Resume: each group costs up to three 300s CBMC runs, so a killed process must
    # not throw away what it already decided.
    done = {}
    if Path(args.out).exists():
        done = {(r["cond"], r["func"]): r for r in json.load(open(args.out))["rows"]}
    tasks = [t for t in tasks if (t[0], t[1]) not in done]
    rows, t0 = list(done.values()), time.time()
    print(f"{len(done)} groups already done, {len(tasks)} to run", flush=True)

    def flush():
        json.dump({"summary": {"partial": bool(tasks), "groups": len(rows)},
                   "rows": sorted(rows, key=lambda r: -r["n"])}, open(args.out, "w"), indent=1)

    with ProcessPoolExecutor(max_workers=_workers()) as ex:
        futs = [ex.submit(work, t) for t in tasks]
        for f in as_completed(futs):
            r = f.result(); rows.append(r); flush()
            print(f"head={r.get('head'):<8} tail={r.get('tail'):<8} end={r.get('end'):<8} "
                  f"{r['cond']}/{r['func']} ({r['n']})", flush=True)

    live_head = [r for r in rows if r.get("head") == "FAIL"]
    tail_dead = [r for r in live_head if r.get("end") == "SUCCESS"]
    summary = {
        "cbmc": "6.4.0",
        "groups": len(rows),
        "head_reachable_groups": len(live_head),
        "head_reachable_silences": sum(r["n"] for r in live_head),
        "head_live_but_end_dead_groups": len(tail_dead),
        "head_live_but_end_dead_silences": sum(r["n"] for r in tail_dead),
        "elapsed_s": round(time.time() - t0),
    }
    json.dump({"summary": summary, "rows": sorted(rows, key=lambda r: -r["n"])},
              open(args.out, "w"), indent=1)
    print("\n" + json.dumps(summary, indent=1))

if __name__ == "__main__":
    main()
