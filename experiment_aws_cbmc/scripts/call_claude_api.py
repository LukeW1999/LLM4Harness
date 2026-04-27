#!/usr/bin/env python3
"""
call_claude_api.py - Call Anthropic Claude API to generate CBMC harnesses.

Provides the same call_qwen() interface as call_qwen_api.py so feedback_loop.py
can swap backends with minimal changes.
"""

import os
import sys
import time
from pathlib import Path

import anthropic
from dotenv import load_dotenv

# Load .env
script_dir = Path(__file__).parent
experiment_dir = script_dir.parent

for env_path in [experiment_dir / ".env", script_dir / ".env"]:
    if env_path.exists():
        load_dotenv(dotenv_path=env_path)
        break
else:
    load_dotenv()

API_KEY = os.getenv("ANTHROPIC_API_KEY")
if not API_KEY:
    raise ValueError("ANTHROPIC_API_KEY not found in .env")

MODEL_NAME = "claude-sonnet-4-6"

_client = anthropic.Anthropic(api_key=API_KEY)


def call_qwen(system_prompt: str, user_prompt: str, temperature: float = 0.0,
              max_retries: int = 4, retry_delay: float = 10.0) -> str:
    """
    Drop-in replacement for call_qwen_api.call_qwen(), using Claude instead.
    Same signature so feedback_loop.py can import this with no other changes.
    """
    last_err = None
    for attempt in range(max_retries):
        try:
            message = _client.messages.create(
                model=MODEL_NAME,
                max_tokens=8192,
                temperature=temperature,
                system=system_prompt,
                messages=[{"role": "user", "content": user_prompt}],
            )
            return message.content[0].text
        except anthropic.RateLimitError as e:
            last_err = e
            wait = retry_delay * (2 ** attempt)
            print(f"    [Claude retry {attempt+1}/{max_retries}] RateLimit, waiting {wait:.0f}s...")
            time.sleep(wait)
        except anthropic.APIConnectionError as e:
            last_err = e
            wait = retry_delay * (2 ** attempt)
            print(f"    [Claude retry {attempt+1}/{max_retries}] ConnectionError, waiting {wait:.0f}s...")
            time.sleep(wait)
        except anthropic.APIStatusError as e:
            # Don't retry on 4xx
            raise
    raise last_err


# Re-export helpers so feedback_loop.py can do: from call_claude_api import call_qwen, read_file, get_function_name
def read_file(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def get_function_name(func_dir: str) -> str:
    parts = func_dir.split("_", 1)
    return parts[1] if len(parts) == 2 else func_dir
