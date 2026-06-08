# Do LLMs Verify or Conform?
## A Formal Study of Harness Quality in LLM-Assisted BMC
### Research Tracker — Weiqi Wang, University of Manchester

---

## Project Overview

**Core Claim**
> We operationalize CBMC's UNSAT signal as a reward channel and test for behaviour consistent with reward hacking: LLMs remove or weaken correct postcondition assertions to achieve UNSAT rather than to specify function behaviour. We provide the first per-assertion-attributable characterisation of this specification-degrading behaviour, linking each sacrificed assertion to a verifier feedback event and, via mutation testing, to a concrete safety consequence.

**Target Venue:** FSE 2027 (deadline ~October 2026; fallback: ASE 2027)

**Submission Target:** 2026/10

**Corpus**
- aws-c-common: 83 functions, 7 data structure families
- s2n-tls: 25 functions (s2n_stuffer module)
- 238 AWS engineer harnesses (ground truth H_GT)
- Primary LLM: 120B open-source × 11 conditions (A–H core + I/J/K ablations + Oracle Setup)
- Replication LLM: Meta Llama 3.3 70B Instruct (`meta-llama/llama-3.3-70b-instruct` via OpenRouter) × 3 conditions (A, G, H) ✅ COMPLETE

**Core Premise:** AWS code is correct and verified; H_GT is the strongest expert oracle; we do not assume H_GT is always stronger than H_LLM.

---

## Progress Snapshot — 2026-06-06

| Component | Status | Key number |
|-----------|--------|-----------|
| A/G/H/gptoss120b | ✅ Complete | PASS: 62.5/31.3/62.7%; Sac: 81.8/—/86.3% |
| A/G/H/Llama3370b | ✅ Complete | PASS: 61.4/31.3/45.8%; Sac: 71.7/—/64.1% |
| I/gptoss120b | ✅ Complete | PASS: 70.5%; Sac: **92.7%** — conformance pressure confirmed |
| J/gptoss120b | ✅ Complete | PASS: 67.5%; Sac: **93.0%** — forgetting ruled out |
| K/gptoss120b | ✅ Complete | PASS: 81.9%; Sac: **25.0%**; Recall: **0.268** (over-constrained!) |
| Oracle/gptoss120b | ✅ Complete | PASS: 84.3%; Sac: **11.8%**; Recall: **0.251** (laziness effect!) |
| M/gptoss120b (NEW) | ✅ Complete | PASS: 75.3%; Sac: **0.0%**; Recall: **0.384** (only condition improving both) |
| Cross-verify K/Oracle/M | ✅ Complete | **PASS rate ordering reversed by recall: M>A>K>Oracle** |
| Deletion_scope metric | ✅ Computed | A: mean 6.7 (72% panic); K: mean 1.6 (80% targeted) |
| Vacuity check script | ✅ Written + running | `scripts/vacuity_check.py` — results overnight |
| Mutant corpus | ✅ Complete | 2584 mutants, 61/83 functions |
| ESBMC runner | ✅ Written | Parity check pending |
| Annotation materials | ✅ Ready for pilot | 101 assertions, codebook fixed |
| κ pilot | ⏳ Human bottleneck | Need second annotator |
| RQ2 batch | ❌ Not started | Blocked on parity check |
| RQ3 pipeline | ❌ Not started | Blocked on RQ2 |
| Writing | ❌ Not started | Target: Sep 2026 |

**Critical path:** κ gate (second annotator) → full annotation → RQ1 final numbers
**Parallel track:** Vacuity check → ESBMC parity → RQ2 batch

**Major finding (2026-06-06):** PASS rate is a misleading metric. Cross-verify recall ordering (M>A>K>Oracle) contradicts PASS rate ordering (Oracle>K>M>A). K and Oracle inflate PASS via over-constraining assumes (31%, 24%). M is the only ablation improving both PASS and recall.

---

## Three Research Questions

| RQ | Question | Method | Status |
|----|----------|--------|--------|
| RQ1 | What properties do LLM-generated harnesses systematically miss, and are omissions knowledge gaps (never generated) or active sacrifices (generated then removed under CBMC pressure)? | H_LLM vs H_GT assertion comparison; three-state outcome; iteration logger; taxonomy κ gate | **Core results in hand**; K/Oracle pending; κ pilot pending |
| RQ2 | Do the specification gaps correspond to real verification failures on functionally incorrect code? (Primary: GT SAT / LLM UNSAT silenced-mutant count. Secondary: GT UNSAT / LLM SAT cases.) | 2584 mutants; dual ESBMC oracle; assertion-level sacrifice attribution | **Not started** — blocked on ESBMC parity check |
| RQ3 | Does the feedback protocol — not model capability — determine whether bugs are silenced, and does this vary per assertion category? | Three arms: A (delete/weaken) / B-strict (refine only) / B-relaxed (refine + assume); per-category SR and PR | **Not started** — blocked on RQ2 subject selection |

---

## Design Overview

### RQ2 Bidirectional CEX Confirm — Four Outcomes

