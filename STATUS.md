# Badges claimed

We apply for **Artifacts Available**, **Artifacts Evaluated — Functional**, and
**Artifacts Evaluated — Reusable**.

## Available

The artifact is archived at a public archival repository with a DOI (see the
paper's Data Availability section) and carries an open licence (`LICENSE`).

## Functional

Every number, table, and figure in the paper is recomputed from the released
verdicts by one command:

```bash
cd experiment_aws_cbmc && python3 scripts/paper_numbers_640.py
```

It prints a per-number table with the paper's claimed value beside the
recomputed one and reports `132 numbers checked, 0 mismatch(es)`. That is the
functional check we ask a reviewer to run first; it needs only Python and takes
about two minutes on a laptop with no model checker installed.

The pipeline that produced those verdicts is included and re-runnable end to
end: mutant generation, harness generation under each prompt condition, the
differential mutation oracle, mechanism attribution, and every follow-up study
(cloze, green-field, cross-corpus, cross-engine). `INSTALL.md` says which parts
need CBMC, which need an LLM API key, and what each costs in wall-clock time.

## Reusable

The differential mutation oracle is the reusable part, and it is not specific to
this paper's corpus or models:

- **A second corpus is already wired in.** `experiment_s2n/` applies the same
  oracle to s2n-tls; `scripts/s2n_*.py` show what a new corpus needs (a proof
  directory, per-function unwind bounds, a build configuration).
- **Models are swappable** behind one interface (`scripts/call_*_api.py`,
  selected by `feedback_loop.py --model`); the paper's own runs span five model
  families through it.
- **Checkers are swappable**: the same protocol runs under CBMC and ESBMC
  (`scripts/esbmc_runner.py`), and the CBMC version is a parameter (`$CBMC640`),
  which is how the 5.95.1 → 6.4.0 migration was audited rather than replaced.
- **The audit registry is a template.** `scripts/paper_numbers_640.py` maps each
  claim in the paper to the code that recomputes it, so a reuser can add claims
  and keep the same guarantee.
- **The mechanism attribution is inspectable and contestable.**
  `scripts/make_adjudication_worksheet.py` emits a blind rating sheet from the
  raw evidence and `scripts/score_adjudication.py` scores any rater against the
  automated rule, so the labels can be re-derived or overturned rather than
  taken on trust.

Documentation is at three levels: `README.md` (what the artifact is and the
claim-to-script map), `INSTALL.md` (setup, smoke test, and the cost of each
stage), and `REQUIREMENTS.md` (hardware, software, and external services).
