#!/usr/bin/env bash
# run_new_conditions.sh
# Run Conditions G and H for both primary (gpt-oss-120b) and replication (Qwen3.6 Plus) LLMs.
#
# Usage:
#   bash run_new_conditions.sh             # run all four batches sequentially
#   bash run_new_conditions.sh G openrouter  # single batch
#
# Each batch logs to logs/run_{COND}_{MODEL}.log
#
# Conditions:
#   G = single-pass, no feedback (gpt-oss-120b primary + Qwen3.6 replication)
#   H = strategy-neutral repair prompt (gpt-oss-120b primary + Qwen3.6 replication)
#   A = baseline iterative (Qwen3.6 replication only — primary A already done)

set -euo pipefail
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"
mkdir -p logs

run_batch() {
    local COND="$1"
    local MODEL="$2"
    local LOG="logs/run_${COND}_${MODEL}.log"
    echo "[$(date '+%H:%M:%S')] Starting Condition $COND / $MODEL → $LOG"
    python3 scripts/feedback_loop.py --all --condition "$COND" --model "$MODEL" --save-json \
        2>&1 | tee "$LOG"
    echo "[$(date '+%H:%M:%S')] Done: Condition $COND / $MODEL"
}

if [ $# -eq 2 ]; then
    run_batch "$1" "$2"
    exit 0
fi

# Full sequential run: G primary → H primary → A replication → G replication → H replication
run_batch G openrouter
run_batch H openrouter

# Replication LLM (Qwen3.6 Plus): switch OPENROUTER_MODEL
export OPENROUTER_MODEL=qwen/qwen3.6-plus
run_batch A openrouter
run_batch G openrouter
run_batch H openrouter

echo "All batches complete."
