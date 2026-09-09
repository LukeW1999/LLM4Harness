#!/usr/bin/env bash
# Run the mutation-guided strengthening test on one condition's LIVE silencing
# functions only. A dead scaffold cannot be repaired by adding assertions, so
# testing it wastes CBMC time and tells us nothing.
#
#   scripts/run_strengthening.sh <condition> <model> <func> [<func> ...]
set -uo pipefail
COND="${1:?}"; MODEL="${2:?}"; shift 2
HERE="$(cd "$(dirname "$0")" && pwd)"; cd "$(dirname "$HERE")" || exit 1
for f in "$@"; do
  echo "=== $COND / $f"
  timeout 1200 python3 scripts/b2_repair.py --cond "$COND" --model "$MODEL" --func "$f" 2>&1 | tail -3
done
