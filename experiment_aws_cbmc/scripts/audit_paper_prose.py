"""Check the paper against the writing rules in CLAUDE.md.

paper_numbers_640.py checks that the numbers are true. audit_paper_coverage.py
checks that no number escaped the registry. This checks how the sentences read:
banned words, self-answering questions, sentence length, punctuation the rules
cut, overused constructions, and paragraphs carrying more numbers than a reader
can hold.

  python3 scripts/audit_paper_prose.py [path/to/paper.tex]
"""
import re, sys
from collections import Counter
from pathlib import Path

PAPER = Path(sys.argv[1]) if len(sys.argv) > 1 else Path.home() / "research/papers/EOY2/paper.tex"

BANNED_EN = ["delve", "tapestry", "embark", "fathom", "unlock", "elevate",
             "robust", "seamless", "furthermore", "moreover",
             "in today's fast-paced world"]
BANNED_ZH = ["深入探讨", "值得注意的是", "总而言之", "综上所述", "维度",
             "抓手", "赋能", "双刃剑", "显而易见", "在当今快节奏的时代"]
NOT_ABOUT = re.compile(r"it(?:'s| is) not about .{0,60}?it(?:'s| is) about", re.I)
XNOTY = re.compile(r"\b(?:rather than|,\s*not\b|and not\b)")
META = [r"The second finding explains the first", r"Which makes the next question",
        r"What is left is", r"bracket the answer", r"we return to it throughout",
        r"is the root of what follows", r"so we state up front",
        r"as we shall see", r"it is worth noting", r"in what follows"]

def strip_tex(t):
    t = re.sub(r"(?<!\\)%.*$", "", t, flags=re.M)
    # tikz uses ";" as a statement terminator and tables use "&"; neither is prose
    t = re.sub(r"\\begin\{(figure|figure\*|table|table\*|tikzpicture|lstlisting|tabular|abstract\*|CCSXML)\}.*?\\end\{\1\}", " ", t, flags=re.S)
    t = re.sub(r"\\keywords\{[^}]*\}", " ", t)
    t = re.sub(r"\\(?:cite\w*|ref|label|citeauthor)\{[^}]*\}", " ", t)
    t = re.sub(r"\\[a-zA-Z@]+\*?(\[[^\]]*\])?", " ", t)
    return re.sub(r"[{}$&\\~]", " ", t)

def prose_line_numbers(raw):
    """Line numbers outside figures, tables, tikz and listings."""
    out, depth = [], 0
    envs = ("figure", "table", "tikzpicture", "lstlisting", "tabular", "CCSXML")
    for i, l in enumerate(raw.splitlines(), 1):
        if re.search(r"\\begin\{(" + "|".join(envs) + r")\*?\}", l):
            depth += 1
        if depth == 0 and not l.lstrip().startswith("%") and l.strip():
            out.append(i)
        if re.search(r"\\end\{(" + "|".join(envs) + r")\*?\}", l):
            depth = max(0, depth - 1)
    return out


def sections(body):
    out, cur, buf = [], "(front matter)", []
    for line in body.splitlines():
        m = re.match(r"\\section\*?\{([^}]*)\}", line)
        if m:
            out.append((cur, "\n".join(buf))); cur, buf = m.group(1), []
        else:
            buf.append(line)
    out.append((cur, "\n".join(buf)))
    return out

def main():
    raw = PAPER.read_text(encoding="utf-8")
    body = raw.split(r"\begin{document}", 1)[-1].split(r"\bibliographystyle", 1)[0]
    lines = raw.splitlines()
    issues = Counter()

    def report(kind, items, note=""):
        issues[kind] = len(items)
        head = f"{kind} ({len(items)})"
        print(f"\n== {head} {note}")
        for it in items[:12]:
            print(f"   {it}")
        if len(items) > 12:
            print(f"   ... and {len(items) - 12} more")

    # 1. banned words, on the raw text so we catch them in captions too
    hits = []
    for lineno, line in enumerate(lines, 1):
        if line.lstrip().startswith("%"):
            continue
        low = line.lower()
        for w in BANNED_EN:
            for m in re.finditer(rf"\b{re.escape(w)}\b", low):
                hits.append(f"L{lineno:<4} {w:<12} ...{line[max(0,m.start()-40):m.start()+40].strip()}...")
        for w in BANNED_ZH:
            if w in line:
                hits.append(f"L{lineno:<4} {w}")
        if NOT_ABOUT.search(line):
            hits.append(f"L{lineno:<4} \"it's not about X, it's about Y\"")
    report("banned words", hits)

    # 2. a question answered by its own next words
    text = strip_tex(body)
    qa = []
    for m in re.finditer(r"([A-Z][^.?!]{10,120}\?)\s+(\S[^.?!]{0,80})", text):
        qa.append(f"{m.group(1).strip()[:80]} -> {m.group(2).strip()[:50]}")
    report("self-answering questions", qa, "(decide per case: some are RQ headings)")

    # 3. punctuation the rules cut
    prose_lines = prose_line_numbers(raw)
    em = [f"L{i}" for i in prose_lines
          if "---" in lines[i - 1] or "\u2014" in lines[i - 1]]
    semis = []
    for i in prose_lines:
        n = strip_tex(lines[i - 1]).count(";")
        if n:
            semis.append(f"L{i:<4} x{n}  ...{strip_tex(lines[i - 1]).strip()[:80]}...")
    report("em dashes", em)
    report("semicolons", semis)

    # 4. sentence length
    longs = []
    for s in re.split(r"(?<=[.!?])\s+", text):
        w = [x for x in s.split() if re.search(r"[A-Za-z]", x)]
        if len(w) > 30:
            longs.append(f"{len(w):>3}w  {' '.join(w)[:100]}")
    longs.sort(key=lambda x: -int(x.split("w")[0]))
    report("sentences over 30 words", longs)

    # 5. an overused construction
    report("\"X not Y\" constructions", [f"{c}x  {w}" for w, c in
                                        Counter(m.group(0).strip() for m in XNOTY.finditer(text)).most_common()],
           "(reserve for headings and the central claims)")

    # 6. numbers per paragraph
    dense = []
    for para in re.split(r"\n\s*\n", body):
        if para.lstrip().startswith("%") or "\\begin{" in para:
            continue
        p = strip_tex(para)
        nums = re.findall(r"(?<![\w.])\d+(?:[.,]\d+)?(?![\w])", p)
        if len(nums) > 6:
            dense.append(f"{len(nums):>3} numbers  {p.strip()[:90]}")
    dense.sort(key=lambda x: -int(x.split()[0]))
    report("paragraphs over 6 numbers", dense, "(rule is 2; 6 flags the worst)")

    # 7. meta-narration, per section
    meta = []
    for name, chunk in sections(body):
        found = [m.group(0) for pat in META for m in re.finditer(pat, chunk, re.I)]
        if len(found) > 1:
            meta.append(f"{name[:34]:36} {found}")
    report("sections with 2+ meta-narration lines", meta)

    print(f"\n---- {sum(issues.values())} flags across {len(issues)} checks")
    return sum(issues[k] for k in ("banned words",))

if __name__ == "__main__":
    sys.exit(1 if main() else 0)
