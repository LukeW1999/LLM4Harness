#!/bin/bash
set -e
cd /root/experiment_aws_cbmc
source .env
export OPENROUTER_MODEL="${OPENROUTER_MODEL:-openai/gpt-oss-120b}"
mkdir -p logs

run_condition() {
    local cond=$1
    echo "--- Condition $cond start: $(date) ---"
    /root/venv/bin/python3 scripts/feedback_loop.py \
        --all \
        --condition "$cond" \
        --model openrouter \
        --max-iter 15
    echo "--- Condition $cond done: $(date) ---"
}

run_condition K
run_condition Oracle
echo "K + Oracle rerun complete: $(date)"
