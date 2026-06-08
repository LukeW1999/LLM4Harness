#!/usr/bin/env bash
# run_replication_llama.sh
# Replication conditions A / G / H using Meta Llama 3.3 70B Instruct via OpenRouter.
#
# Replaces DeepSeek V4 Flash which was broken (97-100% UNKNOWN due to
# hallucinated proof_helpers/proof_allocators.h + wrong-function harnesses).
#
# Run on ECS server:
#   nohup bash run_replication_llama.sh > logs/llama_replication.log 2>&1 &
#   echo $! > /tmp/llama_run.pid

set -euo pipefail
cd "$(dirname "${BASH_SOURCE[0]}")"
mkdir -p logs

log() { echo "[$(date '+%H:%M:%S')] $*"; }

REPLICATION_MODEL="meta-llama/llama-3.3-70b-instruct"
PYTHON=/root/venv/bin/python3

run_batch() {
    local COND="$1"
    local TAG="${REPLICATION_MODEL//\//_}"
    local LOG="logs/run_${COND}_${TAG}.log"

    log "=== Condition $COND / ${REPLICATION_MODEL} ==="
    OPENROUTER_MODEL="$REPLICATION_MODEL" \
        PYTHONUNBUFFERED=1 \
        $PYTHON scripts/feedback_loop.py \
            --all \
            --condition "$COND" \
            --model openrouter \
            --save-json \
        2>&1 | tee "$LOG"
    log "Done: $COND / ${REPLICATION_MODEL}"
    echo ""
}

# ── Condition A (baseline with iterative CBMC feedback) ───────────────────
run_batch A

# ── Condition G (single-pass, no feedback) ────────────────────────────────
run_batch G

# ── Condition H (strategy-neutral repair) ─────────────────────────────────
run_batch H

# ── Build iteration logs for new results ──────────────────────────────────
SLUG="${REPLICATION_MODEL##*/}"
SLUG="${SLUG//-/}"
SLUG="${SLUG//.}"
log "Building iteration logs for A_${SLUG}, G_${SLUG}, H_${SLUG}..."
for COND in A G H; do
    COND_NAME="${COND}_${SLUG}"
    $PYTHON scripts/build_iteration_log.py --condition "$COND_NAME" --overwrite \
        2>&1 | tee -a logs/llama_iterlog.log
done

log "==========================================="
log "Llama 3.3 70B replication complete. Results:"
for COND in A G H; do
    SLUG_LC="${REPLICATION_MODEL##*/}"
    SLUG_LC="${SLUG_LC//-/}"
    SLUG_LC="${SLUG_LC//.}"
    DIR="results/feedback_loop_${COND}_${SLUG_LC}"
    [ -d "$DIR" ] && log "  $COND/llama: $(ls $DIR | wc -l) functions" || log "  $COND/llama: directory not found"
done
log "==========================================="
