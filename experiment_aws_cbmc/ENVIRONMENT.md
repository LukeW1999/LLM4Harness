# Environment & Toolchain

Pinned versions of everything needed to reproduce the experiments. Recorded
2026-06-16 from the production server.

## Tools

| Tool   | Version          | Role                                              |
|--------|------------------|---------------------------------------------------|
| CBMC   | **5.95.1**       | primary model checker (oracle + cross-verify)     |
| ESBMC  | **8.3.0**        | secondary checker (RQ2 robustness, `esbmc_runner`)|
| Python | **3.12** (3.12.3)| all scripts                                       |

Python packages: see `requirements.txt`.

## LLM backends

Harness generation (`feedback_loop.py --model ...`) supports three backends.
The backend is resolved at runtime; results land in a model-suffixed directory
so different models never share a results dir.

| `--model`    | Provider / SDK            | Results dir suffix      | Notes                                   |
|--------------|---------------------------|-------------------------|-----------------------------------------|
| `openrouter` | OpenRouter HTTP           | `_<slug>` e.g. `_gptoss120b` | **Main model for the paper.** `OPENROUTER_MODEL=openai/gpt-oss-120b`, served bf16 (DeepInfra). |
| `claude`     | Anthropic SDK             | `_claude`               | cross-model replication (H_claude etc.) |
| `qwen`       | DashScope                 | `` (none)               | legacy / early runs                      |

The main reported corpus is **gpt-oss-120b via OpenRouter (DeepInfra, bf16)** →
results dir `feedback_loop_<COND>_gptoss120b`.

## API keys (secrets — never commit)

Keys load from a local `.env` (gitignored). Required keys by backend:

```
OPENROUTER_API_KEY=...      # --model openrouter
OPENROUTER_MODEL=openai/gpt-oss-120b
ANTHROPIC_API_KEY=...        # --model claude
DASHSCOPE_API_KEY=...        # --model qwen
```

`.env` MUST stay gitignored and must never reach GitHub or Overleaf.

## Concurrency caveat (determinism)

CBMC `SUCCESS`/`FAILURE` verdicts are load-independent and deterministic.
Only `TIMEOUT` is wall-clock/contention-sensitive. **Run CBMC workers ≤ cores−1
and never run two CBMC-heavy jobs concurrently** — a load spike during the
original K/Oracle runs caused spurious LLM-side TIMEOUTs (see DEVLOG / the
pinned-reproduction notes). The pinned re-run (`*_pin`) was done at low
concurrency to avoid this.

## Canonical data vs compute scratch

- **Canonical = this local repo** `experiment_aws_cbmc/{results,evaluation}`.
- **Server `/root/experiment_aws_cbmc` = compute scratch only** — it has
  diverged/been contaminated before (e.g. a stale 106-summary condition-A vs the
  clean 83). Always pull validated outputs back here; never treat the server copy
  as authoritative.

## Known portability debt

31 of the scripts hardcode `/root/...` paths (server layout). `paper_numbers.py`
has a `_BASE` toggle that falls back to local, but 2 of its checks still require
server-only ground-truth proofs and report `Permission denied: '/root'` off-server
(not numeric errors — path-portability bugs). Tracked as task #49.