| R_GT | R_LLM | Meaning | Action |
|------|-------|---------|--------|
| SAT | SAT | Both detect the mutant | Record, count as both-kill |
| **SAT** | **UNSAT** | **H_GT confirms bug; H_LLM silences it** | **Primary finding** — classify CEX, link to sacrifice log |
| UNSAT | SAT | H_LLM detects a deviation H_GT misses | Secondary finding — report characteristics and triggered assertions |
| UNSAT | UNSAT | Both miss | Discard; count toward lower-bound caveat |

### RQ3 Design: Three-Arm Pipeline (A / B-strict / B-relaxed)

All arms receive the same mutant function (presented as correct code); LLM is unaware it is a mutant. Same LLM used throughout — feedback protocol is the sole independent variable.

**Pipeline A — Delete/Weaken:** On SAT, LLM may remove *or weaken* the violated assertion. `__CPROVER_assume` additions prohibited as a response to SAT (only permitted during the initial setup phase). Replicates default behaviour; expected to silence bugs.

**Pipeline B-strict — Predicate Refinement Only (primary comparator):** On SAT, LLM must refine the assertion predicate to be more precise. Input domain frozen after setup: no `__CPROVER_assume` may be added or modified post-setup. No deletion permitted.

**Pipeline B-relaxed — Refine + Justified Assumes (secondary arm):** Like B-strict but LLM may additionally tighten `__CPROVER_assume` with explicit written justification. Quantifies how much of B-strict's difficulty is assume-based vs assertion-based.

**Key gaps measured:**
- SR_A − SR_{B-strict}: causal effect of deletion prohibition
- SR_{B-strict} − SR_{B-relaxed}: marginal effect of assume-based escape
- PR_A − PR_{B-strict}: pass-rate cost of enforcing predicate-only refinement

**Confirmation (after all pipelines):** Run CBMC(H_buggy_P, f_original) for P ∈ {A, B-strict, B-relaxed}. SAT + concrete execution divergence (f_orig(I) ≠ f_buggy(I)) confirms real bug detection; UNSAT means bug silenced.

**Subject selection:** Stratified subset of RQ2 GT SAT / LLM UNSAT cases, balanced across three taxonomy categories (~25–30 per category). RQ1 category labels reused directly. H_GT never shown to LLM.

**Primary metrics per pipeline P and category c:**
- Silencing rate SR_{P,c}: proportion where CBMC(H_buggy_P, f_original) = UNSAT
- Pass rate PR_{P,c}: proportion where pipeline achieves UNSAT on mutant within budget

The joint (SR, PR) per category across three arms is the primary result table.

### Taxonomy (from RQ1)

| Category | Description | Miss Rate |
|----------|-------------|-----------|
| validity predicate | pointer validity, return value range, error code checks | 20% |
| length invariant | length, capacity, offset relationships | 19% |
| frame condition | assigns clause, memory modification scope | 17% |

---

## TODO List

### Phase 1: Literature Review
**Goal:** Confirm novelty position, complete related work draft
**Duration:** 2 weeks (2026/06/02 – 2026/06/15)
**Criteria:** 7 competitor papers read in depth + 4 foundational papers scanned; 2–3 page related work draft; one-sentence differentiator from BMC-Agent / SpecMind / NL2Contract / HarnessAgent

#### Reading Strategy

Priority logic: read competitors first to establish novelty boundary, then foundations to justify design choices. Papers marked ⚠️ need arxiv ID verified at arxiv.org *before* reading.

**Week 1 schedule (competitors):**
- Day 1–2: BMC-Agent — highest-priority novelty threat (Kroening + Amazon, CBMC + agents, soundness guard)
- Day 3: SpecMind — architecturally closest to the iterative feedback loop
- Day 4–5: NL2Contract + HarnessAgent — define the broader competitive landscape

**Week 2 schedule (remaining papers + draft):**
- Day 1–2: AssertLLM + Clover (verify IDs first)
- Day 3: Papadakis mutation survey (mutation oracle justification)
- Day 4–5: Write related work draft (4-group structure)

#### Core Paper List — Competitors (deep read)

- [ ] **BMC-Agent / Agentic Model Checking** (arxiv 2605.21434) — PRIORITY 1 ⚠️ highest novelty risk
  - What it does: LLM agents + CBMC/Kani backend under "agents propose, solvers verify"; top-down spec generation; 4-stage validation pipeline (reachability, callee feasibility, dynamic replay, realism audit); adaptive refinement loop with **soundness guard** that rejects refinements masking real bugs. Daniel Kroening (CBMC author) + Amazon co-authors.
  - How it differs from our work: BMC-Agent is a *tool* that prevents sacrifice via a soundness guard; we are an *empirical study* that quantifies what sacrifice costs (per-category) and establishes the safety consequences via mutation oracle. BMC-Agent has no expert GT harness corpus, no active-sacrifice fraction metric, no controlled protocol experiment (RQ3). The soundness guard is essentially an engineering implementation of Pipeline B-strict — our paper provides the empirical motivation for why such a guard is necessary.
  - One-sentence differentiator: *BMC-Agent engineers a solution to specification erosion; we provide the first empirical characterisation of its magnitude, per-category structure, and safety cost that motivates why such a solution is needed.*

- [ ] **SpecMind** (arxiv 2602.20610) — PRIORITY 2
  - What it does:
  - How it differs from our work:
  - Key question to answer: does it distinguish spec completeness from verifier pass rate?

