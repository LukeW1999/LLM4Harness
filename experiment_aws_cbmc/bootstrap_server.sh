#!/usr/bin/env bash
# bootstrap_server.sh — one-shot setup for a FRESH Alibaba ECS spot instance
# (Ubuntu 24.04, x86_64) to re-run the LLM4Harness CBMC mutation-oracle pipeline.
#
# SPOT RULE: this box is disposable scratch. `git push` every result the moment
# it is computed; never keep the only copy of anything here. On reclaim the disk
# is wiped and you re-run this script.
#
# Usage (as root; Alibaba Ubuntu default login is root):
#   scp bootstrap_server.sh  root@<ip>:/root/
#   scp .env                 root@<ip>:/root/          # your API keys (gitignored)
#   ssh root@<ip> 'bash /root/bootstrap_server.sh'
#
# Needs a GitHub credential to clone the private repo. Easiest:
#   export GITHUB_TOKEN=ghp_xxx   (a PAT with repo read)  before running,
#   or run `gh auth login` first, or pre-clone the repo yourself.
set -euo pipefail

CBMC_VER="5.95.1"
REPO_HTTPS="github.com/LukeW1999/LLM4Harness.git"
REPO_ROOT="$HOME/LLM4Harness"
EXP_DIR="$REPO_ROOT/experiment_aws_cbmc"
AWS_COMMON_DIR="$HOME/aws-c-common"          # == /root/aws-c-common when run as root (matches cbmc_runner.py)
AWS_COMMON_COMMIT="2f65e1563770713a36517a44053aa84ed49314b8"

echo "==================== 1/6  system deps ===================="
export DEBIAN_FRONTEND=noninteractive
sudo apt-get update -y
sudo apt-get install -y build-essential git curl jq python3 python3-pip

echo "==================== 2/6  CBMC ${CBMC_VER} ===================="
# DO NOT `apt install cbmc` — that pulls 6.x, whose default-on unwinding/pointer
# checks make the LLM harnesses FAIL the fidelity gate on the unmutated function
# and collapse the differential (byte_buf_cat: 33 silenced -> 0). Verified 2026-08.
if cbmc --version 2>/dev/null | grep -q '5\.95\.1'; then
  echo "cbmc ${CBMC_VER} already present"
else
  cd /tmp
  API="https://api.github.com/repos/diffblue/cbmc/releases/tags/cbmc-${CBMC_VER}"
  URL=$(curl -sL "$API" | jq -r '.assets[].browser_download_url' | grep -iE 'ubuntu-22\.04.*\.deb' | head -1)
  [ -z "$URL" ] && URL=$(curl -sL "$API" | jq -r '.assets[].browser_download_url' | grep -iE 'ubuntu-20\.04.*\.deb' | head -1)
  [ -z "$URL" ] && { echo "FATAL: no CBMC ${CBMC_VER} .deb asset found at $API"; exit 1; }
  echo "downloading $URL"
  curl -sL "$URL" -o /tmp/cbmc.deb
  sudo apt-get install -y /tmp/cbmc.deb
fi
# HARD GATE: refuse to continue on the wrong version — a silent 6.x would void every result.
cbmc --version
cbmc --version | grep -q '5\.95\.1' || { echo "FATAL: cbmc is not ${CBMC_VER}; the study will NOT reproduce. Abort."; exit 1; }

echo "==================== 3/6  clone repos ===================="
if [ ! -d "$REPO_ROOT/.git" ]; then
  if [ -n "${GITHUB_TOKEN:-}" ]; then
    git clone "https://${GITHUB_TOKEN}@${REPO_HTTPS}" "$REPO_ROOT"
  else
    git clone "https://${REPO_HTTPS}" "$REPO_ROOT"   # will prompt / use gh auth
  fi
fi
if [ ! -d "$AWS_COMMON_DIR/.git" ]; then
  git clone https://github.com/awslabs/aws-c-common.git "$AWS_COMMON_DIR"
fi
git -C "$AWS_COMMON_DIR" fetch --all -q || true
git -C "$AWS_COMMON_DIR" checkout "$AWS_COMMON_COMMIT"
echo "aws-c-common at $(git -C "$AWS_COMMON_DIR" rev-parse --short HEAD)"
# Optional cross-corpus (§6.1) only — uncomment if you re-run the s2n set:
# [ ! -d "$HOME/s2n-tls/.git" ] && git clone https://github.com/aws/s2n-tls.git "$HOME/s2n-tls"

echo "==================== 4/6  python deps ===================="
cd "$EXP_DIR"
pip3 install --break-system-packages -r requirements.txt 2>/dev/null \
  || pip3 install -r requirements.txt

echo "==================== 5/6  .env check ===================="
if [ -f "$HOME/.env" ] && [ ! -f "$EXP_DIR/.env" ]; then cp "$HOME/.env" "$EXP_DIR/.env"; fi
if [ ! -f "$EXP_DIR/.env" ]; then
  echo "WARNING: no $EXP_DIR/.env — harness GENERATION will fail (audit still works)."
  echo "         upload .env with OPENROUTER_API_KEY / OPENROUTER_MODEL / ANTHROPIC_API_KEY / DASHSCOPE_API_KEY"
fi

echo "==================== 6/6  self-check (recompute every paper number) ===================="
python3 scripts/paper_numbers.py | tail -6 || true

echo
echo "==================== READY ===================="
echo "  * CBMC $(cbmc --version) | aws-c-common $(git -C "$AWS_COMMON_DIR" rev-parse --short HEAD)"
echo "  * Concurrency: run CBMC with --workers <= cores-1 (7 here); never 2 CBMC-heavy jobs at once."
echo "  * Disposable box: git push every result immediately."
echo "  * Smoke test:  python3 scripts/run_mutation_oracle_cbmc.py --dataset feedback_loop_A_gptoss120b --func aws_byte_buf_cat --workers 4"
echo "    (NOTE: that OVERWRITES evaluation/mutation_oracle_cbmc_<dataset>.json — canonical copy is git-tracked; restore/commit deliberately.)"
