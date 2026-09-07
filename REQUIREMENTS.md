# Requirements

## Hardware

| | |
|---|---|
| Minimum (audit only) | any x86-64 machine, 2 GB RAM, 1 GB disk. Recomputing every number in the paper from the released verdicts needs nothing more. |
| Recommended (re-running the oracle) | 8 vCPU, 14 GB RAM, 40 GB disk. This is the machine the paper's runs used: an 8 vCPU Intel Xeon Platinum instance (4 cores x 2 threads), Ubuntu 24.04. |
| Wall-clock | one condition over the 83-function corpus is about 2.7 CPU-hours of CBMC, trivially parallel. `ENVIRONMENT.md` gives per-stage timings. |

CBMC is memory-hungry on a few functions; with fewer than 8 GB some verdicts
turn into `UNKNOWN` and will not match the released JSONs.

## Software

| | |
|---|---|
| OS | Linux (Ubuntu 22.04 and 24.04 tested; WSL2 works) |
| Python | 3.12 (3.10 also works); packages in `experiment_aws_cbmc/requirements.txt` (numpy, scipy, pandas, matplotlib, requests) |
| CBMC | **6.4.0**, the version aws-c-common's own CI proofs run. Not packaged by any distribution: fetch with `experiment_aws_cbmc/scripts/get_cbmc640.sh` and export `$CBMC640`. Needed only to regenerate verdicts, not to audit them. |
| ESBMC | 8.3.0, for the cross-engine section only |
| gcc | for compile-filtering mutants |
| universalmutator | for regenerating mutants (the released mutants are in `experiment_aws_cbmc/mutants/`) |

## External services

Regenerating **harnesses** (as opposed to verdicts) calls hosted LLMs and costs
money:

| Backend | Used for | Key |
|---|---|---|
| OpenRouter | gpt-oss-120b (primary), DeepSeek, Llama, Qwen, GPT-5.5 | `OPENROUTER_API_KEY` |
| Anthropic | Claude Sonnet 4.6 | `ANTHROPIC_API_KEY` |

Keys are read from the environment or a gitignored `.env`; none ship with the
artifact. **No key is needed to reproduce any number in the paper**: every
generated harness is released under `experiment_aws_cbmc/results/`, so the
oracle, the attribution, and the audit all run offline.

Note that these APIs are non-deterministic even at temperature 0, and the
open-weight models are served in a precision the provider chooses, so a fresh
generation run will not reproduce the released harnesses verbatim. The paper
treats repeated runs as independent samples and reports the run-to-run spread;
`scripts/multirun_640.py` and `ENVIRONMENT.md` document this.

## Third-party corpora

`aws-c-common` (commit `2f65e15`) and `s2n-tls`, both Apache-2.0, with their
expert CBMC proof harnesses. The exact source snapshot the paper verified
against is preserved in the artifact, so no network fetch is needed and no
upstream change can silently move the ground truth.
