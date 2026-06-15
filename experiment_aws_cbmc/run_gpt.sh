#!/usr/bin/env bash
set -uo pipefail
cd /root/experiment_aws_cbmc
PYTHON=python3
M="openai/gpt-5.5"; SLUG="gpt55"
log(){ echo "[$(date '+%m-%d %H:%M:%S')] $*"; }
for COND in A G H; do
  log "=== START $COND / $M ==="
  OPENROUTER_MODEL="$M" PYTHONUNBUFFERED=1 $PYTHON scripts/feedback_loop.py \
      --all --condition "$COND" --model openrouter --save-json \
      > "logs/run_${COND}_${SLUG}.log" 2>&1
  log "=== END $COND / $M ==="
  $PYTHON scripts/build_iteration_log.py --condition "${COND}_${SLUG}" --overwrite >> logs/gpt_iterlog.log 2>&1
done
log "GPT55_QUEUE_DONE"
