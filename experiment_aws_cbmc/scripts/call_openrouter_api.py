#!/usr/bin/env python3
"""
call_openrouter_api.py - Call any model via OpenRouter (OpenAI-compatible endpoint).

Same call_qwen / read_file / get_function_name interface as the other backends so
feedback_loop.py can select this with --model openrouter.

Model is controlled by OPENROUTER_MODEL in .env:
  Primary:   openai/gpt-oss-120b   (fast on Cerebras: 726 tok/s)
  Secondary: deepseek/deepseek-v4-flash  (replication; different architecture)

Override per-run:
  OPENROUTER_MODEL=deepseek/deepseek-v4-flash python feedback_loop.py --condition A --model openrouter

Reproducibility: temperature=0, seed=42 for all models.
"""

import os
import re
import time
from pathlib import Path

import requests
from dotenv import load_dotenv

script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
project_root = experiment_dir.parent

# Shell env vars take precedence over .env for OPENROUTER_MODEL
# (so OPENROUTER_MODEL=deepseek/... python feedback_loop.py works correctly)
_shell_model = os.environ.get("OPENROUTER_MODEL")

for env_path in [project_root / ".env", experiment_dir / ".env"]:
    if env_path.exists():
        load_dotenv(dotenv_path=env_path, override=True)

# Restore shell override if it was set before .env loading
if _shell_model:
    os.environ["OPENROUTER_MODEL"] = _shell_model

API_KEY = os.getenv("OPENROUTER_API_KEY")
if not API_KEY:
    raise ValueError("OPENROUTER_API_KEY not found in .env")

MODEL_NAME = os.getenv("OPENROUTER_MODEL", "openai/gpt-oss-120b")

API_URL = "https://openrouter.ai/api/v1/chat/completions"


def call_qwen(system_prompt: str, user_prompt: str, temperature: float = 0.0,
              max_retries: int = 4, retry_delay: float = 10.0) -> str:
    headers = {
        "Authorization": f"Bearer {API_KEY}",
        "Content-Type": "application/json",
    }
    body = {
        "model": MODEL_NAME,
        "temperature": temperature,
        "seed": 42,
        "max_tokens": 8192,
        "messages": [
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt},
        ],
    }
    last_err = None
    for attempt in range(max_retries):
        try:
            r = requests.post(API_URL, headers=headers, json=body, timeout=300)
            r.raise_for_status()
            content = r.json()["choices"][0]["message"]["content"]
            if not content:
                # API returned empty/null content — treat as transient error and retry
                last_err = ValueError(f"API returned null content (attempt {attempt+1})")
                time.sleep(retry_delay * (2 ** attempt))
                continue
            # Strip any <think>...</think> blocks (some models emit these by default)
            if "<think>" in content:
                content = re.sub(r"<think>.*?</think>", "", content, flags=re.DOTALL).strip()
            return content
        except (requests.exceptions.SSLError,
                requests.exceptions.ConnectionError,
                requests.exceptions.Timeout) as e:
            last_err = e
            wait = retry_delay * (2 ** attempt)
            print(f"    [OpenRouter retry {attempt+1}/{max_retries}] {type(e).__name__}, "
                  f"waiting {wait:.0f}s...")
            time.sleep(wait)
        except requests.exceptions.HTTPError as e:
            raise
    raise last_err


def read_file(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def get_function_name(func_dir: str) -> str:
    parts = func_dir.split("_", 1)
    return parts[1] if len(parts) == 2 else func_dir
