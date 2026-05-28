# Do LLMs Verify or Conform?
## A Formal Study of Harness Quality in LLM-Assisted BMC
### Research Tracker — Weiqi Wang, University of Manchester

---

## Project Overview

**Core Claim**
> LLM-generated CBMC proof harnesses verify behavioural consistency of an implementation rather than the semantic specification of a function. We formally quantify this deviation using mutation testing with bidirectional CEX confirmation, and reveal its root cause through bug injection experiments.

**Target Venue:** FSE 2027 (deadline ~October 2026; fallback: ASE 2027)

**Submission Target:** 2026/10

**Corpus**
- aws-c-common: 83 functions, 7 data structure families
- s2n-tls: 25 functions (s2n_stuffer module)
- 238 AWS engineer harnesses (ground truth H_GT)
- 2 LLMs × 6 prompt conditions

**Core Premise:** AWS code is correct and verified; H_GT is the strongest expert oracle; we do not assume H_GT is always stronger than H_LLM.

---

## Three Research Questions

| RQ | Question | Method | Status |
|----|----------|--------|--------|
| RQ1 | What properties do LLM-generated harnesses systematically miss? | H_LLM vs H_GT assertion comparison, taxonomy classification | Core results in hand, replication needed |
| RQ2 | What is the gap between H_LLM and H_GT in detecting real bugs? Are there cases where H_LLM is stronger? | Broad mutant set + bidirectional CEX confirmation | Not started |
| RQ3 | Does the feedback strategy (delete vs refine) determine whether bugs are silenced or detected, and does this effect vary per assertion category? | Pipeline A vs B on stratified subset of RQ2 confirmed bugs; per-category SR and PR | Not started |

---

## Design Overview

### RQ2 Bidirectional CEX Confirm — Four Outcomes

| R_GT | R_LLM | Meaning | Action |
|------|-------|---------|--------|
| SAT | SAT | Both detect the mutant | Record, count as both-kill |
| SAT | UNSAT | H_LLM is weaker | **Primary analysis target**, classify CEX |
| UNSAT | SAT | H_LLM is stronger | **Most interesting**, classify CEX |
| UNSAT | UNSAT | Both miss | Discard |

### RQ3 Design: Pipeline A vs Pipeline B

Both pipelines receive the same mutant function (presented as correct code); LLM is unaware it is a mutant.

**Pipeline A — Delete:** On SAT, LLM may remove the violated assertion. Iterates to UNSAT. Replicates default behaviour; expected to silence bugs.

**Pipeline B — Refine:** On SAT, LLM receives full CEX (concrete input + violated assertion + category) and must refine the assertion without deleting it. May tighten assumes with explicit justification.

**Confirmation (after both pipelines):** Run CBMC(H_buggy_P, f_original) for P ∈ {A, B}. SAT + concrete execution divergence confirms real bug detection; UNSAT means bug silenced.

**Subject selection:** Stratified subset of RQ2 GT SAT / LLM UNSAT cases, balanced across three taxonomy categories. RQ1 category labels reused directly.

**Primary metrics per pipeline P and category c:**
- Silencing rate SR_{P,c}: proportion where CBMC(H_buggy_P, f_original) = UNSAT
- Pass rate PR_{P,c}: proportion where pipeline achieves UNSAT on the mutant within budget

The joint (SR, PR) per category is the primary result: identifies where refine is low-cost vs where it strains LLM capability.

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
**Duration:** 2 weeks
**Criteria:** 8–10 papers read in depth; 2–3 page related work draft; able to articulate the distinction from NL2Contract / SpecMind in one sentence

#### Core Paper List

- [ ] NL2Contract (arxiv 2510.12702)
  - What it does:
  - How it differs from our work:
- [ ] SpecMind (arxiv 2602.20610)
  - What it does:
  - How it differs from our work:
- [ ] HarnessAgent (arxiv 2512.03420)
  - What it does:
  - How it differs from our work:
- [ ] AWS CBMC DFCC documentation
  - What it does:
  - How it differs from our work:
- [ ] AssertLLM
  - What it does:
  - How it differs from our work:
- [ ] Clover
  - What it does:
  - How it differs from our work:
- [ ] Re:Form (arxiv 2505.xxxx)
  - What it does:
  - How it differs from our work:
