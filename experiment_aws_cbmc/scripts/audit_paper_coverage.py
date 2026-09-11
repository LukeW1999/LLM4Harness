"""Which numbers in the paper is the registry not watching?

paper_numbers_640.py proves that the numbers it knows about are still true. It
cannot prove that it knows about all of them, and twice it did not: the default
pass-rate column had two of nine conditions registered, and the LLM-vs-expert
unwinding asymmetry had none. Both drifted. This walks every numeric literal in
paper.tex, drops the ones that are LaTeX rather than evidence, and reports what
no registry entry accounts for.

  python3 scripts/audit_paper_coverage.py [path/to/paper.tex]
"""
import re, sys, importlib.util
from collections import Counter
from pathlib import Path

HERE = Path(__file__).resolve().parent
PAPER = Path(sys.argv[1]) if len(sys.argv) > 1 else Path.home() / "research/papers/EOY2/paper.tex"

# ── registry ────────────────────────────────────────────────────────────────
def registry_values():
    spec = importlib.util.spec_from_file_location("pn", HERE / "paper_numbers_640.py")
    mod = importlib.util.module_from_spec(spec)
    sys.argv = [sys.argv[0]]                      # the registry parses argv itself
    spec.loader.exec_module(mod)
    out = []
    for loc, desc, claimed, fn, tol in mod.R:
        try:
            actual = fn()
        except Exception:
            actual = None
        out.append((loc, desc, float(claimed), None if actual is None else float(actual), tol))
    return out

# ── paper ───────────────────────────────────────────────────────────────────
# Structural numbers that are typography, not evidence.
SKIP_LINE = re.compile(r"""\s*\\(?:ccsdesc|keywords|scalebox|node|draw|path|fill|
    coordinate|begin\{tikzpicture|usetikzlibrary|setlength|documentclass|usepackage|
    cmidrule|midrule|toprule|bottomrule|newtcolorbox|lstset|renewcommand|newcommand)""",
    re.X)
STYLE_LINE = re.compile(r"^\s*\w+/\.style\s*=")
SKIP_CMD = re.compile(r"""\\(?:label|ref|eqref|cite\w*|citeauthor|input|include|
    hspace|vspace|setlength|tabcolsep|itemsep|arraystretch|emergencystretch|
    uchyph|includegraphics|multicolumn|cmidrule|scriptsize|footnotesize)""", re.X)
NUM = re.compile(r"(?<![\w.])(\d{1,3}(?:\d*)?(?:\.\d+)?)(?![\w])")
# "nine expert functions" escaped this audit until a readthrough caught it, so
# quantities spelled as words are checked too.
WORD_NUM = {"two": 2, "three": 3, "four": 4, "five": 5, "six": 6, "seven": 7,
            "eight": 8, "nine": 9, "ten": 10, "eleven": 11, "twelve": 12,
            "twenty": 20, "thirty": 30, "forty": 40, "fifty": 50}
COUNTED = re.compile(r"\b(" + "|".join(WORD_NUM) + r")\s+(?:of\s+the\s+\d|"
                     r"[a-z]*\s?(?:expert|silenc\w+|mutants?|functions?|groups?|"
                     r"harnesses|runs?|conditions?|models?|generators?|iterations?))",
                     re.I)

