# Install and smoke test

## Getting started (about 5 minutes)

```bash
git clone <artifact-url> llm4harness && cd llm4harness
python3 -m venv .venv && . .venv/bin/activate
pip install -r experiment_aws_cbmc/requirements.txt

cd experiment_aws_cbmc
python3 scripts/paper_numbers_640.py
```

Expected last line:

```
132 numbers checked, 0 mismatch(es). CBMC 6.4.0, denom=397
```

Each row shows a claim's location in the paper, the value the paper states, and
the value recomputed from the released per-mutant verdicts. This is the whole
functional claim: no model checker, no API key, no network.

`./artifact/smoke_test.sh` runs the same check plus a few structural assertions
and prints a one-line verdict.

## Regenerating the verdicts (hours)

Only this stage needs a model checker, and it must be the pinned one:

```bash
./scripts/get_cbmc640.sh                                     # verifies sha256, unpacks, never installs
export CBMC640=$HOME/tools/cbmc-6.4.0/extracted/usr/bin/cbmc

python3 scripts/gtfail_640.py        # GT verdicts on all 1,233 mutants -> the 397 GT-fail set (~15 min)
python3 scripts/silenced_640.py      # per-condition silenced/caught/unresolved (~25 min)
python3 scripts/passrate_640.py      # verifier pass rate per condition (~10 min)
python3 scripts/build_adjudicated_mechanism.py   # mechanism labels (no checker needed)
python3 scripts/paper_numbers_640.py             # audit again
```

Each `scripts/*_640.py` writes one `evaluation/*_640.json` and records the older
CBMC 5.95.1 verdict beside the new one, so a re-run is comparable rather than
destructive. Back up `evaluation/` first if you want to diff.

CBMC `SUCCESS`/`FAILURE` verdicts are load-independent, but `TIMEOUT` is not:
run with at most (cores - 1) workers and never two CBMC-heavy jobs at once, or
verdicts that should be decisive will come back `UNKNOWN`.

## Regenerating the harnesses (hours, costs money, non-deterministic)

```bash
export OPENROUTER_API_KEY=...        # or ANTHROPIC_API_KEY for the Claude conditions
python3 scripts/feedback_loop.py --condition A --model openrouter
```

This re-runs the iterative CBMC-feedback generation into
`results/feedback_loop_<C>_<model>/`. It will not reproduce the released
harnesses verbatim (see `REQUIREMENTS.md` on non-determinism); to reproduce the
paper's numbers, use the released harnesses.

## Re-adjudicating the mechanism labels

```bash
python3 scripts/make_adjudication_worksheet.py --sample 10   # blind rating sheet
# fill each `rating:` line in adjudication/worksheet.md with NW, Del, Nar or Unres
python3 scripts/score_adjudication.py --rater <name>         # agreement + Cohen's kappa
```

The sheet shows only raw evidence, never the automated label; `--sample N` keeps
every non-never-written group so the rare mechanisms are always rated.

## Layout

| Path | What |
|---|---|
| `experiment_aws_cbmc/` | Study 1: dataset, prompts, generated harnesses, mutants, verdicts, analysis |
| `experiment_aws_cbmc/scripts/` | the pipeline, the `*_640.py` re-runs, and the audit registry |
| `experiment_aws_cbmc/evaluation/` | released verdicts and derived results (the audit's inputs) |
| `experiment_s2n/` | Study 2: the s2n-tls cross-corpus replication |
| `experiment_aws_cbmc/REPRODUCE.md` | claim-to-script map, in more detail than this file |
| `experiment_aws_cbmc/ENVIRONMENT.md` | pinned versions, host spec, per-stage timings |
