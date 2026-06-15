#!/usr/bin/env bash
# run_all_new_conditions.sh
# Waits for any in-progress G run (PID in /tmp/g_run.pid) to finish,
# then sequentially runs:
#   Condition H  — gpt-oss-120b  (primary)
#   Condition A  — Qwen3.6 Plus  (replication baseline)
#   Condition G  — Qwen3.6 Plus  (replication)
#   Condition H  — Qwen3.6 Plus  (replication)
#
# Progress is logged to logs/run_{COND}_{TAG}.log

set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"
mkdir -p logs

log() { echo "[$(date '+%H:%M:%S')] $*"; }

run_batch() {
    local COND="$1" MODEL="$2" MODEL_OVERRIDE="${3:-}"
    local TAG="${MODEL_OVERRIDE:-$MODEL}"
    TAG="${TAG//\//_}"          # replace / with _ for filename
    local LOG="logs/run_${COND}_${TAG}.log"

    log "Starting Condition $COND / model=$MODEL (override=${MODEL_OVERRIDE:-none}) → $LOG"
    if [ -n "$MODEL_OVERRIDE" ]; then
        OPENROUTER_MODEL="$MODEL_OVERRIDE" \
            python3 scripts/feedback_loop.py --all --condition "$COND" --model "$MODEL" --save-json \
            2>&1 | tee "$LOG"
    else
        python3 scripts/feedback_loop.py --all --condition "$COND" --model "$MODEL" --save-json \
            2>&1 | tee "$LOG"
    fi
    log "Done: Condition $COND / ${MODEL_OVERRIDE:-$MODEL}"
    echo ""
}

# ── 1. Wait for the existing G/gpt-oss-120b run to finish ──────────────────
G_PID=$(pgrep -f "feedback_loop.*condition G.*openrouter" 2>/dev/null | head -1 || true)
if [ -n "$G_PID" ]; then
    log "Waiting for existing Condition G run (PID $G_PID) to finish..."
    while kill -0 "$G_PID" 2>/dev/null; do
        DONE=$(ls results/feedback_loop_G_openrouter/ 2>/dev/null | wc -l || echo 0)
        log "  G/gpt-oss-120b: ${DONE}/84 functions done..."
        sleep 15
    done
    log "Condition G / gpt-oss-120b complete."
fi

DONE=$(ls results/feedback_loop_G_openrouter/ 2>/dev/null | wc -l || echo 0)
log "G/gpt-oss-120b final count: $DONE functions"

# ── 2. Condition H — gpt-oss-120b (primary) ────────────────────────────────
run_batch H openrouter

# ── 3. Replication: A, G, H — DeepSeek V4 Flash ───────────────────────────
REPLICATION_MODEL="deepseek/deepseek-v4-flash"
run_batch A openrouter "$REPLICATION_MODEL"
run_batch G openrouter "$REPLICATION_MODEL"
run_batch H openrouter "$REPLICATION_MODEL"

# ── 4. Run iteration logger on all new conditions ──────────────────────────
log "Running iteration logger on new conditions..."
python3 scripts/build_iteration_log.py --all 2>&1 | tee logs/iteration_log_update.log
log "Iteration logger complete."

log "=========================================="
log "All new conditions complete. Summary:"
for cond in G H; do
    for tag in openrouter qwen_qwen3.6-plus; do
        dir="results/feedback_loop_${cond}_${tag}"
        [ -d "$dir" ] && log "  $cond/$tag: $(ls $dir | wc -l) functions" || true
    done
done
log "Replication A/deepseek-v4-flash: $(ls results/feedback_loop_A_openrouter 2>/dev/null | wc -l) functions"