def paper_numbers(tex):
    body = tex.split(r"\begin{document}", 1)[-1]
    body = body.split(r"\bibliographystyle", 1)[0]
    out = []
    for lineno, line in enumerate(body.splitlines(), 1):
        line = re.sub(r"(?<!\\)%.*$", "", line)                 # comments
        # 1{,}233 and 1\,233 are one number, not two
        line = re.sub(r"(\d)(?:\{,\}|\\,|\\thinspace\s*)(\d{3})", r"\1\2", line)
        if not line.strip() or SKIP_LINE.match(line) or STYLE_LINE.match(line):
            continue
        raw_line = line
        scrubbed = SKIP_CMD.sub(" ", line)
        scrubbed = re.sub(r"\\ref\{[^}]*\}|\\cite\w*\{[^}]*\}|\\label\{[^}]*\}", " ", scrubbed)
        scrubbed = re.sub(r"\{[\d.]+pt\}|\d+pt|\d+em|\d+cm|p\{[\d.]+cm\}", " ", scrubbed)
        scrubbed = re.sub(r"S\\ref|\\S", " ", scrubbed)
        for m in COUNTED.finditer(scrubbed):
            word = m.group(1).lower()
            ctx = re.sub(r"\s+", " ", scrubbed[max(0, m.start()-60):m.end()+50]).strip()
            out.append((lineno, float(WORD_NUM[word]), word, ctx, raw_line))
        for m in NUM.finditer(scrubbed):
            raw = m.group(1)
            val = float(raw.replace(",", "").replace("\u2009", ""))
            ctx = re.sub(r"\s+", " ", scrubbed[max(0, m.start()-60):m.end()+60]).strip()
            out.append((lineno, val, raw, ctx, raw_line))
    return out

# Three kinds of number are not ours to verify against the registry.
CITED = re.compile(r"\\cite\w*\{|\{(?:chong2021|zhong2025impossiblebench|Qi2015|"
                   r"Smith2015|clopper1934|Just2014|Krakovna2020|yang2025harnessagent)\}")
PROTOCOL = re.compile(r"iterations?|timeout|time out at|\bs\b per|CPU-hours|"
                      r"thresholds|Clopper|CI\b|interval|95\\,\\%|unwind bound|"
                      r"object bound|per-mutant")
VERSIONS = re.compile(r"6\.4\.0|8\.3\.0|5\.95\.1|2026|2027|FSE|Sonnet|Llama|"
                      r"3\.3|4\.6|120b|70B|117B|GPT-5|CWE|RQ\d|\bp\s*=|p_\{|"
                      r"object-bits|MAX_BUFFER")

def classify(val, ctx):
    """cited | protocol | structural | measured"""
    if CITED.search(ctx):
        return "cited"
    if VERSIONS.search(ctx):
        return "structural"
    if PROTOCOL.search(ctx):
        return "protocol"
    if val in (0, 1, 2, 3, 4, 5) and not re.search(r"\d\s*(?:of|/)\s*\d", ctx):
        return "structural"
    return "measured"

def main():
    reg = registry_values()
    known = []
    for loc, desc, claimed, actual, tol in reg:
        known.append((claimed, loc, desc))
        if actual is not None and abs(actual - claimed) > tol:
            print(f"  DRIFT  {loc:14} {desc:48} paper={claimed} computed={actual}")
    nums = paper_numbers(PAPER.read_text(encoding="utf-8"))
    covered, uncovered, skipped = 0, [], Counter()
    for lineno, val, raw, ctx, raw_line in nums:
        kind = classify(val, ctx if not CITED.search(raw_line) else raw_line)
        if kind != "measured":
            skipped[kind] += 1
            continue
        hit = next((k for k in known if abs(k[0] - val) <= max(0.06, abs(val) * 0.002)), None)
        if hit:
            covered += 1
        else:
            uncovered.append((lineno, raw, ctx))
    print(f"\nregistry entries: {len(reg)}")
    print(f"measured numbers in the paper: {covered + len(uncovered)}  "
          f"covered: {covered}  uncovered: {len(uncovered)}")
    print(f"not checked: {dict(skipped)} (cited from other work, protocol "
          f"constants we chose, versions and prose integers)")
    if uncovered:
        print("\n-- measured, but no registry entry accounts for it --")
        for lineno, raw, ctx in uncovered:
            print(f"  L{lineno:<4} {raw:>8}   {ctx[:118]}")
    return len(uncovered)

if __name__ == "__main__":
    sys.exit(1 if main() else 0)
