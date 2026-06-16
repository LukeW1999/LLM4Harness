import importlib.util as u, sys
spec=u.spec_from_file_location("av2","/root/experiment_aws_cbmc/scripts/attribution_v2.py")
av2=u.module_from_spec(spec); spec.loader.exec_module(av2)  # (prints hardcoded list; ignore)
print("\n===== VALIDATED attribution_v2 on r3 + deepseek =====")
for c in ["A_claude","A_claude_r2","A_claude_r3","A_deepseekv4pro"]:
    r=av2.run(c)
    if r is None: print(f"{c:<22} (no oracle file)"); continue
    s,fd=r
    print(f"{c:<22} tot={s['total']:>2}  KG={s['KNOWLEDGE-GAP']:>2}  SAC={s['SACRIFICE']:>2}  AOC={s['AOC']:>2}  Unres={s['UNRESOLVED']:>2}")
    for f,(lab,n) in sorted(fd.items()): print(f"        {f:<38} {lab} x{n}")
