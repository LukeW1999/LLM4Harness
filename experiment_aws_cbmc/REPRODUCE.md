# Reproducing the paper's numbers

Every number in the paper is recomputed by **`scripts/paper_numbers.py`**, the
audit registry. It reads the canonical `results/` and `evaluation/` data, recomputes
each claimed value, and prints `OK` or `MISMATCH` against the hardcoded paper claim.

> **Start here.** Environment + tool versions are in `ENVIRONMENT.md`;
> dependencies in `requirements.txt`.

```bash
cd experiment_aws_cbmc
python3 scripts/paper_numbers.py      # recomputes & checks every paper number
```

As of 2026-06-16: 181/183 OK; the 2 non-OK are path-portability bugs (server-only
ground-truth proofs), **not** numeric errors — see task #49 / `ENVIRONMENT.md`.

---

## The pipeline (data flow)

```
dataset_condA/         (83 aws-c-common functions + NL contracts)
   │  feedback_loop.py --condition <C> --model openrouter
   ▼
results/feedback_loop_<C>_gptoss120b/<func>/iter_*_harness.c , summary.json
   │
   ├─ run_mutation_oracle_cbmc.py --dataset feedback_loop_<C>_gptoss120b
   │     ▼  evaluation/mutation_oracle_cbmc_feedback_loop_<C>.json   (Sil/GT)
   │
   └─ cross_verify.py --condition <C> --all --save-json
         ▼  evaluation/cross_verify_results_cond<C>.json            (assertion recall)
                                              │
                                              ▼
                              scripts/paper_numbers.py   ← audits all of the above
```

### Stage scripts

| Stage | Script | Produces |
|-------|--------|----------|
| 1. Generate harnesses | `scripts/feedback_loop.py --condition <C> --model openrouter` | `results/feedback_loop_<C>_gptoss120b/` |
| 2. Mutation oracle (Sil/GT) | `scripts/run_mutation_oracle_cbmc.py --dataset feedback_loop_<C>_gptoss120b` | `evaluation/mutation_oracle_cbmc_feedback_loop_<C>.json` |
| 3. Cross-verify (assertion recall) | `scripts/cross_verify.py --condition <C> --all --save-json --workers <≤cores-1>` | `evaluation/cross_verify_results_cond<C>.json` |
| 4. Audit all numbers | `scripts/paper_numbers.py` | stdout OK/MISMATCH |
| 5. Figures | `figures/*.py` (e.g. `multi_oracle.py`, `llm4harness_pivot.py`) | `figures/*.pdf` |

Mutant generation (one-time, prerequisite for stage 2): `scripts/gen_mutants.py`
→ `mutants/<func>/mutant_*.c` + `metadata.json`.

---

## Conditions (the 8 main RQ1 conditions)

All share `dataset_condA`; they differ only in prompt / repair strategy.

| C | Definition |
|---|------------|
| **A** | full feedback loop (CBMC-guided repair) — primary |
| **G** | single-pass baseline: no CBMC feedback |
| **H** | strategy-neutral: repair prompt gives no deletion instruction |
| **I** | A + GT assertion-category label injected at each SAT failure |
| **J** | A + running deletion log shown to the model |
| **K** | spec-first: NL contract before the CBMC loop (`prompt_condK.txt`) |
| **M** | minimal CBMC guidance: explicit scalar-bounding instruction |
| **Oracle** | GT `__CPROVER_assume` preconditions pre-loaded |

`SF` (scaffolding-first, two-phase) exists in `feedback_loop.py` as an audit target
but is **not** a reported main condition.

---

## The three "recall" metrics (do not conflate)

These are distinct pipelines; the paper's ρ = −0.26 relates the **first two**:

1. **pass%** — `paper_numbers.passrate(C)`: final-iter `SUCCESS`/`UNSAT` over
   summary functions, from `results/.../summary.json`.
2. **assertion recall** — `paper_numbers.arec(C)`: mean Jaccard `harness_recall`
   from `cross_verify_results_cond<C>.json` (this is ρ's y-axis).
3. **oracle Sil/GT** — `paper_numbers.orecall/silgt(C)`: mutation silencing from
   `mutation_oracle_cbmc_feedback_loop_<C>.json`.

**ρ = Spearman(pass%, assertion-recall)** over the 8 conditions ≈ **−0.26**.

---

## Pinned reproduction (`*_pin`)

To re-run a condition deterministically at low concurrency, append `_pin`:
`cross_verify.py --condition A_pin ...` reads `results/feedback_loop_A_gptoss120b_pin/`
and writes `evaluation/cross_verify_results_condA_pin.json`. The 8 pinned
cross-verify outputs (A G H I J K M Oracle) were validated 2026-06-16
(n=83, gt>0=82, valid `harness_recall`).

---

## Cross-engine corroboration (CBMC vs ESBMC): per-engine breakdown

This section expands the paper's "Cross-engine corroboration" paragraph, which
points here for the assertion-mapping details and per-engine breakdown. The
underlying counts are registered in `paper_numbers.py` under group `S8/vi`.

The oracle's own comparable set is already smaller than the full corpus before
a second engine enters the picture: only 41 of the 83 functions have a runnable
expert-and-LLM harness pair for CBMC. Porting the same expert harnesses to
ESBMC narrows the set further, to 35, because on the remaining six functions
ESBMC's own expert harness does not reach a decisive verdict at all, independent
of any mutant. One harness fails to parse under ESBMC's front end, one exceeds
the verification budget without resolving, one is rejected by a stricter
front-end check that CBMC's parser tolerates, and the rest belong to the same
function family as the third. This narrowing reflects what each engine's front
end and resource budget can process, not a completeness gap on the functions
each one can actually decide.

Within that 35-function set, the two engines still disagree on a minority of
individual mutants, each missing some that the other catches. Tracing this
disagreement down to its source points to three distinct and, in kind,
symmetric causes, none of which favours one engine as generally more capable.
Some of the disagreement is a front-end effect: the mutation tool occasionally
produces a mutant that changes a value's type in a way one parser tolerates and
the other rejects outright. Some of it traces to the library-level scaffolding
each harness depends on, where a safety property encoded in the expert proof's
support code was not carried over faithfully when that scaffolding was adapted
to the second engine. The remainder comes from a genuine difference in how the
two engines represent pointers internally, which surfaces only on a comparison
between pointers to unrelated objects, a construct whose behaviour C itself
leaves unspecified.

None of this disturbs the paper's central claim. On the mutants both engines
can adjudicate, their silencing verdicts still agree exactly, twelve silenced
by both and none disputed. The disagreement is confined to the margin of what
each engine can decide, not to the silencing certificates themselves.

---

## Canonical data location

**This local repo is authoritative.** The server `/root/experiment_aws_cbmc` is
compute scratch and has diverged before — always pull validated outputs back here.
See `ENVIRONMENT.md` → "Canonical data vs compute scratch".

## Archived / superseded scripts

One-off and superseded scripts live in `scripts/archive/` (nothing imports them).
Do not run them for reproduction.
