import importlib.util as u
spec=u.spec_from_file_location("av2","/root/experiment_aws_cbmc/scripts/attribution_v2.py")
av2=u.module_from_spec(spec); spec.loader.exec_module(av2)
A=av2.A; norm=av2.norm

def dump(cond):
    ds=f"feedback_loop_{cond}"
    r=av2.run(cond)
    if r is None: return
    summ,fd=r
    kgfuncs=[(f,n) for f,(lab,n) in fd.items() if lab=="KNOWLEDGE-GAP"]
    if not kgfuncs: return
    print(f"\n########## {cond}  (KG funcs: {len(kgfuncs)}) ##########")
    for f,n in sorted(kgfuncs):
        gts=A.get_gt_asserts(f); its=A.get_llm_iter_asserts(ds,f)
        maxit=max(its) if its else None
        final=set(norm(a) for a in its.get(maxit,[])) if maxit is not None else set()
        allllm=set(norm(a) for asl in its.values() for a in asl)
        print(f"\n=== {f}  ({n} silenced mutants) ===")
        print(f"  -- GT asserts MISSING from LLM final (drives KG):")
        for g in gts:
            if norm(g) in final:
                continue
            tag = "NEVER-ANY-ITER" if norm(g) not in allllm else "appeared-then-gone"
            print(f"     [{tag}] {g.strip()}")
        print(f"  -- LLM FINAL-iter (iter {maxit}) asserts (what it DID write):")
        wrote=its.get(maxit,[]) if maxit is not None else []
        if wrote:
            for a in wrote: print(f"     + {a.strip()}")
        else:
            print("     (none)")

for c in ["A_claude","A_claude_r3","H_claude","M_claude","A_deepseekv4pro","A_gptoss120b"]:
    dump(c)
