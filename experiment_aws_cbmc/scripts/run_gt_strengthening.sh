#!/usr/bin/env bash
# GT-guided strengthening over every live silencing group, one function at a time.
#   scripts/run_gt_strengthening.sh <group-list-file>
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"; cd "$(dirname "$HERE")" || exit 1
while read -r cond funcs; do
  [ -z "$cond" ] && continue
  m=claude; case "$cond" in *gptoss*) m=openrouter;; esac
  for f in $funcs; do
    echo "=== $cond / $f"
    timeout 1500 python3 scripts/b2_repair.py --cond "$cond" --model "$m" --func "$f" --gt-guided 2>&1 | tail -2
  done
done < "$1"
echo "=== GT-guided strengthening finished at $(date -Is) ==="