- [ ] **NL2Contract** (arxiv 2510.12702) — PRIORITY 3
  - What it does:
  - How it differs from our work:
  - Key question to answer: iterative verifier feedback? mutation oracle for validation?

- [ ] **HarnessAgent** (arxiv 2512.03420) — PRIORITY 4
  - What it does:
  - How it differs from our work:
  - Key question to answer: BMC or fuzzing? per-assertion completeness analysis?

- [ ] **AssertLLM** — PRIORITY 5 ⚠️ verify full citation before reading
  - What it does:
  - How it differs from our work:
  - Key question to answer: does it use mutation testing to evaluate assertion quality?

- [ ] **Clover** — PRIORITY 6 ⚠️ verify full citation before reading
  - What it does:
  - How it differs from our work:

- [ ] **Re:Form** ⚠️ arxiv ID unknown — search "Re:Form formal verification LLM 2025" first
  - What it does:
  - How it differs from our work:

- [ ] **Contract Strengthening via CHC** (arxiv 2211.12228)
  - What it does:
  - How it differs from our work:

#### Core Paper List — Foundations (scan + targeted read)

- [ ] **Chong et al. (2021)** — *Code-level model checking in the software development workflow at Amazon Web Services*, SPE 2021
  - Why: institutional basis for treating H_GT as "safety-relevant expert practice"; explains harness anatomy, assumes usage, CI integration
  - Read: Section 4 (harness structure) in depth; rest at scan pace

- [ ] **CBMC tool paper** (arxiv 2302.02384) — Kroening et al. 2023
  - Why: tool foundation; DFCC mechanism is what makes frame conditions a first-class verification object
  - Read: DFCC / assigns clause section in depth

- [ ] **Papadakis et al. (2019)** — *Mutation Testing Advances: An Analysis and Survey*, Advances in Computers
  - Why: theoretical basis for "silenced-mutant count" as spec adequacy metric; establishes mutation oracle as circularity-free validation
  - Read: Sections 2–3 (adequacy criteria and mutation score)

- [ ] **Groce et al. (2018)** — *universalmutator*, ICSE 2018 tool paper
  - Why: justifies mutation tool choice; documents operator coverage
  - Scan only. Source: agroce.github.io/icse18t.pdf

#### Related Work Grouping Structure

- [ ] **Group 1: LLM spec / contract synthesis**
  - Papers: NL2Contract, SpecMind, Contract Strengthening via CHC
  - Contrast point: these synthesise specs but do not track per-assertion sacrifice under verifier pressure; no mutation oracle for validation

- [ ] **Group 2: LLM harness generation**
  - Papers: HarnessAgent, [search for others]
  - Contrast point: harness generation for fuzzing ≠ BMC; no formal spec completeness metric; no iterative CBMC feedback

- [ ] **Group 3: Mutation testing as spec oracle**
  - Papers: AssertLLM, Papadakis survey, [search for others]
  - Contrast point: mutation testing evaluates spec quality, but not combined with LLM iterative generation and sacrifice attribution

- [ ] **Group 4: LLM behaviour under iterative feedback**
  - Papers: Re:Form, [search for Reflexion / self-debug / SpecLoop]
  - Contrast point: these study LLM self-correction capability; we study the specific conformance-vs-specification trade-off that emerges in formal verification loops

- [ ] **Group 5: Agentic verification systems** ← new group, added 2026/06/02
  - Papers: BMC-Agent (2605.21434)
  - Contrast point: BMC-Agent engineers a soundness guard to *prevent* specification erosion; our study *quantifies* its magnitude and per-category safety cost, providing the empirical motivation for why such a guard is necessary. BMC-Agent has no GT harness corpus, no sacrifice-attribution metric, no mutation oracle validation, no controlled protocol experiment.

#### Outputs

- [ ] Related work draft (2–3 pages, 5-group structure above)
- [ ] Novelty positioning map (X-axis: formal backing strength, Y-axis: industrial code scale)
- [ ] One-sentence differentiators filled in:
  - Distinction from BMC-Agent: ___
  - Distinction from SpecMind: ___
  - Distinction from NL2Contract: ___
  - Distinction from HarnessAgent: ___

---

### Phase 2: RQ1 Replication + Active Sacrifice Annotation
**Goal:** Complete third-party taxonomy replication; annotate active-sacrifice vs knowledge-gap; finalise RQ1 results
**Duration:** 2–3 weeks
**Criteria:** Two independent κ gates both ≥ 0.8 before full annotation proceeds

#### Prerequisite Check

- [x] Confirm iteration logger is operational for aws-c-common corpus — 1433 iteration_log.json files written across 18 conditions including G/H/gptoss120b (2026-06-04)
- [x] Cross-verification recall computed for A/G/H/gptoss120b: G=0.290, A=0.357, H=0.303 (2026-06-04)
- [x] RQ1 analysis script (analyze_rq1.py) operational — 87.2% sacrifice ratio confirmed across all conditions
- [x] DeepSeek V4 Flash replaced with Meta Llama 3.3 70B as replication model (proof_allocators.h hallucination, 97-100% UNKNOWN)
- [x] Prompt guard added against hallucinated includes (prompt_condA.txt + feedback_loop.py)
- [x] Mutation pipeline (gen_mutants.py) built and validated: 46 compiled mutants from aws_byte_buf_init (47% pass rate), ~3818 extrapolated for 83 funcs
- [ ] Confirm iteration logger operational for s2n-tls corpus (blocked: s2n LLM generation not yet run)
- [ ] Confirm s2n-tls LLM generation data is available for all core conditions
- [x] Run Llama 3.3 70B replication (A/G/H) — COMPLETE (2026-06-04): A=83/83, G=83/83, H=83/83; cross-verify done; A/G replicate gptoss (Δ≤1.1pp PASS); H shows 17pp gap (model-specific sensitivity to strategy-neutral prompt)

