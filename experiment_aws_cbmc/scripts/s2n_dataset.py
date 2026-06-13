#!/usr/bin/env python3
"""Generate dataset_condA/func__<s2n func>/ {implementation.c, header.h, ground_truth_harness.c}."""
import sys, re, subprocess
from pathlib import Path
sys.path.insert(0,"/root/experiment_aws_cbmc/scripts")
import cbmc_runner as C
PROOF=Path("/root/s2n-tls/tests/cbmc/proofs"); DS=Path("/root/experiment_aws_cbmc/dataset_condA")
HELPER="make_common_datastructures.c"
HEADERS=[Path("/root/s2n-tls/stuffer/s2n_stuffer.h"),Path("/root/s2n-tls/api/s2n.h")]

def find_func(func,fp):
    if not fp.exists(): return None,None,None
    lines=fp.read_text().split("\n")
    for i,l in enumerate(lines):
        if not re.search(rf"\b{re.escape(func)}\s*\(",l): continue
        if "{" not in "\n".join(lines[i:i+12]): continue
        depth=0; inf=False
        for j in range(i,len(lines)):
            depth+=lines[j].count("{")-lines[j].count("}")
            if depth>0: inf=True
            if inf and depth==0: return lines[i:j+1],i+1,j+1
    return None,None,None

def proto(func):
    for h in HEADERS:
        if not h.exists(): continue
        t=h.read_text().split("\n")
        for i,l in enumerate(t):
            if re.search(rf"\b{re.escape(func)}\s*\(",l) and ';' in "\n".join(t[i:i+4]):
                # gather until ;
                buf=[]
                for j in range(i,min(i+5,len(t))):
                    buf.append(t[j])
                    if ';' in t[j]: break
                return "\n".join(buf)
    return f"/* declaration of {func} (see s2n_stuffer.h) */"

funcs=sorted(k for k in C.FUNC_CONFIGS if k.startswith("s2n_stuffer") and not k.endswith("_harness"))
n=0
for f in funcs:
    cfg=C.FUNC_CONFIGS[f]; body=None
    for s in cfg["sources"]:
        if Path(s).name==HELPER: continue
        b,_,_=find_func(f,Path(s))
        if b: body="\n".join(b); break
    if not body: print(f,"NO BODY"); continue
    d=DS/f"func__{f}"; d.mkdir(parents=True,exist_ok=True)
    (d/"implementation.c").write_text(body+"\n")
    (d/"header.h").write_text(f"/* {f} -- s2n_stuffer API */\n#include <s2n.h>\n#include \"stuffer/s2n_stuffer.h\"\n\n{proto(f)}\n")
    gt=PROOF/f/f"{f}_harness.c"
    if gt.exists(): (d/"ground_truth_harness.c").write_text(gt.read_text())
    n+=1
print(f"generated {n} s2n dataset dirs")
