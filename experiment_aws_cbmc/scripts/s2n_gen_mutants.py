#!/usr/bin/env python3
"""s2n stuffer mutant generation (s2n config schema; finds func across cfg sources)."""
import sys, os, re, json, subprocess, tempfile, shutil, argparse
from pathlib import Path
sys.path.insert(0,"/root/experiment_aws_cbmc/scripts")
import cbmc_runner as C
PROOF=Path("/root/s2n-tls/tests/cbmc/proofs"); MUT_DIR=Path("/root/experiment_aws_cbmc/mutants_s2n")
HELPER="make_common_datastructures.c"

def find_func(func, fp):
    if not fp.exists(): return None,None
    lines=fp.read_text().split("\n")
    for i,l in enumerate(lines):
        if not re.search(rf"\b{re.escape(func)}\s*\(", l): continue
        if "{" not in "\n".join(lines[i:i+12]): continue
        depth=0; inf=False
        for j in range(i,len(lines)):
            depth+=lines[j].count("{")-lines[j].count("}")
            if depth>0: inf=True
            if inf and depth==0: return i+1,j+1
    return None,None

def src_of(func):
    for s in C.FUNC_CONFIGS[func]["sources"]:
        if Path(s).name==HELPER: continue
        a,b=find_func(func,Path(s))
        if a: return Path(s),a,b
    return None,None,None

def compile_ok(func, mutant, mutated_name):
    cfg=C.FUNC_CONFIGS[func]; h=str(PROOF/func/f"{func}_harness.c")
    srcs=[str(mutant) if Path(s).name==mutated_name else str(s) for s in cfg["sources"]]
    cmd=["cbmc"]+cfg["flags"]+["--function",cfg["harness_entry"],h]+srcs
    try:
        o=subprocess.run(cmd,capture_output=True,text=True,timeout=90); out=o.stdout+o.stderr
        return not any(e in out for e in ("PARSING ERROR","CONVERSION ERROR","fatal error","compilation terminated","error: unknown type"))
    except Exception: return False

def gen(func, force=False):
    out=MUT_DIR/func; meta=out/"metadata.json"
    if meta.exists() and not force: return json.loads(meta.read_text())
    src,s,e=src_of(func)
    if not src: return {"func":func,"status":"not_found","n_compiled":0}
    mutated_name=src.name
    with tempfile.NamedTemporaryFile("w",suffix=".txt",delete=False) as lf:
        for i in range(s,e+1): lf.write(f"{i}\n")
        lfn=lf.name
    with tempfile.TemporaryDirectory() as td:
        try:
            subprocess.run(["mutate",str(src),"c","--lines",lfn,"--noCheck","--mutantDir",td],capture_output=True,text=True,timeout=300)
        except subprocess.TimeoutExpired:
            os.unlink(lfn); return {"func":func,"status":"timeout","n_compiled":0}
        raw=sorted(Path(td).glob("*.c")); out.mkdir(parents=True,exist_ok=True); comp=[]
        for i,m in enumerate(raw):
            if compile_ok(func,m,mutated_name):
                d=out/f"mutant_{i:04d}.c"; shutil.copy2(m,d); comp.append(d.name)
    os.unlink(lfn)
    md={"func":func,"status":"ok","source_file":str(src),"mutated_name":mutated_name,"line_range":[s,e],"n_raw":len(raw),"n_compiled":len(comp)}
    meta.write_text(json.dumps(md,indent=2)); return md

if __name__=="__main__":
    ap=argparse.ArgumentParser(); ap.add_argument("--func"); ap.add_argument("--all",action="store_true"); ap.add_argument("--force",action="store_true")
    a=ap.parse_args()
    funcs=[a.func] if a.func else (sorted(k for k in C.FUNC_CONFIGS if k.startswith("s2n_stuffer") and not k.endswith("_harness")) if a.all else [])
    tot=0
    for f in funcs:
        m=gen(f,a.force); n=m.get('n_compiled',0); tot+=n
        print(f"{f}: raw={m.get('n_raw','?')} compiled={n} src={Path(m.get('source_file','?')).name} [{m.get('status')}]",flush=True)
    if len(funcs)>1: print(f"TOTAL compiled mutants: {tot}")
