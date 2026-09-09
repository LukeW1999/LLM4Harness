#!/usr/bin/env bash
# Regenerate the s2n Baseline arms, but only once the corpus itself is sound under
# our build. Generating against a build where the expert harnesses do not verify
# feeds the model compile errors that are ours, not its own, which is exactly how
# the first two attempts were wasted.
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"; cd "$(dirname "$HERE")" || exit 1

while pgrep -f "s2n_gt_[s]anity" &>/dev/null; do sleep 30; done

# A timeout is a resource limit, not a broken build. What must be zero is any
# harness the build cannot convert or that verifies false on the real function.
if grep -qE "no verdict|FAILED" logs/s2n_gt_sanity.log; then
  echo "=== gate FAILED, not regenerating:"; cat logs/s2n_gt_sanity.log; exit 1
fi
echo "=== gate passed at $(date -Is); regenerating both Baseline arms ==="

set -a; . "$HOME/.config/llm4harness/key.env"; set +a
export OPENROUTER_MODEL="openai/gpt-oss-120b"
export CBMC640="${CBMC640:-$HOME/tools/cbmc-6.4.0/extracted/usr/bin/cbmc}"

scripts/run_s2n_conditions.sh A openrouter fix2 &> logs/s2n_regen2_gptoss.log &
scripts/run_s2n_conditions.sh A claude     fix2 &> logs/s2n_regen2_claude.log &
wait
echo "=== both arms finished at $(date -Is) ==="