#### Ablation Conditions (I/J/K/Oracle Setup) — scripting + server run

- [x] **Write I/J/K/Oracle Setup prompt variants in feedback_loop.py** ← DONE (2026-06-04)
- [x] **Write run_ablations.sh** for server sequential execution ← DONE
- [x] **Run I/J conditions on server** ← COMPLETE (83/83 each)
  - I: 70.5% PASS (+8pp vs A), sacrifice ratio 92.7% — category label INCREASES sacrifice → conformance pressure confirmed
  - J: 67.5% PASS (+5pp vs A), sacrifice ratio 93.0% — deletion log has no effect → statelessness ruled out
- [x] **K/Oracle pipeline bugs diagnosed and fixed** (2026-06-05):
  - Bug 1: `extract_c_code()` only stripped fences at response start — K produces markdown+C responses; fixed to search for fence anywhere in response
  - Bug 2: `compilation_ok` checked stdout only; CBMC sends "PARSING ERROR" and "Invalid User Input" to stderr — fixed to check both streams
  - Bug 3: LLM generated `int main()` entry point; CBMC called with `--function {func}_harness` — fixed with `normalize_entry_point()` post-processing
  - All fixes in `feedback_loop.py` + `cbmc_runner.py`; synced to server
- [x] **K/Oracle re-run completed** (2026-06-06): K=83/83 (81.9% PASS), Oracle=83/83 (84.3% PASS)
- [x] **Condition M designed and completed** (2026-06-06): M=81/83 (75.3% PASS); bounding hint eliminates UNKNOWN-triggered sacrifice entirely (0.0% ratio)
- [x] **Cross-verify K/Oracle/M complete** (2026-06-06): Recall M=0.384 > A=0.346 > K=0.268 > Oracle=0.251
- [x] **Plan 2: deletion_scope computed** (2026-06-06):
  - A: mean 6.7 (72% panic, max single-event=20 deletions)
  - I: mean 5.5 (71% panic — category label does NOT convert panic→targeted)
  - K: mean 1.6 (80% targeted — NL contract enables precise deletion)
  - Result: `scripts/vacuity_check.py` written; running overnight on K/Oracle/M SUCCESS harnesses
- [ ] **Read vacuity check results** — `cat /tmp/vacuity.log` on server next session
- [ ] Sync all results locally from server
- [ ] Update analyze_rq1.py to include K/Oracle/M in full table

#### Annotation Task — Redesigned (2026-06-05)

**Design decision:** Taxonomy κ and attribution validation are now separate tasks.
- **κ gate:** taxonomy classification only (validity_predicate / length_invariant / frame_condition). No LLM output shown. Genuine judgment task.
- **Attribution validation:** automated log-matching + 5-case human audit. Precision estimate, not κ.

**Key codebook rule (2026-06-05):** frame_condition requires comparison to old_X snapshot AND delta=0. null/constant/input_param comparisons → validity_predicate. old_X with non-zero delta → length_invariant. 20/101 auto-classifier errors corrected using this rule.

- [x] **Build scripts/annotate_helper.py** ← DONE (2026-06-03)
- [x] **Run pre-annotation on all 83 functions** — 198 missed GT assertions; 97% never_generated, 2.5% deleted_sacrifice (2026-06-04)
- [x] **Generate pilot annotation materials** (2026-06-05):
  - `annotation/pilot_worksheet.csv` — 101 assertions, 30 functions, corrected suggestions
  - `annotation/pilot_blank.csv` — for second annotator (no suggestions)
  - `annotation/ANNOTATION_GUIDE.md` — English, with precise frame_condition rule + counter-intuitive examples
  - `annotation/attribution_audit.csv` — 5 sacrifice cases with iter_1/final evidence chain
  - `annotation/compute_kappa_pilot.py` — taxonomy κ calculator
  - `annotation/pilot_worksheet_annotated.csv` — Claude's full reference annotation with reasoning
- [x] **Auto-classifier bug fixed** — 20/101 wrong suggestions corrected (9 frame→validity, 4 length→validity, 1 frame→length, 6 more on second pass)
- [ ] **Weiqi annotates pilot** — fill `your_taxonomy` in `pilot_worksheet_annotated.csv` (~60-90 min)
- [ ] **Recruit second annotator** — lab colleague or supervisor; share `pilot_blank.csv` + `ANNOTATION_GUIDE.md`; 60-90 min
- [ ] **Run compute_kappa_pilot.py** — compute taxonomy κ; target ≥ 0.8
- [ ] κ ≥ 0.8 → full annotation on all 198 assertions; κ < 0.8 → review guide disagreements, re-pilot
- [ ] **Attribution audit** — confirm/dispute 5 sacrifice cases in `attribution_audit.csv`; report precision

