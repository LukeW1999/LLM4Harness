#!/usr/bin/env bash
# Generate s2n-tls harnesses for one (condition, model) pair, one function at a
# time, so a single function that loops to the iteration cap cannot stall the
# rest. Generation is API-bound and runs wherever the keys are; only the CBMC
# calls inside the loop use local CPU, so a handful of these in parallel is fine.
#
#   scripts/run_s2n_conditions.sh <condition> <model> [run-tag]
#
# Reads OPENROUTER_API_KEY / ANTHROPIC_API_KEY from the environment.
set -uo pipefail

COND="${1:?usage: run_s2n_conditions.sh <condition> <model> [run-tag]}"
MODEL="${2:?}"
TAG="${3:-}"
HERE="$(cd "$(dirname "$0")" && pwd)"
EXP="$(dirname "$HERE")"
cd "$EXP" || exit 1

FUNCS=$(ls dataset_condA | grep '^func__s2n' | sed 's/func__//')
TOTAL=$(echo "$FUNCS" | wc -l)
echo "=== $COND / $MODEL over $TOTAL s2n functions ==="

i=0
for f in $FUNCS; do
  i=$((i + 1))
  printf '[%2d/%2d] %s ... ' "$i" "$TOTAL" "$f"
  args=(--func "$f" --condition "$COND" --model "$MODEL")
  [ -n "$TAG" ] && args+=(--run-tag "$TAG")
  if out=$(timeout 1800 python3 scripts/feedback_loop.py "${args[@]}" 2>&1); then
    echo "$out" | grep -oE 'Final: compile=[A-Z]+ verify=[A-Z_]+' | tail -1 || echo "done"
  else
    echo "FAILED (exit $?)"
  fi
done
echo "=== $COND / $MODEL finished ==="
