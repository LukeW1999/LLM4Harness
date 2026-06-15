#!/bin/bash
# Wait for K/Oracle process to finish, then run M
cd /root/experiment_aws_cbmc
source .env
export OPENROUTER_MODEL="${OPENROUTER_MODEL:-openai/gpt-oss-120b}"
mkdir -p logs

echo "Waiting for K/Oracle to finish..."
while pgrep -f 'feedback_loop.py' > /dev/null; do
    sleep 30
done

echo "--- Condition M start: $(date) ---" | tee -a logs/nohup_m.out
/root/venv/bin/python3 scripts/feedback_loop.py     --all     --condition M     --model openrouter     --max-iter 15 2>&1 | tee -a logs/nohup_m.out
echo "--- Condition M done: $(date) ---" | tee -a logs/nohup_m.out
