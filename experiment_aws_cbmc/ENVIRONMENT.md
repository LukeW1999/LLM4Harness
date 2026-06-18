# Environment & Toolchain

Pinned versions of everything needed to reproduce the experiments. Recorded
2026-06-16 from the production server.

## Hardware

| Machine | Spec |
|---------|------|
| **Compute server** (Alibaba ECS) | 8 vCPU Intel Xeon Platinum (4 cores x 2 threads/core), 14 GiB RAM, 40 GB disk, Ubuntu 24.04.4 LTS. Billing in stop-mode (data preserved across stops; **public IP may change on restart**). All CBMC oracle / cross-verify / mutant-generation runs ran here. |
| **Local workstation** | WSL2 Ubuntu on Windows; CBMC 6.8.0; the canonical repo and the `aws-c-common` source tree live here. |

### How the experiments were run
- **Mutant generation** (`gen_mutants.py`, `s2n_gen_mutants.py`): `universalmutator` over each target source file, compile-checked with the project's CBMC build flags; 1,233 compiled mutants over 58 mutant-bearing aws-c-common functions (plus the s2n_stuffer set).
- **Harness generation** (`feedback_loop.py`): per (function, condition, model), an iterative CBMC-feedback loop, max 15 iterations, temperature 0, nominal seed 42. Serving non-determinism makes repeated runs independent samples (the multi-run tables).
- **Differential oracle** (`run_mutation_oracle_cbmc.py`): each mutant is run through CBMC twice (expert H_GT, LLM H_LLM); a full 83-function sweep for one condition is about 2.7 CPU-hours (~2 min CBMC/function on this host).
- **Determinism**: CBMC SUCCESS/FAILURE verdicts are load-independent; only TIMEOUT is contention-sensitive, so workers are capped at cores-1 and no two CBMC-heavy jobs run concurrently (see Concurrency caveat). The pinned re-run (`*_pin`, DeepInfra bf16, fallbacks off) was done at low concurrency.

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
