#!/usr/bin/env bash
# Wait for the in-flight gpt-oss s2n generation to drain, then run the Claude arm
# for the two conditions s2n is missing. Generation is API-bound, so the two
# streams run together.
#
#   nohup scripts/queue_s2n_claude.sh &> logs/s2n_claude.log &
set -uo pipefail
HERE="$(cd "$(dirname "$0")" && pwd)"
EXP="$(dirname "$HERE")"
cd "$EXP" || exit 1

# The bracket keeps the pattern from matching this script's own text in the
# command line of whatever shell spawned it, which would wait for ever.
while pgrep -f "feedback_loop.py .*--model o[p]enrouter" &>/dev/null; do
  sleep 60
done
echo "=== gpt-oss arm drained at $(date -Is); starting Claude ==="

set -a; . "$HOME/.config/llm4harness/key.env"; set +a

scripts/run_s2n_conditions.sh H claude &
scripts/run_s2n_conditions.sh M claude &
wait
echo "=== Claude arm finished at $(date -Is) ==="