#### Outputs

- [ ] κ₁ (taxonomy): ___ ; κ₂ (sacrifice attribution): ___
- [ ] Final taxonomy + sacrifice-attribution codebook
- [ ] RQ1 final numbers:
  - pass rate: ___
  - recall: ___
  - validity miss (knowledge-gap / sacrifice / weakened): ___
  - length miss (knowledge-gap / sacrifice / weakened): ___
  - frame miss (knowledge-gap / sacrifice / weakened): ___
  - active-sacrifice fraction per category (denominator: ever-generated H_GT-entailed): ___
  - same-family ∆pp: ___
  - lib-specific ∆pp: ___

---

### Phase 3: Mutant Generation + Compilation Filtering
**Goal:** Build the compilation-passing mutant set M_all
**Duration:** 2 weeks
**Criteria:** ~1,900 compilation-passing mutants; at least 10 per function

**Tool:** universalmutator (Groce et al. 2018) — github.com/agroce/universalmutator

#### Environment Setup

- [x] Install universalmutator — `mutate` command available
- [x] Configure CBMC-aware compilation filter (CBMC macros incompatible with gcc; using CBMC compilation check)
- [x] Write compilation filter script (gen_mutants.py)
- [x] Write mutant metadata recording script — gen_mutants.py records operator type, line range, source function
- [x] Define equivalent-mutant exclusion strategy — TCE filter (compiled object identity) applied at generation time

#### Trial Run

- [x] Trial run on aws_byte_buf_init: 98 raw → 46 compiled (47% pass rate, 16-line function)
- [x] Inspect generated mutant quality — reasonable operator distribution
- [x] Compilation filter verified correct (CBMC not gcc)

#### Full Run

- [x] **Run gen_mutants.py --all on all 83 aws-c-common functions** ← DONE (2026-06-04)
  - Result: **2584 compiled mutants across 61/83 functions** (avg 29.7/function)
  - 7 zero-mutant functions: all use AWS_STATIC_IMPL in .inl files — CBMC cannot compile .inl in isolation
  - 19 low-mutant functions (<10): acceptable, included in RQ2
  - Distribution: 0 mutants (7), 1-9 (15), 10-29 (26), 30+ (35)
- [x] TCE filter applied at generation time
- [x] FUNC_SOURCE_OVERRIDES added for .inl functions (but CBMC limitation accepted)
- [ ] Extend to s2n-tls 25 functions after s2n LLM generation is complete

#### Outputs

- [x] M_all size: **2584 mutants** (61 functions with ≥1 mutant)
- [x] Avg mutants per covered function: **29.7**
- [ ] Mutant operator type distribution (pending)
- **Limitation (Threats to Validity):** 7 inline functions (AWS_STATIC_IMPL in .inl) produce 0 mutants — CBMC compilation limitation. Reported as named limitation; 61/83 functions (73%) have adequate coverage.

---

### Phase 4: Bidirectional CEX Confirm Infrastructure
**Goal:** Build a reliable batch-run and auto-classification pipeline
**Duration:** 2 weeks
**Criteria:** 100% accuracy on manual validation of 10 samples; total runtime estimate is reasonable

#### CBMC-to-ESBMC Migration Validity Check (prerequisite for batch)

- [ ] UNSAT preservation check: ESBMC(H_GT, f) = UNSAT for all functions where CBMC(H_GT, f) = UNSAT
- [ ] SAT-SAT agreement: randomly sample 30 mutants where CBMC(H_GT, m) = SAT; confirm ESBMC(H_GT, m) = SAT for ≥ 27/30 (90%)
- [ ] Document any incompatible cases; retain those under CBMC only and report separately

#### ESBMC Batch Run Script

- [x] **Write scripts/esbmc_runner.py** ← DONE (2026-06-04)
  - Implements `MutantOracleResult`, `run_rq2_batch()`, `run_parity_check()`
  - Four-outcome classification: GT_SAT_LLM_UNSAT (primary), GT_SAT_LLM_SAT, GT_UNSAT_LLM_SAT, GT_UNSAT_LLM_UNSAT
  - Timeout: 300s per run; multiprocessing with configurable workers
- [ ] **Run ESBMC parity check** ← NEXT IMMEDIATE TASK (can run now, parallel with K/Oracle analysis)
  - Step 1: ESBMC(H_GT, f_original) = UNSAT for all 83 functions
  - Step 2: 30 known CBMC-SAT mutants → ESBMC also SAT (≥90%)
  - If fails: adjust ESBMC flags or exclude incompatible functions
- [ ] Install/verify ESBMC locally (check version compatibility with CBMC 6.8.0 harnesses)

#### CEX Assertion Auto-Classification Script

- [ ] Parse ESBMC XML/JSON output; extract triggered assert location + text
- [ ] Map assertion text to taxonomy category (validity/length/frame) using keyword heuristics
- [ ] Handle unclassifiable cases (report separately)

#### Four-Outcome Classification Script

- [ ] Input: R_GT and R_LLM per mutant
- [ ] Output: GT_SAT/LLM_UNSAT (primary), GT_SAT/LLM_SAT, GT_UNSAT/LLM_SAT, GT_UNSAT/LLM_UNSAT
- [ ] Results DB: JSON per function, CSV aggregate

#### Validation

