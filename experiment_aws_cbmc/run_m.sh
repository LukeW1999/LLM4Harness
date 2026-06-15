#!/bin/bash
set -e
cd /root/experiment_aws_cbmc
source .env
export OPENROUTER_MODEL="${OPENROUTER_MODEL:-openai/gpt-oss-120b}"
mkdir -p logs

echo "--- Condition M start: $(date) ---"
/root/venv/bin/python3 scripts/feedback_loop.py \
    --all \
    --condition M \
    --model openrouter \
    --max-iter 15
echo "--- Condition M done: $(date) ---"
