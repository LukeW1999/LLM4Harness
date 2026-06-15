#!/usr/bin/env bash
cd /root/experiment_aws_cbmc
pkill -9 -f run_queue.sh 2>/dev/null
pkill -9 -f "feedback_loop.py.*openrouter" 2>/dev/null
sleep 1
rm -rf results/feedback_loop_A_deepseekv4pro results/feedback_loop_G_deepseekv4pro results/feedback_loop_H_deepseekv4pro
: > logs/model_queue.log
grep '^PYTHON=' run_queue.sh
setsid bash run_queue.sh > logs/model_queue.log 2>&1 < /dev/null &
sleep 18
echo "=== run_queue alive? ==="; pgrep -af run_queue.sh | grep -v pgrep | head -1
echo "=== feedback_loop alive? ==="; pgrep -af "feedback_loop.py.*openrouter" | grep -v pgrep | head -1
echo "=== fresh log ==="; head -3 logs/model_queue.log
