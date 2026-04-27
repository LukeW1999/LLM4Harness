#!/usr/bin/env python3
"""
call_qwen_api.py - Call Qwen API to generate CBMC harnesses for aws-c-common functions.

Uses the DashScope OpenAI-compatible endpoint with qwen2.5-coder-32b-instruct.

Usage:
    python call_qwen_api.py --func <func_dir> --prompt <A|B|C> [--output <path>]

Examples:
    python call_qwen_api.py --func func1_aws_add_size_checked --prompt B
    python call_qwen_api.py --func func3_aws_array_list_back --prompt C
    python call_qwen_api.py --all --prompt B    # run all 5 pilot functions
    python call_qwen_api.py --all --prompt D    # with proof helpers API hint
"""

import os
import sys
import json
import time
import argparse
import requests
from pathlib import Path
from dotenv import load_dotenv

# Load env from project root
script_dir = Path(__file__).parent
experiment_dir = script_dir.parent
project_root = experiment_dir.parent

for env_path in [project_root / ".env", experiment_dir / ".env", script_dir / ".env"]:
    if env_path.exists():
        load_dotenv(dotenv_path=env_path)
        break
else:
    load_dotenv()

API_KEY = os.getenv("DASHSCOPE_API_KEY")
if not API_KEY:
    raise ValueError("DASHSCOPE_API_KEY not found in .env. Please add it.")

API_URL = "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions"
MODEL_NAME = "qwen2.5-coder-32b-instruct"

DATASET_DIR = experiment_dir / "dataset"
PROMPTS_DIR = experiment_dir / "prompts"
RESULTS_DIR = experiment_dir / "results"

PROMPT_FILES = {
    "A": "prompt_A_header_only.txt",
    "B": "prompt_B_header_plus_impl.txt",
    "C": "prompt_C_with_macro_hint.txt",
    "D": "prompt_D_with_proof_helpers.txt",
}

# All pilot functions
PILOT_FUNCTIONS = [
    "func1_aws_add_size_checked",
    "func2_aws_byte_buf_init",
    "func3_aws_array_list_back",
    "func4_aws_byte_buf_append",
    "func5_aws_linked_list_push_back",
]


def call_qwen(system_prompt: str, user_prompt: str, temperature: float = 0.0,
              max_retries: int = 4, retry_delay: float = 10.0) -> str:
    """Call Qwen via DashScope OpenAI-compatible API, with retry on transient errors."""
    headers = {
        "Authorization": f"Bearer {API_KEY}",
        "Content-Type": "application/json",
    }
    body = {
        "model": MODEL_NAME,
        "temperature": temperature,
        "max_tokens": 8192,
        "messages": [
            {"role": "system", "content": system_prompt},
            {"role": "user", "content": user_prompt},
        ],
    }
    last_err = None
    for attempt in range(max_retries):
        try:
            response = requests.post(API_URL, headers=headers, json=body, timeout=300)
            response.raise_for_status()
            return response.json()["choices"][0]["message"]["content"]
        except (requests.exceptions.SSLError,
                requests.exceptions.ConnectionError,
                requests.exceptions.Timeout) as e:
            last_err = e
            wait = retry_delay * (2 ** attempt)
            print(f"    [API retry {attempt+1}/{max_retries}] {type(e).__name__}, waiting {wait:.0f}s...")
            time.sleep(wait)
        except requests.exceptions.HTTPError as e:
            # Don't retry on 4xx client errors
            raise
    raise last_err


def read_file(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def get_function_name(func_dir: str) -> str:
    """Extract function name from directory name like func3_aws_array_list_back."""
    parts = func_dir.split("_", 1)
    return parts[1] if len(parts) == 2 else func_dir


def clean_code_response(content: str) -> str:
    """Strip markdown code fences from LLM response."""
    content = content.strip()
    if content.startswith("```c\n"):
        content = content[5:]
    elif content.startswith("```\n"):
        content = content[4:]
    if content.endswith("\n```"):
        content = content[:-4]
    elif content.endswith("```"):
        content = content[:-3]
    return content.strip() + "\n"


def build_prompt(prompt_type: str, func_dir: str) -> tuple[str, str]:
    """Build system and user prompts for the given function and prompt type."""
    func_name = get_function_name(func_dir)
    func_path = DATASET_DIR / func_dir

    header_content = read_file(func_path / "header.h")
    impl_content = read_file(func_path / "implementation.c")
    prompt_template = read_file(PROMPTS_DIR / PROMPT_FILES[prompt_type])

    system_prompt = "You are an expert in CBMC formal verification of C programs. Output only valid C code."

    # Fill template
    user_prompt = prompt_template
    user_prompt = user_prompt.replace("{HEADER_CONTENT}", header_content)
    user_prompt = user_prompt.replace("{FUNCTION_NAME}", func_name)

    if prompt_type == "A":
        # Header-only: don't include implementation
        user_prompt = user_prompt.replace("{IMPLEMENTATION_CONTENT}", "")
    else:
        user_prompt = user_prompt.replace("{IMPLEMENTATION_CONTENT}", impl_content)

    return system_prompt, user_prompt


def run_one(func_dir: str, prompt_type: str, output_path: Path = None) -> str:
    """Run Qwen on one function with the given prompt type. Returns generated harness."""
    func_name = get_function_name(func_dir)
    print(f"  Calling Qwen ({MODEL_NAME}) for {func_name} [Prompt {prompt_type}]...")

    system_prompt, user_prompt = build_prompt(prompt_type, func_dir)
    raw_response = call_qwen(system_prompt, user_prompt)
    harness_code = clean_code_response(raw_response)

    if output_path is None:
        result_subdir = RESULTS_DIR / f"prompt_{prompt_type}" / func_dir
        result_subdir.mkdir(parents=True, exist_ok=True)
        output_path = result_subdir / f"{func_name}_llm_harness.c"

    output_path.write_text(harness_code, encoding="utf-8")
    print(f"  Saved to: {output_path}")
    return harness_code


def main():
    parser = argparse.ArgumentParser(description="Generate CBMC harnesses with Qwen")
    parser.add_argument("--func", help="Function directory name (e.g. func1_aws_add_size_checked)")
    parser.add_argument("--prompt", choices=["A", "B", "C", "D"], default="B",
                        help="Prompt type: A=header-only, B=header+impl, C=with macro hints")
    parser.add_argument("--all", action="store_true", help="Run all 5 pilot functions")
    parser.add_argument("--output", help="Custom output file path")
    args = parser.parse_args()

    if args.all:
        print(f"\nRunning all pilot functions with Prompt {args.prompt}...\n")
        for func_dir in PILOT_FUNCTIONS:
            try:
                run_one(func_dir, args.prompt)
            except Exception as e:
                print(f"  ERROR for {func_dir}: {e}")
        print("\nDone.")
    elif args.func:
        output_path = Path(args.output) if args.output else None
        run_one(args.func, args.prompt, output_path)
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    main()
