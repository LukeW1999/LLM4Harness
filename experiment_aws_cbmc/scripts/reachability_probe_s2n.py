import sys, os, re, json, glob, subprocess, tempfile, shutil, collections
from pathlib import Path
sys.path.insert(0,"scripts"); import cbmc_runner as C
EXP=Path("/home/weiqi/research/projects/LLM4Harness/experiment_aws_cbmc")
S2N="/home/weiqi/research/projects/LLM4Harness/study_derivability/corpora/s2n-tls"
CBMC=os.environ.get("CBMC640") or shutil.which("cbmc") or "cbmc"
MATCH=["--no-standard-checks","--no-unwinding-assertions"]
def remap(x): return str(x).replace("/root/s2n-tls",S2N)
def final_llm(cond,func):
    hs=sorted(glob.glob(str(EXP/f"results/feedback_loop_{cond}/{func}/iter_*_harness.c")),
              key=lambda p:int(p.split('iter_')[1].split('_')[0]))
    return hs[-1] if hs else None
def inject(src,func):
    lines=src.splitlines(keepends=True)
    call=re.compile(rf"^\s*(?:[\w\s\*]+=\s*)?{re.escape(func)}\s*\(")
    calls=[i for i,l in enumerate(lines) if call.match(l)]
    if not calls: return src,False
    asserts=[i for i,l in enumerate(lines) if i>calls[-1] and re.match(r"\s*(?:__CPROVER_)?assert\s*\(",l)]
    at=asserts[0] if asserts else len(lines)-1
    return "".join(lines[:at]+["    assert(0); /* probe */\n"]+lines[at:]),True
def run(func,text):
    cfg=C.FUNC_CONFIGS.get(func)
    if not cfg or "sources" not in cfg: return "NOCFG"
    tf=tempfile.NamedTemporaryFile("w",suffix="_harness.c",delete=False,dir="/tmp"); tf.write(text); tf.close()
    cmd=[CBMC]+[remap(f) for f in cfg["flags"]]+list(cfg["unwind"])+(list(cfg["unwindset"]) if cfg.get("unwindset") else [])+MATCH+["--function",cfg["harness_entry"],tf.name]+[remap(s) for s in cfg["sources"]]
    try: o=subprocess.run(cmd,capture_output=True,text=True,timeout=300).stdout
    except subprocess.TimeoutExpired: return "TIMEOUT"
    finally: os.unlink(tf.name)
    return "SUCCESS" if "VERIFICATION SUCCESSFUL" in o else ("FAIL" if "VERIFICATION FAILED" in o else "UNKNOWN")
rows=json.load(open(EXP/"evaluation/s2n_640.json"))["rows"]
sil=collections.Counter((r["cond"],r["func"]) for r in rows if r["gt"]=="FAIL" and r["llm"]=="SUCCESS")
out=[]
for (cond,func),n in sil.most_common():
    h=final_llm(cond,func)
    if not h: v="NOHARNESS"
    else:
        t,ok=inject(open(h, errors="replace").read(),func); v=run(func,t) if ok else "NOCALLSITE"
    out.append({"cond":cond,"func":func,"n":n,"probe":v})
    print(f"{v:<10} {cond}/{func} ({n} silenced)",flush=True)
tot=sum(r["n"] for r in out); dead=sum(r["n"] for r in out if r["probe"]=="SUCCESS")
summary={"corpus":"s2n-tls","silenced":tot,"dead":dead,"pct":round(100*dead/tot,1) if tot else 0,
         "by_verdict":dict(collections.Counter(r["probe"] for r in out))}
json.dump({"summary":summary,"rows":out},open(EXP/"evaluation/reachability_probe_s2n_640.json","w"),indent=1)
print("\n"+json.dumps(summary,indent=1))