- [ ] Contract Strengthening via CHC (arxiv 2211.12228)
  - What it does:
  - How it differs from our work:

#### Related Work Grouping Structure

- [ ] LLM spec synthesis group
- [ ] Mutation testing for specs group
- [ ] Harness generation group
- [ ] LLM behaviour analysis group

#### Outputs

- [ ] Related work draft (2–3 pages)
- [ ] Novelty positioning map (X-axis: formal backing strength, Y-axis: industrial code scale)
- [ ] One-sentence differentiators: distinction from NL2Contract is ___; distinction from SpecMind is ___

---

### Phase 2: RQ1 Replication
**Goal:** Complete third-party taxonomy replication, finalise RQ1 results
**Duration:** 2 weeks
**Criteria:** κ > 0.7 (minimum), κ > 0.8 (target)

#### Blind Pilot Preparation

- [ ] Select 30 functions (covering all three categories, split evenly between aws-c-common and s2n-tls)
- [ ] Identify second rater (lab colleague or supervisor)
- [ ] Write taxonomy classification criteria document
  - [ ] Definition and examples for validity predicate
  - [ ] Definition and examples for length invariant
  - [ ] Definition and examples for frame condition

#### Replication Execution

- [ ] Two raters independently classify the 30 functions
- [ ] Compute inter-rater κ
- [ ] Meet to resolve disagreements
- [ ] Update taxonomy definitions if necessary

#### Outputs

- [ ] κ value: ___
- [ ] Final taxonomy document
- [ ] RQ1 final numbers:
  - pass rate: ___
  - recall: ___
  - validity miss: ___
  - length miss: ___
  - frame miss: ___
  - same-family ∆pp: ___
  - lib-specific ∆pp: ___

---

### Phase 3: Mutant Generation + Compilation Filtering
**Goal:** Build the compilation-passing mutant set M_all
**Duration:** 2 weeks
**Criteria:** ~1,900 compilation-passing mutants; at least 10 per function

#### Environment Setup

- [ ] Install universalmutator
- [ ] Configure gcc/clang compilation environment
- [ ] Write compilation filter script
- [ ] Write mutant metadata recording script (source function, operator type, file path)

#### Trial Run

- [ ] Trial run on 10 functions
- [ ] Inspect generated mutant quality
- [ ] Tune mutation operator parameters
- [ ] Verify correctness of compilation filter script

#### Full Run

- [ ] Batch run on all 108 functions
- [ ] Apply compilation filter
- [ ] Count mutants per function
- [ ] Check whether distribution is even

#### Outputs

- [ ] M_all size: ___ mutants
- [ ] Compilation pass rate: ___%
- [ ] Average mutants per function: ___
- [ ] Mutant operator type distribution

---

### Phase 4: Bidirectional CEX Confirm Infrastructure
**Goal:** Build a reliable batch-run and auto-classification pipeline
**Duration:** 2 weeks
**Criteria:** 100% accuracy on manual validation of 10 samples; total runtime estimate is reasonable

#### ESBMC Batch Run Script

- [ ] Input: mutant file + harness file
- [ ] Output: SAT/UNSAT + CEX content
- [ ] Timeout setting: 5 minutes per run
- [ ] Parallelisation: GNU parallel or Python multiprocessing
- [ ] Error handling: how to record timeouts / crashes

#### CEX Assertion Auto-Classification Script

- [ ] Parse ESBMC output format
- [ ] Extract triggered assert line numbers and content
- [ ] Map to taxonomy category (validity / length / frame)
- [ ] Handle unclassifiable cases

#### Four-Outcome Classification Script

- [ ] Input: R_GT and R_LLM
- [ ] Output: SAT/SAT, SAT/UNSAT, UNSAT/SAT, UNSAT/UNSAT
- [ ] Design results database schema

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

- [ ] ESBMC(H_GT, m) for all ~1,900 mutants
- [ ] ESBMC(H_LLM, m) for all ~1,900 mutants
  - [ ] LLM 1 × 6 prompt conditions
  - [ ] LLM 2 × 6 prompt conditions

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

---

### Phase 6: RQ2 Analysis
**Goal:** Complete all RQ2 statistical analysis and confirm kill rate gap
**Duration:** 3 weeks
**Criteria:** Kill rate gap is statistically significant (p < 0.05); ranking consistent with recall

