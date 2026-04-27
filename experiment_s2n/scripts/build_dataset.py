#!/usr/bin/env python3
"""
build_dataset.py
================
Extract function metadata (declaration, docstring, implementation)
for the 25 selected s2n_stuffer target functions.
Outputs: ../dataset/s2n_stuffer_dataset.json
"""

import json
import re
from pathlib import Path

S2N_DIR = Path("/home/weiqi/Verification/s2n-tls")
STUFFER_H   = S2N_DIR / "stuffer/s2n_stuffer.h"
STUFFER_C   = S2N_DIR / "stuffer/s2n_stuffer.c"
PROOFS_DIR  = S2N_DIR / "tests/cbmc/proofs"
DATASET_DIR = Path(__file__).parent.parent / "dataset"
DATASET_DIR.mkdir(parents=True, exist_ok=True)

# 25 target functions (diverse coverage; all GT-verified SUCCESS ≤ 60s)
TARGET_FUNCS = [
    "s2n_stuffer_init",
    "s2n_stuffer_alloc",
    "s2n_stuffer_growable_alloc",
    "s2n_stuffer_free",
    "s2n_stuffer_resize",
    "s2n_stuffer_resize_if_empty",
    "s2n_stuffer_rewrite",
    "s2n_stuffer_rewind_read",
    "s2n_stuffer_wipe",
    "s2n_stuffer_wipe_n",
    "s2n_stuffer_skip_read",
    "s2n_stuffer_skip_write",
    "s2n_stuffer_read",
    "s2n_stuffer_write",
    "s2n_stuffer_read_bytes",
    "s2n_stuffer_write_bytes",
    "s2n_stuffer_erase_and_read",
    "s2n_stuffer_read_uint8",
    "s2n_stuffer_write_uint8",
    "s2n_stuffer_read_uint32",
    "s2n_stuffer_write_uint32",
    "s2n_stuffer_copy",
    "s2n_stuffer_extract_blob",
    "s2n_stuffer_reserve_space",
    "s2n_stuffer_is_consumed",
]


def extract_header_decl(func_name: str, header_text: str) -> tuple[str, str]:
    """
    Return (docstring, declaration) for func_name from header.
    docstring: comment lines immediately before the declaration (may be empty)
    declaration: the function signature line(s) ending with ;
    """
    lines = header_text.splitlines()
    doc_lines = []
    decl_lines = []
    in_doc = False
    in_decl = False

    for i, line in enumerate(lines):
        # Check if this line contains our function
        if not in_decl and re.search(r'\b' + re.escape(func_name) + r'\s*\(', line):
            # Scan backwards for comments
            j = i - 1
            temp_doc = []
            while j >= 0:
                stripped = lines[j].strip()
                if stripped.startswith('*') or stripped.startswith('/*') or stripped.startswith('//'):
                    temp_doc.insert(0, lines[j])
                    j -= 1
                elif stripped == '':
                    j -= 1
                    continue
                else:
                    break
            # Only keep consecutive comment block
            doc_lines = temp_doc

            # Collect declaration (may span multiple lines until ';')
            decl_lines = [line]
            if ';' not in line and ')' not in line:
                k = i + 1
                while k < len(lines) and ';' not in ''.join(decl_lines):
                    decl_lines.append(lines[k])
                    k += 1
            in_decl = True
            break

    docstring = "\n".join(doc_lines).strip()
    declaration = "\n".join(decl_lines).strip()
    return docstring, declaration


def extract_implementation(func_name: str, source_text: str) -> str:
    """
    Extract the C implementation of func_name from source_text.
    Returns the complete function body from '{' to matching '}'.
    """
    lines = source_text.splitlines()
    start_line = -1

    # Find function definition: return_type func_name(
    pattern = re.compile(r'(?:^|\s)' + re.escape(func_name) + r'\s*\(')
    for i, line in enumerate(lines):
        if pattern.search(line):
            # Make sure it's a definition (not a declaration), look for '{' nearby
            # Search forward up to 5 lines for '{'
            for k in range(i, min(i+6, len(lines))):
                if '{' in lines[k]:
                    start_line = i
                    brace_line = k
                    break
            if start_line >= 0:
                break

    if start_line < 0:
        return ""

    # Collect from start_line to closing brace
    depth = 0
    result = []
    for i in range(start_line, len(lines)):
        line = lines[i]
        result.append(line)
        for ch in line:
            if ch == '{':
                depth += 1
            elif ch == '}':
                depth -= 1
        if depth == 0 and i >= brace_line:
            break

    return "\n".join(result)


def get_gt_harness(func_name: str) -> str:
    """Return ground truth harness source."""
    harness = PROOFS_DIR / func_name / f"{func_name}_harness.c"
    return harness.read_text() if harness.exists() else ""


def main():
    header_text = STUFFER_H.read_text()
    source_text = STUFFER_C.read_text()

    # Also need s2n_stuffer_*.c for hex/base64 functions
    extra_sources = {}
    for p in (S2N_DIR / "stuffer").glob("*.c"):
        extra_sources[p.name] = p.read_text()

    dataset = {}
    for func in TARGET_FUNCS:
        docstring, decl = extract_header_decl(func, header_text)

        # Try main stuffer.c first, then others
        impl = extract_implementation(func, source_text)
        if not impl:
            for fname, fsrc in extra_sources.items():
                impl = extract_implementation(func, fsrc)
                if impl:
                    break

        gt_harness = get_gt_harness(func)

        dataset[func] = {
            "func_name": func,
            "header_declaration": decl,
            "docstring": docstring,
            "implementation": impl,
            "gt_harness": gt_harness,
            "proof_dir": str(PROOFS_DIR / func),
        }

        decl_short = decl[:60].replace('\n', ' ')
        impl_lines = len(impl.splitlines()) if impl else 0
        print(f"  {func:45s}  decl={'OK' if decl else 'MISS'}  "
              f"impl={impl_lines:3d}L  gt={'OK' if gt_harness else 'MISS'}")

    out = DATASET_DIR / "s2n_stuffer_dataset.json"
    out.write_text(json.dumps(dataset, indent=2))
    print(f"\nDataset ({len(dataset)} functions) saved to {out}")


if __name__ == "__main__":
    main()
