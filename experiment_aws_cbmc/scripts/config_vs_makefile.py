#!/usr/bin/env python3
"""
config_vs_makefile.py — does our build match the corpus's own proof build?

The s2n entries in cbmc_runner.py were transcribed by hand and had drifted from
the proofs' Makefiles, which made correct harnesses fail to convert and looked
like a model failure. This checks every function in both corpora against the
Makefile that the project's own CI uses, so that class of error cannot recur
silently.

Usage:  python3 scripts/config_vs_makefile.py
"""
import re, sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import cbmc_runner as C  # noqa: E402

def makefile_sources(mk, subs):
    out = {"PROOF_SOURCES": [], "PROJECT_SOURCES": []}
    for line in mk.read_text(errors="replace").splitlines():
        line = line.strip()
        if line.startswith("#") or "+=" not in line:
            continue
        var, val = (x.strip() for x in line.split("+=", 1))
        if var not in out or not val or "HARNESS_FILE" in val:
            continue
        for k, v in subs.items():
            val = val.replace(k, str(v))
        out[var].append(Path(val).name)
    return out

# Sources we link for every aws function even where that proof's Makefile does not.
# The LLM harnesses are prompted to call the proof-helper API, so a function whose
# expert harness happens not to use it would otherwise fail to link for the LLM
# side only. Both harnesses get the identical command line, so the differential
# comparison is unaffected; the build is simply more permissive than the project's.
UNIFORM_EXTRA = {"make_common_data_structures.c", "utils.c", "error.c",
                 "common.c", "allocator.c", "memcpy_override.c", "memset_override.c"}

def check(corpus, proofdir, subs, want):
    rows = []
    for func, cfg in sorted(C.FUNC_CONFIGS.items()):
        if not want(func):
            continue
        mk = proofdir / "proofs" / func / "Makefile"
        if not mk.exists():
            continue
        m = makefile_sources(mk, subs)
        expected = set(m["PROOF_SOURCES"] + m["PROJECT_SOURCES"])
        got = {Path(str(x)).name for x in
               (cfg.get("sources") or list(cfg.get("proof_sources", []))
                + list(cfg.get("project_sources", [])))}
        missing = sorted(expected - got)
        extra = sorted((got - expected) - UNIFORM_EXTRA)
        if missing or extra:
            rows.append((func, missing, extra))
    print(f"{corpus}: {len(rows)} function(s) differ from the project Makefile "
          f"(uniform proof-helper linkage excluded)")
    for func, missing, extra in rows:
        print(f"   {func}")
        if missing:
            print(f"      missing: {', '.join(missing)}")
        if extra:
            print(f"      extra:   {', '.join(extra)}")
    return len(rows)

def main():
    aws_root = Path(str(C.PROOF_SOURCE_DIR)).parent if hasattr(C, "PROOF_SOURCE_DIR") else None
    n = 0
    n += check("s2n-tls", C.S2N_PROOFDIR,
               {"$(PROOF_SOURCE)": C.S2N_PROOFDIR / "sources",
                "$(PROOF_STUB)": C.S2N_PROOFDIR / "stubs",
                "$(SRCDIR)": C.S2N_SRCDIR},
               lambda f: f.startswith("s2n_"))
    aws_proofdir = next((p for p in [Path(str(x)).parent.parent
                                     for x in C.FUNC_CONFIGS["aws_byte_buf_reset"]["proof_sources"]]
                         if (p / "proofs").is_dir()), None)
    if aws_proofdir:
        n += check("aws-c-common", aws_proofdir,
                   {"$(PROOF_SOURCE)": aws_proofdir / "sources",
                    "$(PROOF_STUB)": aws_proofdir / "stubs",
                    "$(SRCDIR)": Path(str(aws_proofdir)).parent.parent},
                   lambda f: f.startswith("aws_"))
    else:
        print("aws-c-common: proof directory not resolvable from the config")
    sys.exit(1 if n else 0)

if __name__ == "__main__":
    main()