- [ ] End-to-end trial run on 10 functions
- [ ] Manually verify classification results
- [ ] Accuracy: ___% (target > 90%)
- [ ] Total runtime estimate: ___ hours

---

### Phase 5: Bidirectional CEX Confirm Batch Run
**Goal:** Build M_confirmed set and complete four-outcome classification
**Duration:** 4 weeks
**Criteria:** M_confirmed > 400; all four outcomes have samples; CEX classification accuracy > 90%

#### Batch Run

- [ ] ESBMC(H_GT, m) for all ~1,900 mutants (single run; shared oracle across conditions)
- [ ] ESBMC(H_LLM, m) for all ~1,900 mutants
  - [ ] Primary LLM (120B) × conditions A, B, D, E, F, G, H (core) + I, J, K (ablations) + Oracle Setup
  - [ ] Replication LLM (Llama 3.3 70B) × conditions A, G, H ← data collected ✅; ESBMC run pending
- [ ] Soundness-parity validation before batch: UNSAT preservation check on H_GT + SAT-SAT agreement ≥ 90% on 30 known-SAT mutants

#### Result Classification

- [ ] Four-outcome classification completed
- [ ] CEX assertion auto-classification completed
- [ ] Manual verification of 50 sampled results (accuracy > 90%)

#### Quality Checks

- [ ] Check whether any function has zero confirmed mutants
- [ ] Check whether UNSAT/SAT outcomes exist (critical for the finding)

#### Outputs

- [ ] Four-outcome distribution:
  - SAT/SAT: ___ (___ %)
  - SAT/UNSAT: ___ (___ %)
  - UNSAT/SAT: ___ (___ %)
  - UNSAT/UNSAT: ___ (___ %)
- [ ] M_confirmed size: ___ mutants
- [ ] Preliminary per-category distribution:
  - validity: ___
  - length: ___
  - frame: ___
- [ ] UNSAT/SAT exists: ___
- [ ] Decision point: M_confirmed < 400 → generate additional mutants before proceeding

---

### Phase 6: RQ2 Analysis
**Goal:** Complete all RQ2 statistical analysis and confirm kill rate gap
**Duration:** 3 weeks
**Criteria:** Kill rate gap is statistically significant (p < 0.05); ranking consistent with recall

#### Primary Result — Silenced-Mutant Count (GT SAT / LLM UNSAT)

