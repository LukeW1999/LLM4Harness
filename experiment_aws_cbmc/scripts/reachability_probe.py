#!/usr/bin/env python3
"""
reachability_probe.py — does the silencing harness actually reach the call?

A harness whose assumptions are unsatisfiable verifies against every mutant
without checking anything, which the mechanism rule reads as never-written
(the expert's assertions are absent from it in exact-match terms) when the real
cause is a scaffold that never puts the mutation on trial.

For each silenced (condition, function) group this injects `assert(0)`
immediately before the call under test in the final LLM harness and runs the
pinned CBMC:

  FAIL     the probe fires, so the call is reachable and the harness is live
  SUCCESS  the probe never fires, so the call is UNREACHABLE: a vacuous harness

Usage:  CBMC640=... python3 scripts/reachability_probe.py [--cond C] [--out FILE]
"""
import argparse, collections, json, os, re, shutil, subprocess, sys, tempfile, time
from pathlib import Path

HERE = Path(__file__).resolve().parent
EXP = HERE.parent
sys.path.insert(0, str(HERE))
from cbmc_runner import FUNC_CONFIGS, COMMON_FLAGS  # noqa: E402

CBMC = os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH = ["--no-standard-checks", "--no-unwinding-assertions"]
PROBE = "    assert(0); /* reachability probe */\n"

def silenced_groups():
    gt = {(r["func"], r["mutant"]): r["gt640"]
          for r in json.load(open(EXP / "evaluation/gtfail_640.json"))["verdicts"]}
    canon = {k for k, v in gt.items() if v == "FAIL"}
    llm = collections.defaultdict(dict)
    for f in ("silenced_640.json", "kllama_oracle_640.json"):
        for r in json.load(open(EXP / "evaluation" / f))["verdicts"]:
            llm[r["cond"]][(r["func"], r["mutant"])] = r["llm640"]
    out = []
    for cond, verdicts in llm.items():
        c = collections.Counter(f for (f, m) in canon if verdicts.get((f, m)) == "SUCCESS")
        out += [(cond, func, n) for func, n in c.items()]
    return sorted(out, key=lambda t: -t[2])

def final_harness(cond, func):
    d = EXP / "results" / f"feedback_loop_{cond}" / func
    its = sorted(d.glob("iter_*_harness.c"),
                 key=lambda p: int(re.search(r"iter_(\d+)_", p.name).group(1)))
    return its[-1] if its else None

def inject(src, func):
    """Put the probe immediately before the call under test."""
    lines = src.splitlines(keepends=True)
    call = re.compile(rf"^\s*(?:[\w\s\*]+=\s*)?{re.escape(func)}\s*\(")
    for i, ln in enumerate(lines):
        if call.match(ln):
            return "".join(lines[:i] + [PROBE] + lines[i:]), True
    return src, False

def run(func, text):
    cfg = FUNC_CONFIGS.get(func)
    if not cfg:
        return "NOCFG"
    tf = tempfile.NamedTemporaryFile("w", suffix="_harness.c", delete=False, dir="/tmp")
    tf.write(text); tf.close()
    cmd = ([CBMC] + list(COMMON_FLAGS) + list(cfg.get("defines", [])) + list(cfg["unwind"])
           + list(cfg.get("unwindset") or []) + MATCH
           + ["--function", f"{func}_harness"] + [str(p) for p in cfg["proof_sources"]]
           + [tf.name] + [str(p) for p in cfg["project_sources"]])
    try:
        o = subprocess.run(cmd, capture_output=True, text=True, timeout=300).stdout
    except subprocess.TimeoutExpired:
        return "TIMEOUT"
    finally:
        os.unlink(tf.name)
    if "VERIFICATION SUCCESSFUL" in o:
        return "SUCCESS"
    if "VERIFICATION FAILED" in o:
        return "FAIL"
    return "UNKNOWN"

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--cond")
    ap.add_argument("--out", default=str(EXP / "evaluation/reachability_probe_640.json"))
    args = ap.parse_args()

    groups = [g for g in silenced_groups() if not args.cond or g[0] == args.cond]
    rows, t0 = [], time.time()
    for cond, func, n in groups:
        h = final_harness(cond, func)
        if h is None:
            rows.append({"cond": cond, "func": func, "n": n, "probe": "NOHARNESS"})
        else:
            text, ok = inject(h.read_text(errors="replace"), func)
            rows.append({"cond": cond, "func": func, "n": n,
                         "probe": run(func, text) if ok else "NOCALLSITE",
                         "harness": h.name})
        r = rows[-1]
        print(f"{r['probe']:<10} {cond}/{func} ({n} silenced)", flush=True)

    vac = [r for r in rows if r["probe"] == "SUCCESS"]
    summary = {
        "cbmc": "6.4.0", "groups": len(rows), "silenced_total": sum(r["n"] for r in rows),
        "vacuous_groups": len(vac), "vacuous_silenced": sum(r["n"] for r in vac),
        "by_verdict": dict(collections.Counter(r["probe"] for r in rows)),
        "elapsed_s": round(time.time() - t0),
    }
    json.dump({"summary": summary, "rows": rows}, open(args.out, "w"), indent=1)
    print("\n" + json.dumps(summary, indent=1))

if __name__ == "__main__":
    main()
