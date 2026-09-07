#!/usr/bin/env bash
# Build the anonymised supplementary package for double-anonymous review.
#
# The public artifact (this branch) is NOT anonymous: its git history, its
# development logs, and its absolute paths all name the authors. This script
# exports a tree with none of that and refuses to finish if anything slips
# through.
#
#   ./make_anonymous_package.sh [output.tar.gz]
set -euo pipefail

HERE="$(cd "$(dirname "$0")" && pwd)"
ROOT="$(cd "$HERE/.." && pwd)"
OUT="${1:-$ROOT/llm4harness-supplementary.tar.gz}"
WORK="$(mktemp -d)"
trap 'rm -rf "$WORK"' EXIT

STAGE="$WORK/llm4harness"
mkdir -p "$STAGE"

# git archive, so no .git and no commit metadata reach the package
git -C "$ROOT" archive HEAD | tar -x -C "$STAGE"

# Files that exist to run the authors' own infrastructure or to record their
# progress. They are development history, not part of the artifact's claims.
DROP=(
  experiment_aws_cbmc/bootstrap_server.sh      # provisions the authors' compute host
  experiment_aws_cbmc/REBUILD.md               # server teardown/recovery notes
  experiment_aws_cbmc/DEVLOG.md                # dated development log
  experiment_aws_cbmc/proposal.md              # PhD proposal
  experiment_aws_cbmc/proposal.html
  experiment_aws_cbmc/figures/year3_timeline.py  # PhD timeline figure
  experiment_aws_cbmc/slides.tex               # internal talks
  experiment_aws_cbmc/slides_journal.tex
  research_design.md                           # running research log, dated and personal
  research_design_todolist.md
)
for f in "${DROP[@]}"; do rm -rf "${STAGE:?}/$f"; done
find "$STAGE" -name 'slides*.pdf' -delete

# Absolute paths and names inside the surviving files. Mutant metadata carries
# the generation path in 322 files, which is the bulk of this.
find "$STAGE" -type f \( -name '*.py' -o -name '*.sh' -o -name '*.md' -o -name '*.json' \
     -o -name '*.txt' -o -name '*.c' -o -name '*.ipynb' -o -name '*.html' \) -print0 \
  | xargs -0 sed -i \
      -e 's#/home/weiqi#/home/anon#g' \
      -e 's#github\.com[:/]LukeW1999/LLM4Harness#example.com/anonymous/artifact#g' \
      -e 's#[Ww]eiqi[ ._-]*[Ww]ang#Anonymous Author#g' \
      -e 's#\bWeiqi\b#Anonymous#g' \
      -e 's#\b[Ll]ukeW1999\b#anonymous#g' \
      -e 's#Lucas C\?\.\? \?Cordeiro#Anonymous Author#g' \
      -e 's#Marie Farrell#Anonymous Author#g' \
      -e "s#University of Manchester#an anonymised institution#g" \
      -e 's#\bManchester\b#Anonymised#g'

# The paper cites the authors' own prior work in third person, which is correct
# under double-anonymous review; that is a citation, not an identity leak, so
# bibliography-shaped mentions are left alone deliberately.

"$HERE/check_anonymity.sh" "$STAGE"

tar -czf "$OUT" -C "$WORK" llm4harness
echo "wrote $OUT ($(du -h "$OUT" | cut -f1))"
echo
echo "Before uploading, open the package and read INSTALL.md as a reviewer would:"
echo "  tar -tzf $OUT | head"
