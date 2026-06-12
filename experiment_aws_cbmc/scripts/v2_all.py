import importlib.util as u
spec=u.spec_from_file_location("av2","/root/experiment_aws_cbmc/scripts/attribution_v2.py")
av2=u.module_from_spec(spec); spec.loader.exec_module(av2)
print("\n===== validated attribution_v2 (cross-seed + screening) =====")
conds=["A_claude","A_claude_r2","A_claude_r3","A_claude_r4","A_claude_r5",
       "H_claude","H_claude_r3","H_claude_r4","H_claude_r5",
       "M_claude","M_claude_r3","M_claude_r4","M_claude_r5",
       "A_deepseekv4pro","G_deepseekv4pro","H_deepseekv4pro",
       "A_gpt55","G_gpt55","H_gpt55",
       "A_llama3370binstruct","G_llama3370binstruct","H_llama3370binstruct"]
for c in conds:
    r=av2.run(c)
    if r is None: continue
    s,_=r
    print(f"{c:<24} tot={s['total']:>2}  KG={s['KNOWLEDGE-GAP']:>2}  SAC={s['SACRIFICE']:>2}  AOC={s['AOC']:>2}  Unres={s['UNRESOLVED']:>2}")
