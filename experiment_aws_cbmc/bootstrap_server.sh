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

CBMC_VER="6.4.0"
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
# Pin 6.4.0. Do NOT `apt install cbmc`: it pulls whatever is current, and CBMC
# changed a default between 6.4.0 and 6.8.0 that decides these results.
#
#   cbmc 6.4.0, no unwinding flag        -> unwinding assertions OFF
#   cbmc 6.8.0, no unwinding flag        -> unwinding assertions ON
#
# aws-c-common's Makefile.common leaves CBMC_FLAG_UNWINDING_ASSERTIONS empty, so
# the production proofs inherit whichever default their CBMC has. With the check
# ON, an LLM harness whose loop outruns its bound is reported unverified instead
# of passing, and the silences behind such harnesses disappear (byte_buf_cat:
# 33 -> 0). That is the check working, not the differential collapsing: those
# harnesses never execute their postconditions (see scripts/reachability_probe.py
# and the dead-scaffold mechanism in the paper). Every script here passes the
# unwinding setting explicitly, so the version only decides what you get when
# you run CBMC by hand.
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
cbmc --version | grep -q '6\.4\.0' || { echo "FATAL: cbmc is not ${CBMC_VER}; the study will NOT reproduce. Abort."; exit 1; }

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
# Cross-corpus (§6.1): needed for any s2n run.
[ ! -d "$HOME/s2n-tls/.git" ] && git clone --depth 1 https://github.com/aws/s2n-tls.git "$HOME/s2n-tls"

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
python3 scripts/paper_numbers_640.py | tail -3 || true

echo
echo "==================== READY ===================="
echo "  * CBMC $(cbmc --version) | aws-c-common $(git -C "$AWS_COMMON_DIR" rev-parse --short HEAD)"
echo "  * Concurrency: run CBMC with --workers <= cores-1 (7 here); never 2 CBMC-heavy jobs at once."
echo "  * Disposable box: git push every result immediately."
echo "  * Smoke test:  python3 scripts/run_mutation_oracle_cbmc.py --dataset feedback_loop_A_gptoss120b --func aws_byte_buf_cat --workers 4"
echo "    (NOTE: that OVERWRITES evaluation/mutation_oracle_cbmc_<dataset>.json — canonical copy is git-tracked; restore/commit deliberately.)"
