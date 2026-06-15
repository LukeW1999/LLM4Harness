#!/usr/bin/env bash
# Queue: conditions A/G/H on three frontier OpenRouter models (SAC-threshold + conjunction screening)
set -uo pipefail
cd /root/experiment_aws_cbmc
PYTHON=$( [ -x /root/venv/bin/python3 ] && echo /root/venv/bin/python3 || echo python3 )
MODELS=("deepseek/deepseek-v4-pro" "qwen/qwen3.7-plus" "qwen/qwen3.7-max")
log(){ echo "[$(date '+%m-%d %H:%M:%S')] $*"; }

for M in "${MODELS[@]}"; do
  SLUG="${M##*/}"; SLUG="${SLUG//-/}"; SLUG="${SLUG//.}"
  for COND in A G H; do
    log "=== START $COND / $M (slug $SLUG) ==="
    OPENROUTER_MODEL="$M" PYTHONUNBUFFERED=1 $PYTHON scripts/feedback_loop.py \
        --all --condition "$COND" --model openrouter --save-json \
        > "logs/run_${COND}_${SLUG}.log" 2>&1
    log "=== END $COND / $M ==="
    $PYTHON scripts/build_iteration_log.py --condition "${COND}_${SLUG}" --overwrite >> logs/queue_iterlog.log 2>&1
  done
  log "##### MODEL DONE: $M #####"
done
log "QUEUE_ALL_DONE"
