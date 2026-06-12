#!/bin/bash
# run_ablations.sh — Run ablation conditions I/J/K/Oracle on aws-c-common (83 functions)
# Uses gpt-oss-120b via OpenRouter.
# Run on server with: nohup bash run_ablations.sh > logs/ablations.log 2>&1 &

set -e
cd /root/experiment_aws_cbmc
source .env
export OPENROUTER_MODEL="${OPENROUTER_MODEL:-openai/gpt-oss-120b}"

mkdir -p logs

echo "============================================="
echo "Ablation Conditions I/J/K/Oracle"
echo "Model: $OPENROUTER_MODEL"
echo "Started: $(date)"
echo "============================================="

run_condition() {
    local cond=$1
    echo ""
    echo "--- Condition $cond ---"
    echo "Start: $(date)"
    /root/venv/bin/python3 scripts/feedback_loop.py \
        --all \
        --condition "$cond" \
        --model openrouter \
        --max-iter 15
    echo "Done: $(date)"
    # Count results
    dir="results/feedback_loop_${cond}_gptoss120b"
    if [ -d "$dir" ]; then
        count=$(ls "$dir" | wc -l)
        echo "  Results: $count/83"
    fi
}

run_condition I
run_condition J
run_condition K
run_condition Oracle

echo ""
echo "============================================="
echo "All ablation conditions complete."
echo "Results:"
for cond in I J K Oracle; do
    dir="results/feedback_loop_${cond}_gptoss120b"
    if [ -d "$dir" ]; then
        echo "  $cond: $(ls $dir | wc -l)/83"
    else
        echo "  $cond: directory not found"
    fi
done
echo "Finished: $(date)"
echo "============================================="