#### Per-Category Kill Rate

- [ ] H_GT kill rate (validity / length / frame)
- [ ] H_LLM kill rate (validity / length / frame)
- [ ] Statistical significance test (Fisher's exact test)

#### Exclusive Kill Analysis

- [ ] Functions exclusively killed by H_LLM
  - [ ] Characteristics of these functions (comment coverage, complexity)
  - [ ] Categories of triggered assertions
- [ ] Functions exclusively killed by H_GT
  - [ ] Correspondence with RQ1 recall

#### Prompt Condition Stratification

- [ ] Kill rate statistics per prompt condition (all 6)
- [ ] Does same-family context improve kill rate?
- [ ] Consistency with RQ1 ∆recall

#### Figures

- [ ] Per-category kill rate comparison chart
- [ ] H_LLM-exclusive / H_GT-exclusive distribution chart
- [ ] Prompt condition vs kill rate chart

#### Outputs

- [ ] Per-category kill rate table:

  |  | H_GT | H_LLM | Gap | p-value |
  |--|------|-------|-----|---------|
  | validity | | | | |
  | length | | | | |
  | frame | | | | |

- [ ] H_LLM exclusive kill finding: ___
- [ ] Kill rate and recall ranking consistency: ___
- [ ] RQ2 conclusion draft

---

### Phase 7: RQ3 Experiment Run
**Goal:** Run Pipeline A and Pipeline B on stratified subset; obtain per-category SR and PR
**Duration:** 3 weeks
**Criteria:** Sufficient sample per category (target ≥ 20 per category per pipeline); pipeline reaches UNSAT within budget for > 70% of subjects

#### Subject Selection

- [ ] From RQ2 GT SAT / LLM UNSAT cases, stratify by category
- [ ] Target balanced sample: ~20–30 per category (adjust if RQ2 counts are low)
- [ ] If any category is under-represented, supplement with additional mutants of that type
- [ ] Prepare mutant files and confirm f_original is available for each subject

#### Pipeline A — Delete

- [ ] Provide LLM with mutant function labelled as correct implementation
- [ ] Run CBMC feedback loop; record each assertion deletion with iteration number
- [ ] Iterate until UNSAT or budget exhausted
- [ ] Pipeline A pass rate per category: ___

#### Pipeline B — Refine

- [ ] Same subjects; same LLM; provide full CEX on SAT (concrete input + violated assertion + category label)
- [ ] Instruct LLM: refine without deleting; justification required for any assume tightening
- [ ] Iterate until UNSAT or budget exhausted
- [ ] Pipeline B pass rate per category: ___

#### Confirmation Step

- [ ] Run CBMC(H_buggy_A, f_original) for all Pipeline A subjects
- [ ] Run CBMC(H_buggy_B, f_original) for all Pipeline B subjects
- [ ] For SAT cases: execute concrete CEX input against both f_original and f_buggy; confirm f_original(I) ≠ f_buggy(I)

#### Outputs

- [ ] Per-category pass rate table:

  |  | Pipeline A PR | Pipeline B PR |
  |--|---------------|---------------|
  | validity | | |
  | length | | |
  | frame | | |

- [ ] Per-category silencing rate table:

  |  | Pipeline A SR | Pipeline B SR |
  |--|---------------|---------------|
  | validity | | |
  | length | | |
  | frame | | |

- [ ] Preliminary narrative: which category is low-cost for refine, which forces tradeoff

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
| Related Work | ☐ Not started | Draft available |
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
- 2 LLMs × 6 prompt conditions
- Report per-condition results; do not report only averages

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
| 1 | Literature Review | ☐ | 2026/06/01 | |
| 2 | RQ1 Replication | ☐ | | |
| 3 | Mutant Generation | ☐ | | |
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
After mutant generation:
    Is M_all large enough? (< 1,000 → tune parameters)

After CEX confirmation:
    Is M_confirmed large enough? (< 400 → generate more mutants)
    Do UNSAT/SAT outcomes exist? (critical for the finding)

After RQ2:
    Is the kill rate gap significant? (p > 0.05 → revisit design)

After RQ3:
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

*Last updated: 2026/05/28*