- [ ] Per-category count of GT SAT / LLM UNSAT cases with concrete CEX confirmation (f_orig(I) ≠ m(I))
- [ ] Count of distinct H_GT assertions certified by ≥1 silenced mutant (vs |H_GT| total)
- [ ] Statistical significance test (Fisher's exact) per category

#### Assertion-Level Sacrifice Attribution

- [ ] For each GT SAT / LLM UNSAT case: extract triggered H_GT assertion; match against RQ1 sacrifice log
- [ ] Classify as sacrifice-attributed (assertion was generated then removed) vs knowledge-gap (never generated)
- [ ] Per-category breakdown: sacrifice-attributed kills vs knowledge-gap kills

#### E4 — Cross-Oracle Transfer Check (secondary)

- [ ] For Pipeline A harnesses (RQ3 subjects): re-run on ESBMC with same tool flags
- [ ] If ESBMC also returns UNSAT → hack is semantic (genuine spec erosion)
- [ ] If ESBMC returns SAT → tool-specific artefact; flag case
- [ ] Report: fraction confirmed semantic vs tool-specific per category

#### Secondary Finding — GT UNSAT / LLM SAT Cases

- [ ] Count and characterise functions where H_LLM detects a deviation H_GT misses
- [ ] Categories of triggered H_LLM assertions; check if entailed by documented contract
- [ ] Note: this is a secondary independent finding, not the primary analysis

#### Prompt Condition Stratification

- [ ] Silenced-mutant count per prompt condition (all 11)
- [ ] Does same-family context reduce silencing?
- [ ] Consistency with RQ1 ∆recall

#### Figures

- [ ] Per-category silenced-mutant count chart (primary result)
- [ ] Sacrifice-attributed vs knowledge-gap kill breakdown per category
- [ ] GT UNSAT / LLM SAT secondary finding chart

#### Outputs

- [ ] Primary result table (per category):

  |  | Silenced mutants (GT SAT/LLM UNSAT) | Sacrifice-attributed | Knowledge-gap | p-value |
  |--|-------------------------------------|---------------------|---------------|---------|
  | validity | | | | |
  | length | | | | |
  | frame | | | | |

- [ ] E4 transfer check: ___ % semantic, ___ % tool-specific
- [ ] GT UNSAT / LLM SAT secondary finding: ___
- [ ] RQ2 conclusion draft
- [ ] Decision point: p > 0.05 → revisit design before proceeding to RQ3

---

### Phase 7: RQ3 Experiment Run
**Goal:** Run three-arm pipeline (A / B-strict / B-relaxed) on stratified subset; obtain per-category SR and PR
**Duration:** 3 weeks
**Criteria:** ≥22 subjects per category per arm (power = 0.80 at α = 0.05 for SR gap ≥ 0.35); pipeline reaches UNSAT for > 70% of subjects in at least one arm

#### Subject Selection

- [ ] From RQ2 GT SAT / LLM UNSAT cases, stratify by category
- [ ] Target: 25–30 per category (if any category < 22 confirmed bugs from RQ2, report that category's RQ3 as exploratory)
- [ ] Prepare mutant files and confirm f_original is available for each subject
- [ ] Confirm H_GT is withheld from LLM throughout

#### Pipeline A — Delete/Weaken

- [ ] Provide LLM with mutant function labelled as correct implementation
- [ ] Run CBMC feedback loop; on SAT LLM may delete OR weaken assertion; no new assumes permitted
- [ ] Record each deletion/weakening with iteration number and triggered assertion
- [ ] Iterate until UNSAT or budget exhausted
- [ ] Vacuity audit: call-site reachability check (insert assert(false) after setup; if UNSAT → vacuous, exclude)
- [ ] Pipeline A pass rate per category: ___

#### Pipeline B-strict — Predicate Refinement Only

- [ ] Same subjects; same LLM; provide full CEX on SAT (concrete input + violated assertion + category label)
- [ ] Instruct LLM: refine assertion predicate only; input domain frozen; no deletion; no new assumes
- [ ] Predicate-weakening detection: check assert(!P && Q) with GT preconditions; if SAT → Q ⊊ P, flag as weakening not refinement
- [ ] Iterate until UNSAT or budget exhausted
- [ ] Vacuity audit: same reachability check
- [ ] Pipeline B-strict pass rate per category: ___

#### Pipeline B-relaxed — Refine + Justified Assumes

- [ ] Same as B-strict but LLM may add __CPROVER_assume with explicit written justification
- [ ] Predicate-weakening detection: same as B-strict (GT precondition domain used, not arm's own)
- [ ] Vacuity audit: same reachability check + per-assume negation localiser for B-relaxed cases
- [ ] Pipeline B-relaxed pass rate per category: ___

#### Confirmation Step (after all three arms)

- [ ] Run CBMC(H_buggy_P, f_original) for P ∈ {A, B-strict, B-relaxed}
- [ ] For SAT cases: execute concrete CEX input; confirm f_original(I) ≠ f_buggy(I)
- [ ] UNSAT = bug silenced → counts toward SR_P

#### Outputs

- [ ] Per-category pass rate table:

  |  | Pipeline A PR | B-strict PR | B-relaxed PR |
  |--|---------------|-------------|--------------|
  | validity | | | |
  | length | | | |
  | frame | | | |

- [ ] Per-category silencing rate table:

  |  | Pipeline A SR | B-strict SR | B-relaxed SR | SR_A − SR_{B-strict} |
  |--|---------------|-------------|--------------|----------------------|
  | validity | | | | |
  | length | | | | |
  | frame | | | | |

- [ ] Preliminary narrative: which category is low-cost for refine (SR drops, PR stable), which forces tradeoff (SR improves but PR degrades)

---

### Phase 8: RQ3 Fine-Grained Analysis
**Goal:** Complete per-category SR/PR analysis, cross-RQ consistency check, determine final narrative
**Duration:** 3 weeks
**Criteria:** SR gap (A−B) is statistically significant in at least one category; cross-RQ consistency holds

#### Per-Category SR and PR Analysis

- [ ] SR_{A,c} vs SR_{B,c} for each category c; compute gap SR_A − SR_B per category
- [ ] PR_{A,c} vs PR_{B,c} for each category c; compute cost PR_A − PR_B per category
- [ ] Statistical significance test (Fisher's exact or chi-squared per category)
- [ ] Identify: which category is low-cost refine (PR barely changes, SR drops sharply)?
- [ ] Identify: which category forces tradeoff (SR improves but PR degrades)?

#### Assertion Deletion Log Analysis (Pipeline A)

- [ ] Distribution of deleted assertion categories across iterations
- [ ] Average iteration at which each category is first deleted
- [ ] Consistency with RQ1 active sacrifice distribution

#### Internal Consistency Check (Three-RQ Chain)

- [ ] High RQ1 miss rate → low RQ2 kill rate → high SR_A (bug silenced) per category?
  - validity: consistent / inconsistent
  - length: consistent / inconsistent
  - frame: consistent / inconsistent

#### Figures

- [ ] (SR, PR) scatter per category for both pipelines
- [ ] SR gap and PR gap bar chart per category
- [ ] Three-RQ consistency summary table

#### Outputs

- [ ] Final per-category joint (SR, PR) table
- [ ] RQ3 conclusion: frame condition is low-cost / validity forces tradeoff / (actual result)
- [ ] RQ3 conclusion draft

---

### Phase 9: Results Integration + Narrative
**Goal:** Confirm submission target and paper structure
**Duration:** 2 weeks
**Criteria:** Narrative can be expressed in three sentences; supervisor endorses it

#### Results Integration

- [ ] View all three RQ results together
- [ ] Check narrative for internal consistency
- [ ] Confirm wording of central claim

#### Venue Decision

- [ ] Discuss results with supervisor
- [ ] Primary: FSE 2027 (~October 2026 deadline)
- [ ] Fallback: ASE 2027 (if RQ3 not stable by Oct 2026)
- [ ] Decision confirmed: ___

#### Paper Structure

- [ ] Abstract (core claim + 3 findings)
- [ ] Introduction (motivation + RQs + contributions)
- [ ] Study Design (corpus + methodology)
- [ ] RQ1 Results
- [ ] RQ2 Results
- [ ] RQ3 Results
- [ ] Discussion (implications + comparison with NL2Contract / SpecMind)
- [ ] Threats to Validity
- [ ] Related Work
- [ ] Conclusion

#### Outputs

- [ ] Three-sentence narrative:
  1. RQ1: ___
  2. RQ2: ___
  3. RQ3: ___
- [ ] Submission target: ___
- [ ] Paper structure outline (key points per section)

---

### Phase 10: Writing
**Goal:** Complete first full paper draft
**Duration:** 6 weeks
**Criteria:** One section per week; prioritise completion over polish

| Section | Status | Notes |
|---------|--------|-------|
| Introduction | ☐ Not started | |
| Study Design | ☐ Not started | |
| RQ1 | ☐ Not started | Partial content available |
| RQ2 | ☐ Not started | |
| RQ3 | ☐ Not started | |
| Discussion | ☐ Not started | |
| Threats to Validity | ☐ Not started | |
| Related Work | ☐ Not started | Draft available from Phase 1 |
| Abstract | ☐ Not started | Write last |
| Conclusion | ☐ Not started | |

---

### Phase 11: Supervisor Review + Revision
**Goal:** Bring paper to submission standard
**Duration:** 3 weeks
**Criteria:** Supervisor endorses submission; all likely reviewer questions have answers

- [ ] Send complete paper to supervisor
- [ ] Receive feedback; compile revision list
- [ ] Revise in priority order
- [ ] Threats to validity are honest and complete
- [ ] Final check: formatting, references, figures
- [ ] Submission preparation

---

## Threats to Validity — Notes

**Internal Validity**
- UNSAT is not uniquely interpretable (H too weak, OR path not covered)
- All conclusions are scoped to mutants where at least one side is SAT
- Kill rate is a relative comparison, not an absolute value

**External Validity**
- Corpus is limited to aws-c-common and s2n-tls
- We make no claim of generalisation to all C code

**Construct Validity**
- Subjectivity in taxonomy classification (mitigated by inter-rater κ)
- Auto-classification of CEX-triggered assertions reduces manual judgement
- RQ3 binary judgement is a simplification (mitigated by fine-grained stratification)

**Conclusion Validity**
- Primary LLM × 11 conditions; replication LLM × 3 conditions
- Report per-condition and per-category results; do not report only aggregates
- A vs B-strict comparison controls for LLM capability by construction (same LLM, same mutant)

---

## Distinctions from Related Work

| Work | Approach | Corpus | Distinction from Our Work |
|------|----------|--------|---------------------------|
| NL2Contract | One-directional evaluation, discriminative power | Python/Java | No feedback loop, non-industrial C, no root cause analysis |
| SpecMind | Multi-turn feedback, soundness + completeness | Python | Feedback is not formal; no mutation testing |
| HarnessAgent | Fuzzing harness generation | Open-source libraries | Fuzzing ≠ BMC; no CEX confirmation |
| Our work | Bidirectional CEX confirm + bug injection | Industrial C (AWS) | Formal backing + root cause analysis + industrial scale |

---

## Progress Tracker

| Phase | Content | Status | Start | End |
|-------|---------|--------|-------|-----|
| 1 | Literature Review | 🔵 In progress | 2026/06/02 | |
| 2 | RQ1 Replication | 🔵 In progress | 2026/06/04 | I/J ✅; K/Oracle re-running; κ gate pending |
| 3 | Mutant Generation | 🔵 In progress | 2026/06/04 | 2584 mutants, 61/87 funcs ≥10; 7 funcs 0 (inline, limitation) |
| 4 | CEX Infrastructure | ☐ | | |
| 5 | CEX Batch Run | ☐ | | |
| 6 | RQ2 Analysis | ☐ | | |
| 7 | RQ3 Experiment Run | ☐ | | |
| 8 | RQ3 Fine-Grained Analysis | ☐ | | |
| 9 | Results Integration | ☐ | | |
| 10 | Writing | ☐ | | |
| 11 | Review + Revision | ☐ | | |
| 12 | Submission | ☐ | 2026/10 (FSE) / 2027 (ASE fallback) | |

---

## Key Decision Points

```
After mutant generation (Phase 3):
    M_all < 1,000 → tune operator parameters

After CEX confirmation (Phase 5):
    M_confirmed < 400 → generate more mutants
    UNSAT/SAT outcomes absent → investigate ESBMC configuration

After RQ2 (Phase 6):
    Kill rate gap p > 0.05 → revisit design

After RQ3 (Phase 8):
    SR/PR results stable by Oct 2026 → submit FSE 2027
    Results not stable by Oct 2026 → submit ASE 2027 (same scope, no reduction)
```

---

## Supervisor Meeting Template

**Structure for each meeting:**

```
1. What was completed last week (against the TODO list)
2. Key numbers / results
3. Issues encountered
4. Plan for next week
5. Items requiring supervisor decision
```

---

*Last updated: 2026/06/02 — Phase 1 rewrite: prioritised reading schedule, foundation papers added, uncertain arxiv IDs flagged, E1 probe clarified as separate subset, Phase 3 equivalent-mutant strategy added, Phase 4 CBMC-to-ESBMC migration check moved forward*
