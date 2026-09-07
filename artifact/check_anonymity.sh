#!/usr/bin/env bash
# Fail if anything author-identifying survives in a directory bound for
# double-anonymous review. Run it on an exported package, not on the repo.
#
#   ./check_anonymity.sh <dir>
set -uo pipefail
DIR="${1:?usage: check_anonymity.sh <dir>}"

# Author name, username, supervisors, institution, personal paths, the origin
# remote, and the compute host. Extend this list rather than trusting it.
PATTERNS='weiqi|lukew1999|luke[ _-]?w|cordeiro|farrell|manchester|/home/weiqi|github\.com[/:]LukeW1999|8\.211\.240|postgrad\.'

hits=$(grep -rIlE "$PATTERNS" "$DIR" 2>/dev/null)
gitdir=$([ -d "$DIR/.git" ] && echo "$DIR/.git" || true)

if [ -n "$gitdir" ]; then
  echo "FAIL: $DIR contains .git — commit metadata carries author names and emails."
  exit 1
fi

if [ -n "$hits" ]; then
  echo "FAIL: author-identifying strings in:"
  echo "$hits" | sed 's/^/  /'
  echo
  echo "First match per file:"
  echo "$hits" | while read -r f; do
    printf '  %s: %s\n' "$f" "$(grep -IhoEm1 "$PATTERNS" "$f")"
  done
  exit 1
fi

echo "PASS: no author-identifying string found in $DIR (patterns: $PATTERNS)"
