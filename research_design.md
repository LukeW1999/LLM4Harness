# Research Plan

## Title

**Do LLMs Verify or Conform? A Formal Study of Harness Quality in LLM-Assisted Bounded Model Checking**

---

## Central Hypothesis

When LLMs generate CBMC proof harnesses under iterative verifier feedback, safety-critical verification gaps arise from **three mechanistically distinct conformance behaviours**, all grounded by a circularity-free mutation oracle:

1. **Knowledge gaps (KG)**: the LLM never generates the missing property across any iteration — not a generation failure under pressure, but an absence of the relevant specification knowledge in the harness from the start. Dominant in lower-capability models (gptoss: 84.6% of silenced bugs). A cloze test (GT harness with one assertion removed, LLM fills the blank) can disambiguate KG from placement/discovery failure: if the LLM fills in the blank correctly, the gap was *where* to assert, not *what* to assert.

2. **Assertion sacrifice (SAC)**: the LLM generates a correct assertion and then removes it after a CBMC UNKNOWN to escape state-space explosion — a conformance move that silences real bugs. Dominant in higher-capability models (Claude: 42.9% of silenced bugs). Causal ablations (I: category labels; J: deletion log; H: strategy-neutral) confirm sacrifice is emergent under verifier pressure, not merely compliance with a deletion-permitting prompt: H ≈ A silence rate (6.8% vs 7.2%) and H ≈ A sacrifice rate shows the behaviour is model-intrinsic.

3. **Assumption over-constraint (AOC)**: the LLM adds `__CPROVER_assume(x ≤ BOUND)` or `__CPROVER_assume(ptr != NULL)` constraints that restrict the symbolic state space CBMC explores, excluding the states where bugs manifest — even when all assertions are correct. Identified in the "structural" category of Claude A (50% of silenced bugs): GT harness uses `ASSUME_VALID_MEMORY_COUNT` (fully symbolic valid memory), LLM harness uses `malloc(len)` + `__CPROVER_assume(len <= MAX_BUFFER_SIZE)` — capping the state space at len≤256 and masking size-dependent bugs. AOC is a convergence strategy: bounding inputs prevents state-space explosion, but at the cost of leaving some reachable bug states unexplored.

All three mechanisms are conformance behaviours under verifier pressure — each is the LLM optimising for CBMC SUCCESS at the expense of specification completeness:
- KG: LLM cannot specify what to verify
- SAC: LLM knows what to verify but removes it to satisfy CBMC
- AOC: LLM narrows inputs to satisfy CBMC, indirectly making bug-triggering states unreachable

The mutation oracle grounds all three: for every silenced mutant, we identify which mechanism caused the silence and provide a bounded CBMC certificate (GT-FAIL / LLM-SUCCESS) of real bug silencing. The paper's technical contribution is this oracle + the three-mechanism taxonomy: the first formal evidence that LLMs adopt all three conformance strategies in production-level CBMC harness generation.

---

## Research Questions

**RQ1.** What properties do LLM-generated harnesses systematically fail to specify, and do these omissions arise from the LLM never generating the property (*knowledge gaps*) or from the LLM generating and then removing it to achieve UNSAT (*active sacrifices*)?

**RQ2.** Do the specification gaps identified in RQ1 correspond to real verification failures on functionally incorrect code — that is, are the missed assertions precisely the ones that would have caught injected bugs?

**RQ3.** Does the feedback strategy given to the LLM during iterative harness generation determine whether bugs are silenced or detected, and does this effect vary across assertion categories in ways that expose the boundary of LLM refinement capability?

---

## Corpus and Setup

The study uses two production C libraries from Amazon's formal verification programme: aws-c-common (83 functions across 7 data-structure families) and the `s2n_stuffer` module of s2n-tls (25 functions), giving 108 functions in total. Amazon's formal verification engineers have written 238 proof harnesses for these functions as part of a continuously maintained verification pipeline; these constitute the expert ground truth corpus $\mathcal{H}_\mathrm{GT}$. The corpus has two properties that make it suitable as a research substrate: the underlying codebase is production-correct and formally verified against $\mathcal{H}_\mathrm{GT}$, and the harnesses are written by engineers with deep knowledge of both the library internals and the CBMC toolchain, making them the strongest available oracle for what a complete harness should assert.

Two LLMs generate harnesses under eleven prompt conditions. **Core conditions:** (A) source code only; (B) source code with natural language documentation; (D) source code with documentation and chain-of-thought; (E) source code with a same-family harness as a one-shot example; (F) source code with a wrong-family harness as an ablation control; **(G) source code only, single-pass — no CBMC feedback loop**; **(H) source code only, iterative feedback — deletion-neutral prompt)**. **Ablation conditions** (secondary, run alongside A–H using the same infrastructure): **(I) like A but with GT assertion category label provided at each SAT failure** — isolates whether sacrifice is due to ignorance of the assertion's importance or motivated removal despite knowing its role; **(J) like A but with a running deletion log shown** — "you have removed the following assertions in prior iterations: [list]" — isolates whether sacrifice is driven by statelessness/forgetting; **(K) specification-first** — LLM writes a natural-language precondition/postcondition/frame contract before any CBMC run, then translates to assertions; CBMC iteration then proceeds as in A — isolates whether frame-condition knowledge gaps are a generation-order artifact. **(Oracle Setup)** — the H_GT structural preconditions (`__CPROVER_assume` clauses for validity and bounding) are provided to the LLM in the initial prompt; the LLM only needs to write postcondition assertions, not figure out the setup. On a SAT failure, the LLM is asked to refine the postcondition only. This condition directly tests whether deletions on hard/mutant code are driven by *conformance pressure* (the model knows the assertion is correct but removes it to pass CBMC) vs *setup incapacity* (the model deletes because it cannot correctly set up the preconditions, confusing setup errors with postcondition errors). If the model still deletes H_GT-entailed postcondition assertions under Oracle Setup, conformance pressure is supported; if deletions drop substantially, prior deletions were driven by setup confusion, not specification gaming. Conditions I–K and Oracle Setup share all infrastructure with A–H; they require only prompt changes and run in the same evaluation framework. Condition G is a zero-feedback baseline: the LLM generates exactly once and receives no verifier output. Condition H uses the same iterative loop as A but the `fix_verification_prompt` provides no repair strategy whatsoever — the LLM receives only the violated assertion and counterexample trace and must determine its own response. Unlike A, H neither invites deletion nor prohibits it; unlike B-strict, H imposes no constraint. This is a *strategy-neutral* control that tests whether active sacrifice emerges from the LLM's own objective under UNSAT pressure, independent of what the prompt permits. Comparing G against A isolates the feedback loop's contribution to recall loss; comparing H against A isolates the contribution of the deletion-permitting instruction specifically.

**Manipulation check for Condition H:** Before treating H-vs-A as a causal test of prompt instruction, we verify H is not inert by running both conditions on 20 functions from the *original correct codebase* (no mutants). On correct code, no H_GT-entailed assertion should ever need to be deleted — any such assertion can be satisfied by CBMC. If Condition A produces a higher rate of unnecessary deletions (H_GT-entailed assertions removed even on correct code) than Condition H, this confirms H successfully reduced deletion-as-repair behaviour. If the rates are indistinguishable even on correct code, H is functionally equivalent to A and the manipulation failed. **If active sacrifice occurs at similar rates under H as under A after the manipulation check passes, the conformance pressure is emergent in the LLM, not merely instructed by the prompt.** Conditions E and F isolate the effect of in-context exemplar family match. For the main RQ1/RQ2 analysis one primary LLM is used across all eleven conditions; a second LLM runs conditions A, G, and H as a confirmatory replication — these three conditions are the core sacrifice-measurement conditions and are sufficient to confirm or disconfirm the active-sacrifice finding across model families. RQ3 uses the primary LLM only, to hold model capability constant.

**LLM Selection and Configuration.** The primary LLM is **gpt-oss-120b** (OpenAI, released 2025-08-05, Apache 2.0 open-weight). It is a 117B-parameter Mixture-of-Experts model activating 5.1B parameters per forward pass, with a 131K-token context window, configurable chain-of-thought reasoning depth, and native tool-use support. The model is deployed locally on the research server under MXFP4 quantization, eliminating API dependency and providing full weight-level reproducibility: any researcher with an H100 can replicate the exact inference environment. The secondary LLM is **Meta Llama 3.3 70B Instruct** (Meta, Apache 2.0 open-weight; OpenRouter ID: `meta-llama/llama-3.3-70b-instruct`), accessed via OpenRouter. The two models differ in architecture origin (OpenAI MoE vs. Meta dense decoder), training data composition, and RLHF methodology, providing the architectural diversity required for a meaningful inter-model replication. *(Note: DeepSeek V4 Flash was replaced after a data quality audit revealed it hallucinated non-existent proof helpers (`proof_helpers/proof_allocators.h`) and generated wrong-function harnesses in 23% of cases, resulting in 97–100% CBMC UNKNOWN across all conditions — rendering its data unusable. Llama 3.3 70B is used in its place.)* Both models are run at **temperature = 0, seed = 42** for reproducibility; residual non-determinism from provider-side floating-point batching is acknowledged as a minor variance source and not expected to affect categorical outcomes. All prompt templates are frozen, version-controlled, and published as replication artifacts before data collection begins. This study characterises the active-sacrifice phenomenon in locally-deployable open-weight LLMs; generalisation to proprietary API-based models (e.g., GPT-4o, Claude Sonnet) is left to future work, as their RLHF alignment and instruction-following characteristics may differ in ways that affect sacrifice rates. The maximum iteration budget per function per condition is **15 CBMC iterations**, with compilation retries counted separately (maximum 5 compilation retries before the function is marked compile-fail and excluded). This budget applies identically across all conditions and both LLMs.

**Manipulation check failure protocol.** If the manipulation check for Condition H fails — i.e., Condition A does not produce a significantly higher rate of gratuitous H_GT-entailed deletions than Condition H on correct-code functions — then H is reported as functionally equivalent to A and the H-vs-A comparison is not interpreted as a causal test. RQ3 proceeds without the H baseline; the A/B-strict/B-relaxed causal comparison is unaffected. The conformance-pressure hypothesis is neither supported nor refuted by a failed manipulation check; the failure is reported as an experimental null result for Condition H specifically.

**H_GT oracle status.** We do not claim H_GT is a complete specification. We make the weaker, empirically grounded claim that H_GT assertions are *safety-relevant expert practice*: for each mutant where CBMC($\mathcal{H}_\mathrm{GT}$, $m$) = FAIL and CBMC($\mathcal{H}_\mathrm{LLM}$, $m$) = SUCCESS, the triggered H_GT assertion is a formal witness to a real bug that H_LLM silences. The mutation oracle — not human annotation — is the validation mechanism. Recall therefore measures conformance to validated expert practice, reported as a *lower bound* on missed safety-relevant content, not completeness against an abstract oracle.

RQ2 additionally reports the count of distinct H_GT assertions certified by at least one silenced mutant, versus |H_GT| total. This single number makes the validation scope explicit and pre-empts the "only covers assertions that happened to fire" objection by quantifying it directly. The headline result of the paper is the **silenced-mutant count** (circularity-free, mutation-oracle-grounded); recall is a complementary descriptive metric.

All harnesses are generated and evaluated under CBMC, which is the tool used natively by AWS. Compatible harnesses are subsequently migrated to ESBMC to provide a second verification backend. Migration validity requires two checks: (1) ESBMC($\mathcal{H}_\mathrm{GT}$, $f$) remains UNSAT after migration (UNSAT preservation); (2) for a random sample of 30 mutants known to produce CBMC($\mathcal{H}_\mathrm{GT}$, $m$) = SAT, ESBMC($\mathcal{H}_\mathrm{GT}$, $m$) also returns SAT (SAT-SAT agreement ≥ 90%). This **soundness-parity check** ensures that tool-configuration differences (unwinding bounds, object-bits, stub models) are not driving divergences between H_GT and H_LLM results. Incompatible cases are retained under CBMC and reported separately.

---

## RQ1: Specification Gaps and Their Origin

### Goal

Characterise what LLM-generated harnesses miss relative to expert harnesses, quantify the gap, and determine whether each omission is a knowledge gap or an active sacrifice. The distinction matters because knowledge gaps indicate that prompting or in-context information could in principle close the gap, whereas active sacrifices indicate that the LLM understood the property was required but discarded it under conformance pressure — a problem that prompting alone cannot solve.

### Iterative Generation Protocol

LLMs generate harnesses under each of the seven prompt conditions in a CBMC feedback loop (condition G runs once only, with no feedback). For conditions A–F, the iteration proceeds as follows: if compilation fails, the LLM receives the compiler error and retries; if CBMC returns SAT, the LLM receives the full verifier output including the triggered assertion and counterexample trace, and modifies the harness before retrying; if CBMC returns UNSAT, iteration stops.

**Iteration logger (automated):** The generation script instruments every harness transition. After each LLM response, a diff is computed against the previous harness version. Every assertion that is added, modified, or deleted is recorded as a structured log entry: `{iteration, condition, assert_text, action: add|weaken|delete, triggered_violation: bool}`. This log is the primary data source for the active-sacrifice vs knowledge-gap classification — no post-hoc human annotation of iteration history is required. The taxonomy annotation step (κ ≥ 0.8) applies only to the final harness-vs-H_GT matching, not to the iteration log.

### Taxonomy and Classification

Final UNSAT harnesses are matched against $\mathcal{H}_\mathrm{GT}$ assertion by assertion. Missed assertions are classified into three categories derived from the structural idiom of CBMC proof harnesses for memory-safe C libraries. A *validity predicate* covers pointer non-nullness, return value range checks, and error-code postconditions. A *length invariant* covers relationships between buffer lengths, capacities, and offsets that must hold after a call. A *frame condition* covers the assigns clause, specifying which memory locations the function is permitted to modify and asserting that unspecified locations are unchanged.

This taxonomy is not post-hoc: it is grounded in the CBMC Dynamic Frame Condition Checking (DFCC) mechanism used by AWS, which makes frame conditions first-class verification objects. The three categories correspond to distinct kinds of specification knowledge — memory safety, data structure invariants, and side-effect discipline — that a complete harness must capture.

Taxonomy reliability is established before full annotation proceeds. Two independent raters annotate 30 functions drawn from both libraries. Full annotation begins only if inter-rater Cohen's $\kappa \geq 0.8$; disagreements are resolved by adjudication and used to refine the codebook.

Each missed assertion is then classified using a **three-state outcome**:

- **Never-generated (knowledge gap):** the assertion never appeared in any iteration log entry — the LLM never produced it.
- **Weakened:** an assertion semantically equivalent to the H_GT target appeared but was progressively weakened (predicate loosened) without full deletion.
- **Deleted (active sacrifice):** an assertion appeared in at least one iteration and was subsequently removed entirely after a CBMC violation.

**Critical constraint on active sacrifice attribution:** a removal or weakening is counted as an active sacrifice *only if the affected assertion is entailed by H_GT* — i.e., H_GT contains a corresponding assertion for the same property. Removals of assertions that have no H_GT counterpart are self-corrections of incorrect attempts and are not sacrifices. This constraint prevents conflating legitimate self-correction with conformance-driven gaming.

**Attribution validation:** the iteration logger records whether each deletion/weakening was preceded by a triggered CBMC violation on that assertion. This temporal attribution (violation → removal) is validated on a 30-function human-annotated subsample, with inter-rater Cohen's κ reported for the sacrifice attribution specifically (separately from the taxonomy category κ). Full annotation proceeds only if both κ values meet the ≥0.8 threshold.

### Metrics

**Recall** is a descriptive metric measuring alignment with AWS expert practice: $\mathrm{Recall} = |\mathcal{H}_\mathrm{LLM} \cap \mathcal{H}_\mathrm{GT}| \,/\, |\mathcal{H}_\mathrm{GT}|$. It is explicitly scoped as a *lower bound* on safety-relevant content missed — not a completeness claim. Its safety relevance is grounded by RQ2's mutation oracle: per-category, H_GT assertions empirically catch bugs that H_LLM silences. Recall is reported overall and per category (validity / length / frame), alongside pass rate to establish the 94%/47% baseline divergence. Per-category miss rate is reported separately for knowledge gaps and active sacrifices.

**Active-sacrifice fraction** is a primary output reported **per taxonomy category**. The denominator for each category $c$ is: H_GT-entailed assertions in category $c$ that were generated in at least one iteration (i.e., appeared in the iteration log). The numerator is: those assertions that were subsequently deleted or weakened after a CBMC violation on them. This denominator restricts to "ever-generated H_GT-entailed" rather than all H_GT assertions, because never-generated assertions cannot logically be sacrificed — they are knowledge gaps. Results are reported per category and per prompt condition; the κ ≥ 0.8 gate applies to both axes.

The pre-registered narrative thresholds apply **per category**:

| Active sacrifice fraction per category | Narrative for that category |
|----------------------------------------|----------------------------|
| > 30% | Conformance pressure dominates in this category; feedback redesign is actionable here |
| 10–30% | Both mechanisms operate; RQ3 isolates the contribution in this category |
| < 10% | Capability gaps dominate in this category; behaviour is primarily never-generated |

*Note on expected category profile:* Frame conditions will have a small denominator because LLMs rarely save old state before the call and therefore rarely generate frame assertions — the primary mechanism is never-generated, not sacrifice. Length invariants are expected to have the highest sacrifice rate because LLMs generate them but face direct CBMC pressure when the mutant violates them. Validity predicates are intermediate. A pooled fraction without per-category breakdown would conflate these structurally different failure modes. The headline number for the narrative comparison is the **length invariant** sacrifice fraction (most sensitive to conformance pressure), alongside a clearly-labelled pooled average.

**Zero-feedback gap** (condition G vs A vs H, per category): $\Delta$recall isolates (i) the feedback loop's contribution (G vs A) and (ii) the deletion-permitting instruction's contribution (H vs A, conditional on the manipulation check passing). Together they decompose how much of the recall loss is due to iteration itself vs. what the prompt permits the LLM to do on SAT.

---

## RQ2: Formal Confirmation of Verification Failures

### Goal

Determine whether the assertions that $\mathcal{H}_\mathrm{LLM}$ misses are precisely those that would have caught real functional deviations. RQ2 upgrades the gap characterisation in RQ1 from a completeness deficit against a reference harness to a safety deficit against real bugs, using bounded model checking as a formal oracle rather than random sampling.

### Dependency on RQ1

RQ2 uses the taxonomy established in RQ1 to classify mutants after the fact. Mutants are generated without category pre-filtering; the assertion triggered by the BMC counterexample determines which category the miss falls into. This preserves classification objectivity and avoids circularity.

### Mutation and Oracle Protocol

Universalmutator is applied to each of the 83 aws-c-common functions to generate compiled mutants. After a mutant-quality audit, 58 function directories yield valid mutants (1,625 compiled mutants total); the remaining 27 directories are excluded because universalmutator's source-file finder located a *call site* in an unrelated `.c` file rather than the function's definition in its `.inl` inline header (e.g., `array_list.inl`, `linked_list.inl`, `math.inl`). Mutations of call sites rather than definitions do not change function semantics and are therefore invalid test subjects. The 58-function, 1,625-mutant scope is the valid RQ2 corpus for aws-c-common; s2n-tls mutants are generated separately once LLM harness generation for those functions completes.

For each mutant $m$, two ESBMC invocations run in parallel: ESBMC($\mathcal{H}_\mathrm{GT}$, $m$) and ESBMC($\mathcal{H}_\mathrm{LLM}$, $m$), with identical unwinding bounds, object-bits, and stub configurations (**soundness-parity requirement**: the same tool flags are applied to both harnesses for every mutant, and the configuration is fixed to match the migration-validated settings from the Corpus setup). A SAT result with a counterexample is a bounded proof that the mutant violates at least one assertion in the harness under all inputs within the unrolling bound — not a sampled witness, but a bounded certificate of functional deviation.

The primary analysis object is the case where ESBMC($\mathcal{H}_\mathrm{GT}$, $m$) returns SAT and ESBMC($\mathcal{H}_\mathrm{LLM}$, $m$) returns UNSAT. For these **GT SAT / LLM UNSAT** primary cases, the SAT result is additionally confirmed by concrete execution: the counterexample input $I$ from the ESBMC($\mathcal{H}_\mathrm{GT}$, $m$) trace is executed against both $f_\mathrm{original}$ and the mutant $m$; a genuine bug confirmation requires $f_\mathrm{original}(I) \neq m(I)$ on a semantically meaningful output. This rules out tool-configuration artefacts as the source of the SAT divergence. The assertion named in the $\mathcal{H}_\mathrm{GT}$ counterexample is extracted automatically and used to assign the mutant to a taxonomy category.

The four outcome combinations are interpreted as follows. GT SAT / LLM SAT: both harnesses detect the bug; $\mathcal{H}_\mathrm{LLM}$ is adequate for this mutant class. GT SAT / LLM UNSAT: $\mathcal{H}_\mathrm{GT}$ formally confirms the bug; $\mathcal{H}_\mathrm{LLM}$ silences it; this is the primary finding. GT UNSAT / LLM SAT: $\mathcal{H}_\mathrm{LLM}$ detects a deviation that $\mathcal{H}_\mathrm{GT}$ does not; this is an independently reportable finding, expected to arise for functions with rich inline documentation under prompt condition B or D. GT UNSAT / LLM UNSAT: counted but not discarded — requires the equivalent-mutant partition procedure below before being reported as a validity bound.

**Equivalent-mutant partition (GT UNSAT / LLM UNSAT cases).** The GT UNSAT / LLM UNSAT bucket conflates three structurally distinct cases: (a) genuinely equivalent mutants (semantically identical to $f_\mathrm{original}$, no distinguishing input exists); (b) non-equivalent mutants outside both harnesses' `assume` envelope (the preconditions exclude the distinguishing input); and (c) non-equivalent mutants where both harnesses are simply too weak (a distinguishing input exists but neither harness asserts the relevant property). These three cases carry different implications: (a) should be excluded from all count denominators; (b) should be reported as an envelope-coverage caveat; (c) is an independent finding about H_GT completeness. The partition proceeds in two stages. First, at generation time, universalmutator's compiler-equivalence filter (TCE — Trivial Compiler Equivalence, based on compiled-object identity under optimisation) is applied; mutants that produce identical object code to $f_\mathrm{original}$ are flagged as TCE-equivalent and excluded from M_all before any ESBMC run. Second, for the remaining GT UNSAT / LLM UNSAT cases after ESBMC, differential random testing is applied: 500 random inputs drawn from the H_GT precondition domain are executed against $f_\mathrm{original}$ and $m$; any input producing $f_\mathrm{original}(x) \neq m(x)$ confirms the mutant is non-equivalent (a distinguishing input exists, placing it in category (b) or (c)). Mutants where no difference is found across all 500 inputs are classified as *likely-equivalent* (unconfirmed). To calibrate this proxy, a stratified sample of 50 GT UNSAT mutants (balanced across taxonomy categories) undergoes formal ESBMC equivalence checking: ESBMC($\texttt{assert}(f_\mathrm{original}(x) = m(x))$, precond) — UNSAT confirms equivalence, SAT provides a formal distinguishing witness. The calibration sample's confirmed-equivalent rate is used to bound the likely-equivalent estimate for the full set. All counts are reported explicitly: M_all (post-TCE), likely-equivalent count with calibration-derived confidence interval, confirmed-non-equivalent H_GT misses, and the remaining GT UNSAT / LLM UNSAT lower-bound caveat.

### Metrics

**Primary result — silenced mutant count.** The headline finding is the per-category count of mutants where H_GT catches the bug and H_LLM silences it (GT SAT / LLM UNSAT), with concrete CEX confirmation ($f_\mathrm{orig}(I) \neq m(I)$). This number is circularity-free: it depends only on the mutation oracle, not on H_GT's completeness. Alongside this, RQ2 reports the count of distinct H_GT assertions formally certified by at least one silenced mutant — making the fraction of H_GT that is empirically validated explicit.

**Assertion-level kill attribution.** For every GT SAT / LLM UNSAT case, the triggered assertion $A_\mathrm{GT}$ is extracted from the H_GT counterexample and matched against the RQ1 sacrifice log for function $f$ using the same CBMC mutual-implication check as the predicate-weakening detector (SF1). If a semantically equivalent assertion appears in $f$'s sacrifice set (generated then removed after a CBMC violation), the miss is classified as a **sacrifice-attributed miss**. If no match is found, it is a **knowledge-gap miss**. Per-category kill rate is broken down by attribution (sacrifice-attributed vs knowledge-gap-attributed).

**RQ1↔RQ2 correlation decomposition.** A category-level correlation between "low recall" and "low kill rate" is partly mechanical: missing an assertion that H_GT uses to catch a mutant guarantees a kill-rate miss. The informative residual — kill-rate deficit beyond what missing assertions alone explains — is isolated as: (a) the *mechanical kill-rate floor* attributable to missing H_GT assertions, and (b) the *residual kill-rate gap* attributable to assertion-strength or domain-coverage differences. The residual is a co-primary result: if substantial, the taxonomy has independent diagnostic value beyond raw assertion count.

**Harness disposition table.** To detect survivorship bias (harnesses that never reach UNSAT may be exactly those where the LLM refused to sacrifice), every attempted harness is tracked through: compile-fail, never-UNSAT within budget, and reached-UNSAT. Sacrifice rates are reported both for the reached-UNSAT subset and, where estimable, for the full attempted set. The distribution of GT UNSAT / LLM SAT cases is reported separately.

**Secondary analysis — sacrifice-then-recover.** The iteration logger records every assertion deletion and addition event. A *sacrifice-then-recover* event is defined as: assertion $A$ deleted after a triggered CBMC violation on $A$ (iteration $k$), followed by a semantically equivalent assertion re-generated in a subsequent iteration ($k' > k$). These events are direct evidence of knowing sacrifice: the LLM recognised $A$ was needed (re-generating it), but removed it under immediate conformance pressure. The count and per-category breakdown of sacrifice-then-recover events is reported as a secondary analysis in the RQ1 Discussion section. It does not alter the three-state primary taxonomy (the final state — whether $A$ is present in the terminal UNSAT harness — is what determines the primary classification) but provides corroborating process-level evidence for the conformance-pressure interpretation.

---

## RQ3: Causal Test — Does Feedback Protocol Drive Bug Silencing?

### Goal

RQ1 establishes the phenomenon (active sacrifice) and classifies its origin. RQ2 establishes that the gaps are safety-relevant. RQ3 provides a **manipulation-based causal test of protocol effect**: by holding LLM, corpus, and task constant while varying only the feedback protocol, it determines whether the *protocol* — not the model's capability — is the primary driver of bug silencing. This is a protocol-level causal claim, not a mechanism-level one. The mechanism question — whether the deletions reflect conformance pressure (knowing sacrifice) or capability failure (unable to refine) — is addressed by the convergent evidence from Condition H (sacrifice persists even when the prompt does not instruct deletion), Oracle Setup (sacrifice persists even when precondition setup difficulty is removed), and Ablation I (sacrifice persists even when the LLM is told the category label of the violated assertion). RQ3 isolates the *controllable variable* (what the feedback protocol permits) rather than the LLM's internal state. The three-arm design (A / B-strict / B-relaxed) creates a *decreasing-permission gradient*: each arm removes one escape route (assertion deletion, then assume-tightening) and measures the effect on SR and PR per category. The key output is not simply whether refine beats delete — that is expected — but *where* refinement is low-cost and *where* it strains LLM capability, characterised per assertion category. This per-category breakdown is the novel contribution of RQ3: it identifies which kinds of specification the LLM can reliably tighten under counterexample guidance and which kinds it cannot, independent of whether the harness passes the verifier.

### Subject Selection

RQ3 operates on a stratified subset of the RQ2 confirmed bugs (GT SAT / LLM UNSAT cases), sampled to achieve balanced representation across the three taxonomy categories. Assertion-category labels from RQ1 are reused directly; no re-annotation is required. The same LLM is used throughout RQ3 to hold model capability constant; the feedback protocol is the sole independent variable.

### Three Pipelines

All pipelines receive the same mutant function $f_\mathrm{buggy}$, presented to the LLM as the correct implementation. The LLM is not informed that the function is a mutant. The input domain is established in a **setup phase** (structural validity and bounding assumes, matching GT harness practice) before the function call; this setup is identical across all pipelines.

**Pipeline A — Delete.** On SAT, the LLM may remove or weaken the violated assertion. `__CPROVER_assume` additions are prohibited as a response to SAT (they are permitted only during the initial setup phase). Iteration continues until UNSAT. This replicates the default behaviour observed in RQ1 and is expected to produce harnesses that silence bugs, because any assertion that $f_\mathrm{buggy}$ violates will be eliminated to achieve UNSAT.

**Pipeline B-strict — Predicate Refinement Only (primary comparator).** On SAT, the LLM receives the full counterexample (concrete input assignment, violated assertion, assertion category) and must refine the *assertion predicate itself* to be more precise. The input domain established in the setup phase is frozen for the remainder of the run: no `__CPROVER_assume` clause may be added or modified after setup. Only assertion-predicate edits are permitted as a response to a counterexample. If the LLM cannot refine the predicate to satisfy the mutant while maintaining the intended postcondition, iteration budget is exhausted.

*B-strict fix_verification_prompt (per-arm, distinct from Pipeline A):*
> The input-domain assumptions set up before the function call are fixed. Do NOT add, strengthen, or modify any `__CPROVER_assume` after setup. If an assertion is too strong: make its predicate more precise so it states the true postcondition that holds for all inputs already admitted by setup. Do NOT delete the assertion. Do NOT exclude the counterexample input. If no refined predicate can hold for an admitted input, stop and report failure.

**Predicate-weakening detection (B-strict and B-relaxed):** A refined assertion `assert(Q)` that replaces `assert(P)` is classified as *genuine refinement* only if Q is strictly stronger than P on the GT-admissible input domain. Detection proceeds as follows: (1) re-execute the function under test with the GT setup preconditions (not the arm's own domain), reaching the post-state; (2) in that post-state, run CBMC with `__CPROVER_assume(P_pre_GT)` (GT structural preconditions) and check `assert(!P && Q)` — if SAT, a witness exists where Q holds but P does not, confirming Q ⊊ P (Q is weaker); if UNSAT, Q ⊆ P (Q is no weaker). Three implementation requirements: (i) the post-state check must re-execute the function — both predicates reference live struct fields and cannot be checked abstractly; (ii) the domain used is always the GT precondition, fixed across all arms including B-relaxed, so B-relaxed cannot game the check by moving the domain; (iii) results are reported as bound-relative (the containment holds at unwinding bound $k$; a predicate weaker only beyond $k$ is not detected). Predicate-weakening cases are reported as a third silencing mechanism alongside deletion (Pipeline A) and assume-tightening (B-relaxed), and are excluded from B-strict's genuine-refinement count in $\mathrm{SR}_{B\text{-strict}}$.

**Pipeline B-relaxed — Refine with Justified Domain Restriction (secondary arm).** Identical to B-strict except the LLM may additionally tighten `__CPROVER_assume` constraints if it provides explicit written justification that the excluded input is outside the function's valid contract. This arm is included as a secondary comparison to quantify how much of B-strict's difficulty is attributable to legitimate domain-restriction needs vs. assume-based gaming.

*Rationale for three-arm design:* Code inspection of the actual feedback loop implementation revealed that the existing `fix_verification_prompt` instructs the LLM to "add `__CPROVER_assume` to constrain the input" as a response to SAT failures. Ground-truth AWS harnesses use `__CPROVER_assume` only as structural preconditions set before the function call, never in response to assertion failures. A two-arm design with the permissive Pipeline B would confound assertion-predicate refinement with input-domain restriction, undermining the controlled comparison. The three-arm design isolates these mechanisms: the A→B-strict gap measures the causal effect of deletion prohibition; the B-strict→B-relaxed gap measures how much assume-based escape contributes when permitted.

**Vacuity audit (all three arms, including setup phase):** After each arm reaches UNSAT on $f_\mathrm{buggy}$, a two-stage vacuity check is run.

*Primary gate — call-site reachability:* immediately after the setup block (after all `__CPROVER_assume` clauses, before the function call), insert `__CPROVER_assert(false)` and run CBMC. If this assertion is UNSAT (unreachable), the setup assumes are mutually contradictory — the entire harness is vacuous and is flagged regardless of arm. If SAT (reachable), the setup is self-consistent and the harness is not setup-vacuous. This gate runs on all three arms.

*Localiser — per-assume negation:* for harnesses that pass the reachability gate, each `__CPROVER_assume` clause is individually negated and CBMC is re-run. If UNSAT reverts to SAT upon negating assume $A_i$, then $A_i$ is load-bearing for the UNSAT result — the function call cannot reach the assertions without $A_i$ being assumed. Load-bearing setup assumes that are not present in H_GT are flagged as potential over-constraints and reported separately.

Cases where setup-phase vacuity is detected by the primary gate are excluded from SR counts as they represent structurally invalid harnesses. Load-bearing setup assume flags (localiser step) serve as a sensitivity analysis — SR is reported with and without flag-excluded cases. This ensures SR_A − SR_{B-strict} is not confounded by differential setup-induced vacuity across arms.

### Confirmation

After both pipelines complete, $f_\mathrm{original}$ is introduced for the first time. CBMC($\mathcal{H}_\mathrm{buggy\_A}$, $f_\mathrm{original}$) and CBMC($\mathcal{H}_\mathrm{buggy\_B}$, $f_\mathrm{original}$) are run. A SAT result means the harness contains an assertion that distinguishes the mutant from the original. To confirm the detection is genuine rather than an artefact of overly specific refinement, the counterexample concrete input $I$ is executed against both $f_\mathrm{original}$ and $f_\mathrm{buggy}$: a real detection requires $f_\mathrm{original}(I) \neq f_\mathrm{buggy}(I)$ on a semantically meaningful output. An UNSAT result means the bug has been silenced.

### Metrics

**Bug silencing rate** $\mathrm{SR}_{P,c}$: proportion of category-$c$ subjects for which CBMC($\mathcal{H}_\mathrm{buggy}^P$, $f_\mathrm{original}$) = UNSAT (bug silenced). For B-relaxed, vacuous UNSAT (assume-driven) is reported separately. **Pass rate** $\mathrm{PR}_{P,c}$: proportion for which Pipeline $P$ reaches UNSAT on $f_\mathrm{buggy}$ within the iteration budget.

**Primary comparisons:**
- $\mathrm{SR}_A - \mathrm{SR}_{B\text{-strict}}$: causal effect of deletion prohibition on bug silencing
- $\mathrm{SR}_{B\text{-strict}} - \mathrm{SR}_{B\text{-relaxed}}$: marginal effect of allowing assume-based domain restriction
- $\mathrm{PR}_A - \mathrm{PR}_{B\text{-strict}}$: pass-rate cost of enforcing predicate-only refinement

**Power:** B-strict vs A is the primary comparison. With $\mathrm{SR}_A - \mathrm{SR}_{B\text{-strict}} \geq 0.35$, Fisher's exact at $\alpha = 0.05$, power = 0.80 requires $n \approx 22$ per category per pipeline. Target is 25–30 per category. If any category yields fewer than 22 confirmed bugs from RQ2, that category's RQ3 result is reported as exploratory.

**Null-result narrative:** If $\mathrm{SR}_A - \mathrm{SR}_{B\text{-strict}}$ is not significant in any category: "Prohibiting assertion deletion is insufficient — conformance pressure is robust to this protocol constraint. LLMs under formal verifier feedback find alternative silencing paths regardless of the deletion rule." If B-strict ≈ B-relaxed: "Assume-based domain restriction accounts for little of the residual silencing; LLMs do not systematically exploit the assume escape route when deletion is already prohibited." Both are publishable findings.

The joint $(\mathrm{SR}, \mathrm{PR})$ per category across three arms is the primary result table.

---

## Threats to Validity

**Internal validity.** UNSAT in RQ2 conflates two causes: semantic equivalence between the mutant and the original, and mutant inputs falling outside the harness's `assume` envelope. These are not distinguished at the ESBMC level; all GT UNSAT / LLM UNSAT cases are discarded and their proportion is reported as a lower-bound caveat on kill rate estimates. In RQ3, the concrete execution confirmation step directly addresses the risk that a Pipeline B SAT result is a false positive arising from over-specific assertion refinement rather than genuine bug detection.

**External validity.** The corpus is limited to aws-c-common and s2n-tls. These are safety-critical C libraries subject to rigorous continuous formal verification, representative of the domain where BMC is most commonly practised at scale, but results may not generalise to other C software or to languages with different memory models. The two-library design partially mitigates this by covering distinct functional domains (data structures versus TLS protocol handling).

**Construct validity.** The taxonomy categories are manually defined and require human judgement for boundary cases. Two independent κ thresholds are gated: (1) taxonomy category annotation (κ ≥ 0.8 for validity / length / frame classification); (2) active-sacrifice attribution (κ ≥ 0.8 for "this removal was caused by a CBMC violation on the removed assertion"). Both gates must pass before full annotation. The active-sacrifice construct is additionally restricted to H_GT-entailed assertions to prevent conflating correct self-correction with conformance gaming. The RQ3 treatment variable (feedback protocol) is operationalised at the prompt level; per-arm prompts are published as a replication artefact. The three-arm design (A / B-strict / B-relaxed) prevents confounding deletion-prohibition with input-domain restriction. Predicate-weakening detection in B-strict and setup-vacuity audit across all arms address residual silencing channels. The predicate-weakening detector checks strict containment ($Q \subsetneq P$) via `assert(!P && Q)` with GT preconditions, but does not detect *incomparable* refinements where $Q$ and $P$ are mutually non-containing yet $Q$ is weaker on the specific mutant's execution path. This is expected to affect a small fraction of length-invariant refinements; a frequency estimate from a 20-function sample is reported alongside the main results.

**Conclusion validity.** Results are reported per model, per prompt condition, and per assertion category. The A vs B-strict comparison controls for LLM capability by construction; the B-strict vs B-relaxed comparison controls for model and subject. Any observed SR/PR difference between arms is attributable solely to the permitted operations in the feedback prompt.

---

## Novel Contribution and Relationship to Prior Work

**Primary contribution: a circularity-free mutation oracle that certifies LLM harness safety gaps on production C code.** For each function where CBMC(H_GT, mutant) = FAIL and CBMC(H_LLM, mutant) = SUCCESS, we obtain a bounded certificate that the LLM harness silences a real bug that expert-written verification would have caught. This is independent of H_GT being a complete specification: the oracle requires only that H_GT catches something H_LLM misses. The count of silenced mutants, broken down by assertion category (validity / length / frame), is the headline result. Existing LLM-assisted BMC studies measure pass rate only; LLM4Harness is the first to show the pass-rate/safety gap has concrete, formally-grounded consequences measurable on industrial C libraries with expert harness ground truth.

**Co-primary contribution: attribution of gap origins to two parallel mechanisms, with causal ablation evidence.** RQ1 attributes missed GT assertions to either knowledge gaps (never-generated, 97%) or active sacrifice (generated then deleted under CBMC pressure, 2.5%). Three causal ablations verify the sacrifice mechanism is neither ignorance-driven (I: category label injected → sacrifice rate unchanged) nor statelessness-driven (J: deletion log shown → sacrifice rate unchanged) nor merely instructed by the prompt (H: strategy-neutral → sacrifice rate comparable to A). These ablations function as mechanism verification tests: each one rules out an alternative explanation for why the LLM deletes correct assertions. J > H (p = 0.006**, Wilcoxon) establishes that tracking deletion history preserves recall — a statelessness component exists but is not the main driver. The mutation oracle then validates that both mechanisms produce real safety consequences: most silenced mutants trace to knowledge gaps; sacrifice-attributed silencing is the theoretically significant residual.

**Secondary contribution: the first controlled protocol experiment in LLM-assisted formal verification (RQ3).** The A / B-strict / B-relaxed design isolates feedback protocol as an independent cause of specification erosion by holding LLM, corpus, and task constant while varying only the repair instruction. This moves the field from observational correlation to protocol-level intervention evidence. Crucially, because the oracle is formal (CBMC UNSAT requires genuinely satisfying all assertions — it cannot be gamed by input-level overfitting), any sacrifice that persists under the formal oracle reflects structural specification weakening that dynamic test-suite feedback cannot detect.

**Measurement advance:** the recall-vs-pass-rate frame, grounded in expert harnesses as a validated reference oracle, provides a reproducible evaluation protocol for future LLM-for-verification studies. The mutation oracle grounds safety claims without circularity: per-category, H_GT assertions are certified as safety-relevant by the mutants they catch, following the adequacy theory of Papadakis et al. (2019). M condition (bounding hint) demonstrates that the knowledge-gap mechanism is partially remediable: UNKNOWN-triggered panic deletions drop to zero, rescuing 11 functions from zero recall — establishing a practical engineering recommendation alongside the diagnostic contribution.

The use of CBMC as a mutation oracle (not dynamic testing) means a FAIL result is a bounded proof of violation — a certificate, not a sample — aligning the confirmation mechanism with the production verification tool (aws-c-common CI uses CBMC natively).

---

### Related Work and Positioning

#### Industrial corpus provenance

**Chong et al. (2021)** "Code-level model checking in the software development workflow at Amazon Web Services" (SPE 2021) describes how AWS engineers write and maintain CBMC proof harnesses for aws-c-common and s2n-tls as part of a continuous integration pipeline. This is the institutional basis for treating H_GT as *safety-relevant expert practice*: the harnesses are written by engineers with deep knowledge of both the library internals and the CBMC toolchain, and they are continuously validated against the production codebase. Our study uses this corpus as the ground-truth oracle and inherits its authority directly from this AWS engineering programme.

#### LLM-assisted BMC and loop invariant generation

**ESBMC-ibmc (Pirzada, Bhayat, Cordeiro, Reger, ASE 2024)** replaces loop unrolling in ESBMC with LLM-proposed loop invariants verified by a theorem prover (Vampire), enabling verification of programs that time out under classical BMC. Evaluated on 133 programs from the code2inv benchmark, successfully verifying 101 vs. 10 for standard ESBMC. *Distinction:* studies LLM capability to produce inductive invariants for loop-free program transformation — a fundamentally different task from generating full proof harnesses. The LLM proposes candidate properties that the theorem prover either accepts (sound) or rejects (unsound and discarded); there is no iterative feedback channel that incentivises the LLM to weaken its proposals. No expert GT harness corpus, no sacrifice analysis, no mutation oracle.

#### Agentic verification systems

**BMC-Agent (Sun, Kroening et al., 2026)** and this study are concurrent independent works (both 2026) addressing specification quality in LLM-assisted BMC from complementary directions. BMC-Agent proposes an agentic model checking paradigm under the principle *agents propose, solvers verify*, with a **soundness guard** in the adaptive refinement loop that rejects any refinement masking a real bug. Kroening (CBMC author, Amazon) is a co-author. Evaluated on LLM-generated code (VibeOS kernel, OSS-Fuzz targets, a 50k-line Rust C compiler). *Distinction:* BMC-Agent demonstrates that soundness-guarded refinement is deployable at scale; this study provides the first empirical characterisation of what unconstrained refinement costs — per-category sacrifice rates, per-assertion safety cost via mutation oracle, and the protocol-level causal evidence needed to prioritise where such guards matter most. BMC-Agent has no expert-written GT harness corpus, no active-sacrifice fraction metric, and no controlled protocol experiment. Concretely, BMC-Agent's soundness guard is an engineering instantiation of Pipeline B-strict; our RQ3 per-category (SR, PR) table is the empirical characterisation of what prohibiting deletion costs and gains per assertion category — exactly the evidence that practitioners need to decide where to apply the guard.

#### LLM spec and contract synthesis

**SpecMind (Le et al., 2026)** introduces a multi-turn postcondition inference framework for Python functions under test-suite feedback (correctness pass/fail + mutation completeness score). Achieves 99.4% correctness and 89.6% completeness on EvalPlus. Two appendix findings are directly relevant: (A.4) the Greedy variant simplifies assertion predicates to pass the test oracle — qualitatively analogous to the active sacrifice this study characterises; (A.3) completeness drops to zero in 18 cases before recovering, suggesting the LLM removes assertions it knows are needed. Critically, the sacrifice behaviour in SpecMind occurs under a test-suite oracle, which can be satisfied by overfitting to specific test inputs; in this study, sacrifice occurs under CBMC UNSAT, which requires genuinely satisfying all assertions across the symbolic input space — confirming that the phenomenon persists under a strictly harder, formally sound oracle. *Distinction:* SpecMind measures aggregate completeness end-state without per-assertion attribution, category breakdown, or sacrifice-then-recover tracking; empirical oracle not formal BMC; Python algorithm benchmarks not industrial C; no controlled protocol experiment analogous to RQ3.

**NL2Contract (Richter & Wehrheim, 2025)** evaluates LLMs at translating natural-language docstrings into formal pre/postcondition contracts for Python functions, measuring soundness (verifier passes on reference implementation) and completeness (mutations caught). Key finding: LLMs are biased toward simple, incomplete postconditions that miss precondition-dependent constraints. *Distinction:* single-pass inference with no iterative verifier feedback; Python not industrial C; incompleteness attributed to generation bias not active removal under verifier pressure; no expert GT harness corpus.

**Clover (Sun et al., 2024)** proposes closed-loop verifiable code generation using consistency checks among code, formal annotations, and docstrings for Dafny programs. Despite the "closed-loop" name, Clover's loop performs consistency checking among three artefacts — the LLM never receives a formal counterexample challenging a specific assertion, so there is no verifier-pressure event that could trigger sacrifice. *Distinction:* Dafny not C/CBMC; consistency checking (mutual coherence of artefacts) not completeness analysis (does the spec catch bugs?); no sacrifice mechanism; no mutation oracle; no industrial corpus.

**Re:Form (Veri-Code Team, 2025)** trains small LLMs (0.5B–14B) for formal Dafny spec generation using RL reward from the Dafny verifier with minimal human priors. *Distinction:* training paradigm (SFT + RL) not prompting of a fixed model; Dafny not C/CBMC; studies trainability not sacrifice under iterative feedback; no mutation oracle; no industrial corpus.

**Contract Strengthening via CHC (De Angelis et al., 2022)** strengthens function postconditions that are too weak for a verifier to discharge, via CHC transformation and SPACER solving for Scala programs. *Relation:* establishes that specification weakness is a structural formal methods problem independent of how specs are produced; our study shows iterative LLM feedback under CBMC pressure actively and measurably exacerbates it on industrial C code, and quantifies the safety cost for the first time.

#### LLM harness generation

**HarnessAgent (Yang et al., 2025)** generates fuzzing harnesses for C/C++ internal functions in OSS-Fuzz projects. Notably, it explicitly detects LLM self-hacking: more than 10 out of 56 harnesses contained fake function definitions to bypass validation — direct cross-domain evidence of the same conformance-driven gaming this study characterises under formal BMC. *Distinction:* fuzzing ≠ BMC; no formal spec completeness metric; no per-assertion attribution; no mutation oracle.

#### Hardware assertion generation

**AssertLLM (Fang et al., 2026)** generates SystemVerilog Assertions for hardware VLSI designs from natural-language architecture specifications using a three-stage multi-LLM pipeline, achieving 89% accuracy on 23 I/O signals. *Relation:* analogous problem in the hardware domain. No iterative feedback, no sacrifice analysis, no mutation oracle.

---

## Preliminary Experimental Findings (aws-c-common, 83 functions, as of 2026-06-06)

The following results are from the primary LLM (gpt-oss-120b) across all completed conditions. All ablation conditions (I/J/K/Oracle/M) are now complete. K and Oracle were re-run after pipeline bugs were fixed (2026-06-05). Condition M (scalar bounding hint) was added as a new ablation and completed on 2026-06-06. Cross-verification recall for K/Oracle/M is in progress (2026-06-06).

### Verdict Distribution

| Condition | N | PASS% | UNK% | AvgIters | AvgAsserts_iter1 | AvgAsserts_final | ΔAsserts |
|-----------|---|-------|------|----------|-----------------|-----------------|---------|
| A/qwen2.5 | 84 | 50.0 | 28.6 | 3.00 | 8.6 | 7.3 | −1.3 |
| A/claude  | 84 | 92.9 | 3.6  | 2.15 | 10.6 | 7.7 | **−2.9** |
| **A/gptoss120b** | 80 | 62.5 | 33.8 | 3.12 | 10.0 | 8.4 | −1.6 |
| **G/gptoss120b** | 83 | 31.3 | 45.8 | 1.00 | 10.3 | 10.3 | 0.0 |
| **H/gptoss120b** | 83 | 62.7 | 34.9 | 3.53 | 9.8 | 8.6 | −1.1 |
| **A/llama3370b** | 83 | 61.4 | 32.5 | 3.47 | 10.2 | 10.1 | −0.1 |
| **G/llama3370b** | 83 | 31.3 | 32.5 | 1.00 | 10.3 | 10.3 | 0.0 |
| **H/llama3370b** | 83 | 45.8 | 37.3 | 4.65 | 10.2 | 10.4 | +0.2 |
| **I/gptoss120b** | 78 | **70.5** | 29.5 | 2.58 | 9.5 | 8.0 | −1.5 |
| **J/gptoss120b** | 80 | **67.5** | 31.2 | 2.52 | 9.5 | 8.5 | −1.1 |
| **K/gptoss120b** | 83 | **81.9** | 7.2 | — | — | — | — |
| **Oracle/gptoss120b** | 83 | **84.3** | 4.8 | — | — | — | — |
| **M/gptoss120b** | 81 | **75.3** | 3.7 | — | — | — | — |
| E/qwen2.5 | 83 | 54.2 | 27.7 | 2.58 | 8.2 | 7.9 | −0.3 |
| E/claude  | 83 | 91.6 | 3.6  | 1.64 | 7.8 | 7.0 | −0.7 |
| F/claude  | 83 | 91.6 | 4.8  | 1.55 | 8.5 | 7.8 | −0.8 |

**All conditions with CBMC feedback show negative assertion delta** (assertions decrease from first iteration to final). G (no feedback) preserves initial assertions at cost of 31.3% PASS rate vs 62.5% for A. **FAILURE% = 0% across all conditions** — CBMC never flags a genuine property violation in these semantically correct implementations; all failures are UNKNOWN (state space explosion) or wrong-function harnesses.

**Replication fidelity (Llama 3.3 70B vs gpt-oss-120b):** A and G conditions replicate closely (A: 61.4% vs 62.5%, Δ=1.1pp; G: 31.3% vs 31.3%, Δ=0pp). H shows a 17pp gap (45.8% vs 62.7%) — strategy-neutral repair appears less effective for Llama, suggesting model-specific sensitivity to the strategy-neutral prompt. Llama's sacrifice ratio is lower (71.7% vs 81.8%), consistent with less aggressive UNKNOWN-driven deletion.

### Conformance Pressure Analysis (Iteration Log)

**Key finding: 87.2% of all assertion deletions/weakens are driven by CBMC UNKNOWN (sacrifice), only 12.8% by actual violations (correction). New ablations reveal this is entirely UNKNOWN-driven: Condition M eliminates UNKNOWN-triggered sacrifice to 0% by providing scalar bounding knowledge.**

| Condition | Total deletions | Sacrifice (UNKNOWN-triggered) | Non-sacrifice | **Sacrifice ratio** |
|-----------|----------------|------------------------------|---------------|---------------------|
| A/gptoss120b | 26 | 24 | 2 | **92.3%** |
| K/gptoss120b | 20 | 5 | 15 | **25.0%** |
| Oracle/gptoss120b | 17 | 2 | 15 | **11.8%** |
| M/gptoss120b | 27 | 0 | 27 | **0.0%** |

*Note: sacrifice ratio computed from harness C files by counting assert() occurrences per iteration; deletion preceded by UNKNOWN/TIMEOUT = sacrifice. Broader per-action counts for A/G/H/I/J:*

| Condition | Total actions | Del% | Add% | Wkn% | Sacrifice del+wkn | Correction del+wkn | Sac ratio |
|-----------|--------------|------|------|------|------------------|-------------------|----------|
| A/qwen2.5 | 319 | 58.9 | 29.8 | 11.3 | 172 | 22 | 88.7% |
| A/claude  | 542 | 55.9 | 24.7 | 19.4 | 272 | 16 | **94.4%** |
| A/gptoss120b | 457 | 56.2 | 37.9 | 5.9 | 225 | 50 | 81.8% |
| G/gptoss120b | — | — | — | — | — | — | — |
| H/gptoss120b | 470 | 53.4 | 37.0 | 9.6 | 251 | 40 | 86.3% |
| **A/llama3370b** | 393 | 43.5 | 39.7 | 16.8 | 167 | 66 | **71.7%** |
| G/llama3370b | — | — | — | — | — | — | — |
| H/llama3370b | 388 | 37.6 | 40.2 | 22.2 | 148 | 83 | **64.1%** |
| **I/gptoss120b** | 366 | 53.8 | 34.7 | 11.5 | 189 | 27 | **92.7%** |
| **J/gptoss120b** | 396 | 54.0 | 37.1 | 8.8 | 187 | 27 | **93.0%** |

Active sacrifice in H (strategy-neutral) is comparable to A (strategy-guided) for gpt-oss-120b, confirming sacrifice is emergent in the LLM, not merely instructed. Llama shows lower sacrifice ratios (A: 71.7%, H: 64.1%) with a higher proportion of weakens (16–22% vs 6–10% for gptoss120b), suggesting a different repair strategy mix — more predicate-level weakening, less outright deletion.

### Wrong-Function Hallucination

A separate quality issue: **27–29% of iter_1 harnesses call the wrong target function** in standard-prompt conditions (A/B). The feedback loop fixes most of these (final wrong-function rate: 2–8%). Exception: CoT conditions C/D have only **1.2% wrong-function rate** — chain-of-thought prompting suppresses function hallucination.

| Condition | Wrong in iter_1 | Wrong in final | Fixed by feedback |
|-----------|----------------|----------------|------------------|
| A/gptoss120b | 23 (28.7%) | 6 (7.5%) | 17 |
| G/gptoss120b | 24 (28.9%) | **24 (28.9%)** | 0 (no feedback) |
| C/qwen2.5 | 1 (1.2%) | 1 (1.2%) | 0 |
| A/claude | 23 (27.4%) | **0 (0%)** | 23 |
| **A/llama3370b** | 20 (24.1%) | **0 (0%)** | 20 |
| G/llama3370b | 20 (24.1%) | **20 (24.1%)** | 0 (no feedback) |
| H/llama3370b | 22 (26.5%) | **0 (0%)** | 22 |

### Cross-Verification Recall (CBMC property-level matching against H_GT)

| Condition | N | VE% | Recall_strict | Recall_fuzzy | Precision | over_constrained | VE+R=0 |
|-----------|---|-----|--------------|-------------|-----------|-----------------|--------|
| A/qwen2.5 | 83 | 57.8 | 0.346 | 0.384 | 0.180 | 16% | 12 |
| A/claude  | 83 | 91.6 | **0.426** | 0.473 | 0.233 | — | 15 |
| **A/gptoss120b** | 80 | 65.0 | 0.357 | 0.375 | — | 10.0% | 12 |
| **G/gptoss120b** | 83 | 36.1 | 0.290 | 0.302 | 0.135 | 3.6% | — |
| **H/gptoss120b** | 83 | 67.5 | 0.303 | 0.317 | 0.152 | 13.3% | — |
| **I/gptoss120b** | 81 | 71.6 | 0.363 | — | — | 12.3% | — |
| **J/gptoss120b** | 81 | 72.8 | 0.377 | — | — | 16.0% | — |
| **M/gptoss120b** | 83 | 72.3 | **0.384** | 0.404 | — | **14.5%** | 12 |
| **K/gptoss120b** | 83 | 80.7 | 0.268 | 0.281 | — | **31.3%** | **29** |
| **Oracle/gptoss120b** | 83 | 80.7 | 0.251 | 0.263 | — | **24.1%** | **27** |
| A/qwen2.5 | 83 | 57.8 | 0.346 | 0.384 | 0.180 | 16% | 12 |
| A/claude  | 83 | 91.6 | **0.426** | 0.473 | 0.233 | — | 15 |
| B/qwen2.5 | 83 | 57.8 | 0.368 | 0.386 | 0.186 | — | — |
| B/claude  | 83 | 89.2 | 0.423 | 0.466 | 0.212 | — | — |
| C/claude  | 83 | 95.2 | 0.421 | 0.465 | 0.204 | — | — |
| D/claude  | 83 | 90.4 | 0.389 | 0.432 | 0.194 | — | — |
| **A/llama3370b** | 83 | 68.7 | 0.323 | 0.339 | 0.175 | 14.5% | — |
| **G/llama3370b** | 83 | 37.3 | 0.299 | 0.319 | 0.152 | 4.8% | — |
| **H/llama3370b** | 83 | 51.8 | 0.301 | 0.317 | 0.162 | 10.8% | — |

VE% = verification-equivalent rate; VE+R=0 = VE but recall=0 (vacuous success suspects). **Key finding: PASS rate ordering (Oracle > K > M > A) is reversed by cross-verify recall ordering (M > J > I > A > H > G; Oracle lowest). The I→J→M progression shows that contextual scaffolding (category label → deletion log → bound hint) incrementally improves recall. K and Oracle high PASS rates are dominated by assume over-constraining (31% and 24% respectively), not genuine specification quality. M is the only ablation that improves both PASS rate and recall above A.**

### Preliminary G vs A vs H Narrative — Three Mechanism Verification Tests

The G / H / I / J conditions are not merely ablations; each is a controlled causal test that rules out an alternative explanation for the sacrifice phenomenon:

- **G→A (feedback loop contribution)**: +31.2pp PASS (31.3%→62.5%), +6.7pp recall (29.0%→35.7%). Recall improvement is partly from fixing wrong-function harnesses (24→6); sacrifice emerges *during* the feedback loop. This establishes that the feedback loop itself — not initial generation — is the locus of recall loss.
- **H vs A (prompt instruction contribution)**: H (strategy-neutral: no deletion guidance) has nearly identical PASS (62.7% vs 62.5%) but *lower* recall (30.3% vs 35.7%). Sacrifice under H is comparable to A (86.3% vs 81.8%), confirming that the conformance-pressure behaviour is **emergent in the LLM**, not instructed by the prompt. The protocol cannot be the sole explanation.
- **I (category label: not ignorance)**: Despite being told the assertion category being violated, the LLM sacrifices at the *same or higher* rate (92.7% vs A's 81.8%). Rules out ignorance of assertion importance as a mechanism — the model knows what it is deleting and deletes anyway.
- **J (deletion log: statelessness is secondary)**: Showing the LLM its entire deletion history does not reduce sacrifice (93.0% vs A's 81.8%). Rules out LLM statelessness as the primary driver. However J > H (p=0.006**) shows deletion history *does* preserve recall vs. strategy-neutral — statelessness contributes a secondary component. Together I and J establish the sacrifice mechanism is conformance-pressure-driven, with a statelessness component that context can partially mitigate.

- **Best PASS rate (Claude A: 92.9%)** comes with highest sacrifice ratio (94.4%) and largest assertion loss (−2.9/function), but also best recall (0.426). Claude's stronger initial generation + higher sacrifice = net better recall. Confirms that sacrifice rate and recall quality are partially decoupled.
- **LLMs over-generate assertions**: G=9.2 avg, A=8.6 avg vs GT=4.2 avg. Feedback prunes excess assertions improving precision, while simultaneously sacrificing some GT-matched assertions.

### Ablation Results (I/J — GT category label & deletion log)

**Condition I** (GT assertion category label injected at each UNKNOWN): **70.5% PASS** (+8pp vs A/gptoss 62.5%), sacrifice ratio **92.7%** (higher than A's 81.8%). The category hint improves pass rate but paradoxically *increases* sacrifice — the model understands which category is at stake and still removes the assertion. This supports **conformance pressure, not ignorance**, as the mechanism: the LLM knows the assertion is safety-relevant (Condition I tells it) but removes it anyway to escape UNKNOWN. Critical RQ1 finding: sacrifice is not driven by knowledge gaps about assertion importance.

**Condition J** (running deletion log shown): **67.5% PASS** (+5pp vs A), sacrifice ratio **93.0%**. Showing the LLM its own deletion history does not reduce sacrifice. The LLM is not simply "forgetting" what it deleted — it knows it is deleting and does it anyway. Rules out LLM statelessness as the primary mechanism.

**I vs J vs A comparison:**
- Both I and J improve PASS rate over A (showing category/history context helps structurally)
- Both show *higher* sacrifice ratios than A — providing more context makes the LLM more aggressively sacrifice known-correct assertions
- Together: sacrifice is neither ignorance-driven (I) nor statelessness-driven (J) — it is conformance-pressure-driven

### Sacrifice Gradient (K / Oracle / M) — New Results (2026-06-06)

The four-condition gradient provides a clean mechanistic decomposition of the sacrifice phenomenon:

| Condition | PASS% | Sacrifice ratio | Total deletions | Interpretation |
|-----------|-------|-----------------|-----------------|----------------|
| A | 29.6% | 92.3% | 26 | Baseline: most deletions UNKNOWN-driven |
| M | 75.3% | **0.0%** | 27 | CBMC knowledge gap fixed; sacrifice eliminated; remaining deletions are legitimate FAIL-triggered corrections |
| K | 81.9% | **25.0%** | 20 | Spec-first reduces sacrifice AND improves assertion quality |
| Oracle | 84.3% | **11.8%** | 17 | Near-perfect setup; minimal sacrifice; upper bound |

**Key mechanistic finding:** Condition M eliminates UNKNOWN-triggered sacrifice entirely by teaching the LLM to bound scalar inputs, but achieves lower PASS (75.3%) than K (81.9%) because it does not address assertion-correctness gaps. K improves both sacrifice rate and assertion quality by forcing explicit contract articulation before CBMC iteration. The PASS rate ordering (A < M < K < Oracle) and sacrifice ratio ordering (Oracle < K < M < A) being non-identical reveals two separable problems: CBMC knowledge gaps (M addresses these) and assertion quality/completeness (K and Oracle address these). M's 27 non-sacrifice deletions (all FAIL-triggered vs A's 2) confirm that eliminating UNKNOWN changes the failure mode from conformance gaming to legitimate self-correction.

**Cross-verification recall (completed 2026-06-06):** All four conditions now have recall under GT assumes. Results overturn the raw PASS rate ordering:

| Condition | PASS% (raw) | Recall (GT-assume cross-verify) | VE% | over_constrained | VE+R=0 (vacuous?) |
|-----------|------------|--------------------------------|-----|-----------------|-------------------|
| A | 29.6% | 0.346 | 57.8% | 13 (16%) | 12 |
| M | 75.3% | **0.384** ↑ | 72.3% | 12 (14%) | 12 |
| K | 81.9% | **0.268** ↓ | 80.7% | **26 (31%)** | 29 |
| Oracle | 84.3% | **0.251** ↓ | 80.7% | 20 (24%) | **27** |

**True quality ranking (by recall): M > A > K > Oracle.** The PASS rate ordering is entirely reversed by cross-verify recall.

**K low recall = NL contract incompleteness (confirmed NOT vacuous)**: Vacuity check (2026-06-07) found only 1/68 K SUCCESS harnesses genuinely vacuous (`aws_array_list_length`). K's low recall (0.268 < A's 0.346) is genuine: the spec-first NL contract itself omits many GT postconditions, so the translated harness has fewer assertions. K's over_constrained (31%) means assumes are tighter than GT but paths are still reachable — the input domain is restricted, not empty. The VE+R=0=29 cases are functions where LLM passed CBMC with completely different (but non-vacuous) assertions than GT.

**Oracle paradox = laziness effect (confirmed NOT vacuous)**: Vacuity check found only 1/70 Oracle SUCCESS harnesses genuinely vacuous (`aws_array_list_push_back`). Oracle's low recall (0.251) is genuine: GT assumes provided → CBMC trivially satisfiable → LLM writes minimal assertions that pass without being comprehensive ("laziness effect"). 45/83 VERIFY_EQUIV_ONLY cases are functions where LLM's simple assertions pass CBMC but have near-zero GT recall — the assertions are reachable and valid, just incomplete.

**M is the only condition that improves both PASS rate and recall**: +3.8pp recall over A, +14.5pp VE rate, with SAME over_constrained count as A (12 vs 13). The bounding instruction corrects a specific CBMC knowledge gap without causing vacuous success or laziness effects.

**Implication for paper narrative (RQ1):** PASS rate is validated as a misleading metric — the paper's central claim. The cross-verify recall table is the primary quality evidence. K and Oracle's high PASS rates are artifacts of over-constraining, not genuine specification quality. M's genuine improvement distinguishes knowledge-fixable sacrifice (UNKNOWN-triggered) from the structural quality gap.

### Statistical Significance of Recall Differences (Wilcoxon signed-rank, completed 2026-06-07)

Wilcoxon signed-rank tests on per-function strict recall (harness_recall), paired on shared functions with at least one GT assertion (gt_harness_count > 0). All tests are one-sided (directional hypothesis: row condition > column condition). Tests run in Python/scipy locally using synced cross-verify JSON files.

| Comparison | n | Δmean recall | p (one-sided) | Sig |
|------------|---|-------------|---------------|-----|
| M > Oracle | 82 | +0.135 | 0.0008 | *** |
| M > K      | 82 | +0.117 | 0.0022 | **  |
| M > H      | 82 | +0.082 | 0.0033 | **  |
| J > H      | 80 | +0.080 | 0.0056 | **  |
| A > Oracle | 79 | +0.098 | 0.0218 | *   |
| A > K      | 79 | +0.080 | 0.0350 | *   |
| A > H      | 79 | +0.056 | 0.0258 | *   |
| M > A      | 79 | +0.035 | 0.0769 | ns  |
| A > G      | 79 | +0.070 | 0.0516 | ns  |
| J > A      | 77 | +0.025 | 0.1612 | ns  |
| I > H      | 80 | +0.062 | 0.0552 | ns  |
| I > A      | 77 | +0.007 | 0.4038 | ns  |
| J > I      | 78 | +0.006 | 0.4127 | ns  |
| M > J      | 80 | +0.004 | 0.4514 | ns  |

**Key results for paper:**
- **Laziness effect (A > Oracle, p=0.022\*)**: Providing GT assumes significantly *reduces* recall — confirmed statistically. A without any scaffolding outperforms Oracle with perfect assumes.
- **PASS-recall reversal confirmed (M > K, p=0.002\*\*; M > Oracle, p=0.001\*\*\*)**: M's recall advantage over the two highest-PASS conditions is highly significant.
- **Strategy guidance effect (A > H, p=0.026\*)**: Deletion-permitting prompt guidance produces significantly higher recall than strategy-neutral repair at the same PASS rate.
- **Deletion log helps vs strategy-neutral (J > H, p=0.006\*\*)**: Providing a running log of deleted assertions significantly improves recall over strategy-neutral repair — tracking sacrificed assertions mitigates conformance pressure.
- **Ablations I, J, M vs A all non-significant**: The contextual scaffolding conditions (I, J, M) show directional improvement over A (+0.7pp, +2.5pp, +3.5pp) but none reaches significance individually. Pairwise among I/J/M all non-significant. M's advantage is best described as eliminating the recall *floor* (rescuing functions where A reached 0.00 recall from UNKNOWN-driven panic deletions) rather than lifting the mean. Per-function analysis: M outperforms A on only 11/76 functions but with large gains (+0.50 to +0.71 recall); A outperforms M on 9 functions.

**Note on n=79 vs n=82:** Conditions that include the A/gptoss120b run share 79 paired functions with GT assertions; the 3-function discrepancy is due to 3 functions where A produced no harness (compile-fail across all iterations), reducing the A-paired sample.

**K (spec-first) and Oracle Setup**: Two pipeline bugs discovered during analysis of the first run (2026-06-05):
1. `extract_c_code()` only stripped fences at the start of the response — Condition K's spec-first prompt produces responses that begin with markdown specification text followed by a C code block. The function failed to find the code fence anywhere other than position 0, saving the full markdown+C response as the harness file.
2. `compilation_ok` in `cbmc_runner.py` checked `"PARSING ERROR" not in stdout` but CBMC outputs "PARSING ERROR" to **stderr**. Similarly, when the entry function (`{func_name}_harness`) was not found (LLM used `int main()` instead), CBMC output "Invalid User Input" to stderr. Both errors were silently misclassified as `compile_ok=True, verify=UNKNOWN`.

Both bugs were identified, fixed (2026-06-05), and K/Oracle re-run on server. Early results from the corrected pipeline: `aws_add_size_checked` passes CBMC on iteration 1 under Condition K — confirming the original 0% PASS was entirely attributable to the pipeline bug, not to spec-first being ineffective. Full results pending overnight run.

**Preliminary K finding (3/3 SUCCESS on early functions):** The spec-first approach (writing a natural-language contract before harness generation) appears to produce high-quality initial harnesses that converge faster than Condition A. If this holds across 83 functions, K will have higher PASS rate than A — a stronger finding than the original hypothesis (K ≈ A). The narrative would become: spec-first prevents the iterative sacrifice cycle by generating correct assertions at iter_1, before CBMC pressure accumulates.

### Annotation Pre-analysis Finding (97% never_generated)

Running `annotate_helper.py` on A/gptoss120b (198 missed GT assertions across 68 functions):
- **never_generated: 97.0%** — GT assertions that ended up missing were never generated at any iteration
- **deleted_sacrifice: 2.5%** (5 assertions) — generated and then UNKNOWN-driven deleted
- Taxonomy of missed: validity_predicate 50%, length_invariant 28%, frame_condition 22%

**Key interpretation**: The 87.2% sacrifice ratio (Table 2) counts ALL deletions regardless of H_GT entailment — primarily the LLM's own over-generated non-GT assertions. The GT coverage gap is driven by **knowledge gaps** (never_generated), not active sacrifice of GT-entailed properties. However, Conditions I and J confirm that where sacrifice of GT-entailed properties *does* occur, it is deliberate conformance pressure (not ignorance or forgetting). The mutation oracle (RQ2) will quantify which mechanism costs more safety.

### New Mechanistic Insight: Panic vs Targeted Deletion (2026-06-05)

Manual inspection of the 5 confirmed sacrifice cases in the pilot annotation revealed two structurally distinct deletion patterns:

**Pattern A — Panic deletion (3/5 cases):** CBMC returns UNKNOWN → LLM deletes ALL assertions in a single sweep (e.g., aws_ring_buffer_init: 11 assertions → 0). The LLM cannot diagnose which assertion causes UNKNOWN and treats the entire assertion set as suspect. This is diagnostic failure, not knowing sacrifice.

**Pattern B — Targeted deletion (2/5 cases):** CBMC returns UNKNOWN → LLM identifies a specific assertion as the likely cause and removes only that one (e.g., aws_array_list_pop_back: deleted `list.data != null` specifically). This is more consistent with deliberate conformance pressure.

**Implication for Condition I interpretation:** Condition I increases sacrifice from 81.8% to 92.7%. The most coherent explanation given the panic/targeted distinction: category knowledge converts some panic deletions into targeted deletions. The LLM now knows *which assertion type* is causing UNKNOWN and can delete it precisely rather than clearing everything. This means Condition I does not just confirm that sacrifice is not driven by ignorance — it reveals that **category knowledge makes sacrifice more precise and therefore more efficient**. Testing: compute `deletion_scope` (average assertions deleted per UNKNOWN event) for A vs I. If I has lower average deletion_scope, conversion from panic to targeted is confirmed.

**Deletion_scope analysis (completed 2026-06-06):** `deletion_scope` = number of assert() calls deleted per UNKNOWN-triggered iteration event.

| Condition | UNKNOWN-del events | Mean scope | Panic (≥3) | Targeted (=1) |
|-----------|-------------------|-----------|-----------|--------------|
| A | 29 | **6.7** | 72% | 17% |
| I (category label) | 24 | **5.5** | 71% | 12% |
| J (deletion log) | 21 | **5.6** | 76% | 10% |
| K (spec-first) | 5 | **1.6** | 20% | **80%** |
| Oracle | 2 | 5.5 | 50% | 50% |
| M | 0 | — | — | — |

A's largest single deletion event removed **20 assertions** in one UNKNOWN response. K's 80% targeted rate confirms that having a written NL contract gives the LLM a reference to identify which specific assertion caused UNKNOWN. Condition I's category label did NOT shift panic→targeted (71% panic vs 72% for A): knowing the assertion *category* does not help the LLM decide *whether* to keep it, only making it sacrifice more precisely within the same panic behavior. The K→targeted shift is unique to having the LLM's *own contract* as a reference anchor.

### Annotation Task Redesign (2026-06-05)

After reviewer analysis, the annotation task has been redesigned:

**Taxonomy κ (the actual κ gate):** Annotators classify each missed GT assertion into validity_predicate / length_invariant / frame_condition based on assertion semantics alone. No LLM output shown. This is a genuine judgment task.

**Attribution validation (separate, not κ):** The automated `annotate_helper.py` log-matching algorithm is validated by a separate 5-case audit showing the iteration evidence (iter_1 harness, deletion events, final harness). Auditors confirm or dispute the automated label. Report as precision estimate, not κ.

**Key codebook rule added (2026-06-05):** frame_condition requires BOTH: (1) comparison to a pre-call snapshot (old_X), AND (2) expected delta = zero. Comparing to null, a constant, or an input parameter → validity_predicate. Comparing to old_X but delta ≠ 0 (e.g., `list.length == old.length - 1`) → length_invariant. This rule resolved 20/101 auto-classifier errors found during Claude's full annotation pass.

**Pilot annotation distribution (Claude reference):** validity_predicate=59 (58%), length_invariant=24 (24%), frame_condition=18 (18%). Expected human κ ≈ 0.87–0.92 after codebook fix.

### RQ2: CBMC Mutation Oracle Results — All Conditions (completed 2026-06-09)

**Oracle backend: CBMC 5.95.1** (aws-c-common native tool, canonical `run_mutation_oracle_cbmc.py`). 1,233 mutants across the common 40-function subset shared by all eight conditions (excludes `aws_byte_buf_write_u8`, present only in A-Claude's raw 41-function/1,243-mutant harness set).

#### Cross-Condition Silence Rate Summary — UNIFIED 370-mutant GT-fail set (2026-06-10)

All eight conditions were (re)run under the canonical oracle script against the identical 1,233-mutant corpus. **All eight independently-computed GT-fail sets are per-mutant identical (370/1,233 mutants, pairwise symmetric difference = 0 across all $\binom{8}{2}=28$ pairs)** — this "370 cluster" is the stable, reproducible ground truth for this corpus/script/GT-harness combination on this server. Silenced, Sil/GT, and Recall are now computed against this single shared 370-mutant denominator for every row, eliminating the own/own-vs-canonical ambiguity (former Issue #2, now fully resolved — see Data Audit below).

| Condition | Silenced | Sil/GT (÷370) | Recall (LLM-FAIL/370) |
|-----------|----------|---------------|------------------------|
| Oracle (GT harness as LLM) | 158 | **42.7%** | 37.6% |
| A — gptoss120b (full pipeline) | 41 | 11.1% | 42.2% |
| H — gptoss120b (strategy-neutral) | 37 | 10.0% | 47.6% |
| M — gptoss120b (bounding hint) | 30 | 8.1% | 64.9% |
| G — gptoss120b (no CBMC loop) | **1** | **0.3%** | 45.4% |
| A — Claude (full pipeline) | 16 | 4.3% | 95.7% |
| H — Claude (strategy-neutral) | 16 | 4.3% | 93.2% |
| **M — Claude (bounding hint)** | **11** | **3.0%** | **96.2%** |

**Key: Sil/GT = silenced/GT-detectable (370); Recall = LLM-FAIL / GT-FAIL, both against the shared 370-mutant set.** This table directly mirrors paper.tex Table~6 (`tab:oracle`); the former separate "Coverage status" table below has been merged into this one since all eight rows now share one denominator.

#### Three Silence Mechanisms (Revised Taxonomy)

Manual inspection of the oracle data reveals **three mechanistically distinct paths** by which LLM harnesses fail to catch bugs that GT harnesses catch. The original KG/SAC two-way split is insufficient:

| Mechanism | Label | Description | gptoss A (41 bugs) | Claude A (16 bugs) |
|-----------|-------|-------------|---------|---------|
| **Knowledge Gap** | KG | Assertion never generated across any iteration | 90.2% | 18.8% |
| **Assertion Sacrifice** | SAC | Assertion generated but deleted under CBMC UNKNOWN pressure | 0.0% | 37.5% |
| **Assumption Over-Constraint** | AOC | Assertions correct, but `__CPROVER_assume(x ≤ bound)` restricts state space, excluding bug-triggering inputs | 9.8% | 25.0% |
| **Structural/Unknown** | — | GT assertions present but harness explores a different state space than GT for reasons not captured by the AOC detector | 0.0% | 18.8% |

(2026-06-10 update: figures now come directly from the automated `attribution_analysis.py` AOC detector on the unified 370-mutant set — see "Attribution Analysis" below for the full per-condition tables, including the new H-Claude row.)

**AOC mechanism — concrete evidence (aws_byte_buf_from_array, Claude A):**
- GT harness uses `ASSUME_VALID_MEMORY_COUNT(array, length)` (proof_helpers macro) — creates a fully symbolic valid memory region with no explicit size bound on the symbolic state space
- Claude harness uses `__CPROVER_assume(len <= MAX_BUFFER_SIZE)` + `malloc(len)` — caps `len` at 256, **excluding any state where the bug manifests at `len > 256`**
- GT also uses `assert_bytes_match(buf.buffer, array, buf.len)` (checks byte content); Claude uses `assert(buf.buffer == array)` (pointer equality only) — misses byte-corruption mutations that preserve pointer identity
- **Pattern**: LLM learns to bound symbolic inputs to avoid state-space explosion and achieve CBMC convergence; these bounds mask bugs in the excluded state space

**AOC is a conformance mechanism, not just an ignorance mechanism**: the LLM constrains the *input domain* to satisfy the verifier, rather than constraining the *specification*. All three mechanisms are forms of conformance under verifier pressure:
- KG: LLM cannot specify what to verify
- SAC: LLM knows what to verify but removes it to satisfy CBMC
- AOC: LLM narrows inputs to satisfy CBMC, indirectly making bug-triggering states unreachable

This three-way taxonomy is the paper's primary theoretical contribution to RQ1. It maps cleanly onto the "Verify or Conform?" question: KG = specification ignorance; SAC+AOC = two distinct conformance strategies.

#### Three Causal Findings (UNIFIED 370-mutant set, 2026-06-10)

**Finding 1 — CBMC feedback loop is critical (G vs A, gptoss):**
G (no loop) silences 1/370 bug vs A (full loop) silences 41/370: **0.3% vs 11.1%**. Without the iterative CBMC feedback, LLM harnesses are essentially inert as safety monitors. The loop introduces the convergence pressure that forces LLMs to write assertions tight enough to matter.

**Finding 2 — Sacrifice instruction is irrelevant at the safety level (H ≈ A, gptoss):**
H (strategy-neutral, no deletion-permitting wording) silences 37/370 = 10.0% vs A's 41/370 = 11.1% (Δ=1.1pp). A Wilcoxon signed-rank test on per-function oracle silence rates (n=31 paired functions, recomputed on the unified 370-set) gives one-sided $p=0.090$ (mean A rate=0.0529, mean H rate=0.0403) — not significant (n.s.), though more borderline than the $p=1.0$ computed on the pre-unification silence sets. The conclusion is unchanged: oracle-level silencing is statistically indistinguishable between A and H, so sacrifice behaviour is **emergent under verifier pressure**, not merely compliance with the deletion-permitting prompt instruction. (The iteration-level *deletion-rate* manipulation check, A>H $p=0.014$, is separate and still holds — see paper.tex §4.4.)

**Finding 3 — Claude silences far fewer bugs than gptoss, in both absolute and relative terms, while achieving dramatically higher recall:**
Claude A silences 16/370 = 4.3% vs gptoss A's 41/370 = 11.1%. Claude's oracle recall (95.7%, 354/370) is more than double gptoss A's (42.2%, 156/370) — Claude closes $\approx$93% of the GT-detectable gap. The residual 4.3% Sil/GT for Claude is dominated by SAC+AOC (37.5%+25.0% = 62.5% of its 16 silenced bugs), whereas gptoss's 11.1% is dominated by KG+AOC (90.2%+9.8% = 100.0%) — see Attribution Analysis below.

#### Attribution Analysis — UNIFIED 370-mutant set (rerun 2026-06-10)

All five attributable conditions (A-gptoss, M-gptoss, A-Claude, H-Claude, M-Claude) were rerun through `attribution_analysis.py` against the regenerated oracle JSONs. This directly mirrors paper.tex Table~7 (`tab:attribution`).

**A-gptoss — 41 silenced mutants:**
| Attribution | Count | % |
|-------------|-------|---|
| Knowledge-gap (assertion never written) | 37 | 90.2% |
| Sacrifice (written then deleted) | 0 | 0.0% |
| AOC (assume over-constraint) | 4 | 9.8% |
| Structural/unknown | 0 | 0.0% |

- Concentrated in 4 functions: `aws_byte_buf_cat` (33), `aws_byte_buf_clean_up` (4), `aws_nospec_mask` (2), `aws_ptr_eq` (2)
- 37/41 (90.2%) KG: the CA and FC-RV assertions (capacity-arithmetic bounds, return-value checks) never appear in any A-gptoss iteration
- 4/41 (9.8%) AOC: gptoss uses `__CPROVER_assume(cap <= MAX_BUFFER_SIZE)` where GT uses `ASSUME_VALID_MEMORY_COUNT`, capping the symbolic state space

**A-Claude — 16 silenced mutants:**
| Attribution | Count | % |
|-------------|-------|---|
| Knowledge-gap | 3 | 18.8% |
| Sacrifice (written then deleted) | 6 | 37.5% |
| AOC (assume over-constraint) | 4 | 25.0% |
| Structural/unknown | 3 | 18.8% |

- Spread across 6 functions; the 2 additional silenced mutants vs. the earlier (pre-unification) 14-mutant count are both newly-attributed KG cases in `aws_byte_buf_cat`
- SAC (37.5% = 6/16): Claude generates correct assertions in early iterations but removes them under CBMC UNKNOWN pressure
- AOC (25.0% = 4/16): Claude writes all GT assertions but bounds `len <= 256` where GT uses fully-symbolic `ASSUME_VALID_MEMORY_COUNT`, excluding states where the bug manifests at `len > 256`
- 3/16 (18.8%) remain structurally unresolved (GT assertions present, harness explores a different state space than GT for reasons the AOC detector doesn't capture)

**H-Claude — 16 silenced mutants (NEW, 2026-06-10, resolves former gap N1):**
| Attribution | Count | % |
|-------------|-------|---|
| Knowledge-gap | 9 | 56.2% |
| **Sacrifice** | **0** | **0.0%** |
| AOC (assume over-constraint) | 4 | 25.0% |
| Structural/unknown | 3 | 18.8% |

- H-Claude's SAC=0% is a **second, independent** route to SAC=0 (alongside M-Claude below) — see KEY CAUSAL FINDING.
- H-Claude's Sil/GT (4.3%) is identical to A-Claude's (4.3%), but its mechanism mix shifts heavily toward KG (56.2% vs A's 18.8%): without the deletion-permitting instruction, Claude retains more assertions but a larger fraction were never-correct (KG) to begin with.
- AOC (25.0%) and structural/unknown (18.8%) are unchanged from A — these mechanisms are independent of the deletion instruction.

**M-Claude — 11 silenced mutants (unchanged from 2026-06-09 under unification):**
| Attribution | Count | % |
|-------------|-------|---|
| Knowledge-gap | 5 | 45.5% |
| **Sacrifice** | **0** | **0.0%** |
| AOC (assume over-constraint) | 3 | 27.3% |
| Structural/unknown | 3 | 27.3% |

**M-gptoss — 30 silenced mutants (was 27 pre-unification):**
| Attribution | Count | % |
|-------------|-------|---|
| Knowledge-gap | 28 | 93.3% |
| Sacrifice | 0 | 0.0% |
| AOC (assume over-constraint) | 2 | 6.7% |

Like A-gptoss, gptoss120b's residual silencing under M is almost entirely KG (93.3%), confirming SAC=0% is consistent for gptoss across A and M — gptoss's silencing mechanism is capability-limited (KG), not UNKNOWN-pressure-driven (SAC), regardless of bounding hints. (Per-function breakdown for the +3 newly-silenced KG mutants vs. the pre-unification 27-mutant set not yet re-derived; see regenerated `attribution_feedback_loop_M_gptoss120b.json` on server.)

**KEY CAUSAL FINDING — conjunctive driver for SAC (updated 2026-06-10):**
M-Claude has SAC=0% (vs A-Claude's 37.5%) by resolving CBMC UNKNOWN events — the original causal claim. **H-Claude independently also has SAC=0%, despite retaining UNKNOWN events at A's rate (4.3% Sil/GT, identical to A)** — this is a second, independent route to SAC=0. Of the three Claude conditions measured (A/H/M), only A combines *both* the deletion-permitting instruction *and* unresolved UNKNOWN pressure, and only A has SAC>0. H removes the instruction (SAC→0 despite persistent UNKNOWN); M removes the UNKNOWN pressure (SAC→0 despite the instruction remaining). Together these support a **conjunctive causal model**: SAC requires *both* factors; removing either one is sufficient to eliminate it. KG and AOC persist under both H (56.2%/25.0%) and M (45.5%/27.3%), reflecting specification-ignorance and domain-constraint choices independent of CBMC convergence pressure.

#### Oracle: Negative Control (Worst-Performing Condition)

**Reframed 2026-06-10**: "Oracle" is *not* an aspirational ceiling — it is the condition where the LLM is handed \HGT's `__CPROVER_assume` preconditions for free and only has to write its own postconditions. With the setup problem removed, CBMC is satisfiable from iteration 1 and the LLM writes the weakest assertions of any condition ("laziness effect"): 158/370 = **42.7% Sil/GT** (12.8% of all 1,233 mutants) — the **highest of all eight conditions**, consistent with Oracle's lowest RQ1 recall (0.251) and highest pass rate (84.3%). This is a **negative control**: it isolates conformance pressure once the precondition-discovery problem is removed and shows that "free" preconditions induce laziness, not safety.

The LLM conditions (no access to \HGT's preconditions) reach 3.0–11.1% Sil/GT (gptoss: 8.1–11.1%; Claude: 3.0–4.3%), with Claude~A achieving 95.7% recall. The within-gptoss inversion is the cleanest illustration: Oracle has both the highest pass rate (84.3%) *and* the highest Sil/GT (42.7%) of all eight conditions, while Condition~A has a much lower pass rate (62.5%) but far lower Sil/GT (11.1%) — pass rate and safety move in opposite directions (see Attribution Analysis above). We no longer report a "10× gap"; the previously-used "oracle ceiling" framing has been removed throughout paper.tex (abstract, RQ2 key findings, practical implications, limitations, threats, conclusion) — see paper.tex §Methodology "Condition details" for the Oracle condition's formal definition.

**Coverage status (2026-06-10): all 8 CBMC oracle conditions complete and unified on the shared 370-mutant GT-fail set** — see the "Cross-Condition Silence Rate Summary" table above, which now serves as the single canonical results table (the previously-separate "Coverage status (own/own vs canonical-351)" table has been retired; see Data Audit below for the resolution history).

### Replication Narrative (Llama 3.3 70B)

**Cross-verify recall (completed 2026-06-09):** A=32.7%, H=30.4%, G=30.3%

- **A replicates well**: 61.4% vs 62.5% PASS (Δ=1.1pp), recall 32.7% vs 34.6% (Δ=1.9pp). Confirms iterative feedback benefit is model-agnostic.
- **G replicates exactly**: both models achieve 31.3% PASS in single-pass zero-feedback condition; recall 30.3% vs 29.0% (Δ=1.3pp).
- **H gap (17pp)**: Llama achieves only 45.8% PASS vs gptoss 62.7%. Recall 30.4% vs 30.3% — nearly identical despite PASS gap. Llama spends more iterations (avg 4.65 vs 3.53) without recall gain.
- **Recall model ranking (Cond. A)**: Claude (43.1%) > gptoss (41.6%) > llama (32.7%). Stronger models close KG more, but all three show sacrifice under UNKNOWN pressure.
- **Sacrifice is cross-model**: Llama's 71.7% sacrifice ratio (A) and 64.1% (H) confirm active sacrifice is not gptoss-specific, though Llama shows more predicate-weakening relative to deletion — a behavioural difference worth reporting in the RQ1 taxonomy breakdown.

---

## KG Assertion Taxonomy (S2 from Revision Roadmap, as of 2026-06-09)

Manual inspection of knowledge-gap assertions from silenced-mutant attribution reveals four categories. This taxonomy addresses reviewer R2 and R3's request to characterize missing assertions structurally.

### Category 1: Capacity Arithmetic (CA)
Assertions about remaining buffer capacity relative to the size of the operation being performed.

**Example (aws_byte_buf_cat, gptoss A — 20 silenced bugs):**
```c
assert((old_dest.capacity - old_dest.len) >= (buffer1.len + buffer2.len + buffer3.len));
assert((old_dest.capacity - old_dest.len) < (buffer1.len + buffer2.len + buffer3.len));
```

**Why LLMs miss this**: Capacity-arithmetic checks require reasoning about the *difference* between pre-call and post-call state (`old_dest.capacity`, `old_dest.len` captured before the call) against the *aggregate* size of input operations. This requires (a) saving pre-call state and (b) constructing the arithmetic inequality that expresses the capacity precondition. LLMs typically generate the simpler `assert(buf.capacity >= buf.len)` postcondition but miss the *pre-operation capacity sufficiency check*.

**Safety cost**: A mutant that corrupts the capacity check before `aws_byte_buf_cat` executes would silently pass an LLM harness that omits the CA assertion — exactly the scenario that caused the 20-silenced-bug concentration in gptoss A.

### Category 2: Functional Correctness — Return Value (FC-RV)
Assertions that the function's return value correctly reflects its postcondition.

**Example (aws_ptr_eq, gptoss A — 2 silenced bugs):**
```c
assert(rval == (p1 == p2));
```

**Why LLMs miss this**: For simple comparison/predicate functions, LLMs often generate the structural postconditions (pointers not NULL, buffer valid) but forget to assert that the *return value* captures the computed result. `aws_ptr_eq` returns a `bool` — the LLM verifies that the function doesn't crash but not that it returns the right answer.

**Safety cost**: A mutant that inverts the return value (`return !(p1 == p2)`) would pass an LLM harness that doesn't assert `rval == (p1 == p2)`.

### Category 3: Validity Predicates (VP)
AWS-specific `aws_X_is_valid()` macro calls that check structural invariants of the library's data structures.

**Example (aws_byte_buf_cat, aws_ring_buffer_buf_belongs_to_pool — multiple):**
```c
assert(aws_byte_buf_is_valid(&buffer1));
assert(aws_ring_buffer_is_valid(&ring_buf));
```

**Why LLMs miss this**: `aws_byte_buf_is_valid()` is defined in `proof_helpers/` and checks multiple internal invariants (non-NULL if non-zero capacity, len ≤ capacity, allocator consistency). LLMs that don't know these domain-specific macros write instead `assert(buf.len <= buf.capacity)` — partial coverage of the same property. The validity predicate is a compact encoding of multiple invariants; the LLM generates the invariants piecemeal but misses the aggregate check.

**Safety cost**: A mutant that corrupts allocator state while preserving len/capacity would pass an LLM harness that checks only `len ≤ capacity` but is caught by `aws_byte_buf_is_valid`.

### Category 4: Frame Conditions (FC-F)
Assertions that fields unrelated to the operation's purpose remain unchanged after the call.

**Example:**
```c
assert(old_buf.allocator == buf.allocator);   // allocator not modified
assert(ring_buf_old == ring_buf);             // ring buffer metadata unchanged
assert(buf_old == buf);                       // secondary buffer unchanged
```

**Why LLMs miss this**: Frame conditions require (a) saving pre-call state for every field that should be preserved and (b) asserting equality after the call. LLMs rarely save pre-call state comprehensively — they capture only the fields they expect to change. Unchanged fields are not mentioned in the function documentation, so the LLM has no obvious prompt to add frame assertions.

**Safety cost**: A mutant that modifies a field the operation should not touch (e.g., corrupting `allocator` during a write operation) would pass any harness that doesn't check frame conditions.

### Taxonomy Summary

| Category | Example | LLM Failure Mode | Cloze Prediction |
|----------|---------|------------------|-----------------|
| CA — Capacity Arithmetic | `assert(capacity - len >= sum)` | Missing pre-call state capture + arithmetic | 50-70% fill (domain-specific arithmetic) |
| FC-RV — Return Value | `assert(rval == (p1 == p2))` | Misses return value assertion for predicate functions | 80-90% fill (obvious from signature) |
| VP — Validity Predicates | `assert(aws_byte_buf_is_valid(&buf))` | Uses piecemeal checks instead of AWS macros | 40-60% fill (needs knowledge of aws_X_is_valid) |
| FC-F — Frame Conditions | `assert(old_allocator == new_allocator)` | Never saves pre-call state for unchanged fields | 20-40% fill (requires systematic frame reasoning) |

**Implication for cloze test**: If fill rates are high for FC-RV (obvious) but low for FC-F (requires systematic frame reasoning), the root cause is *specification discovery failure* for FC-F (LLM doesn't think to check frame conditions) and potentially *knowledge gap* for VP (LLM doesn't know the AWS validity macros). This distinction has practical implications: FC-RV and CA gaps might be addressable with better prompting (one-shot examples showing return-value assertions); FC-F gaps require different scaffolding (systematic pre-call state capture); VP gaps require domain documentation injection.

---

## Data Audit & Open Issues (2026-06-10)

### UNIFICATION (2026-06-10): the "370 cluster" — all eight conditions now share one GT-fail set

Following the audit below, all eight oracle conditions (Oracle, A/H/M/G-gptoss, A/H/M-Claude) were rerun (or reverified) under the canonical `run_mutation_oracle_cbmc.py` against the identical 1,233-mutant/40-function corpus, including a fresh A-Claude run restricted to the common 40-function subset (excluding `aws_byte_buf_write_u8`). **All eight independently-computed GT-fail sets are per-mutant identical: 370/1,233 mutants, pairwise symmetric difference = 0 across all 28 pairs.** This "370 cluster" is the stable, reproducible ground truth for this corpus/script/GT-harness combination on this server, confirmed 4 times for the Oracle condition alone (370 reproduced on every rerun, even though Oracle's own Silenced/Sil/GT/Recall numbers shifted across reruns: 120/34.2%/45.3% → 158/42.7%/37.6%, reflecting CBMC timeout non-determinism on the *LLM* side, not the GT side).

This unification **fully resolves Issues #1 and #2 below** (the A-gptoss dual-source ambiguity and the own/own-vs-canonical-351 recall-definition ambiguity both disappear once every condition shares one denominator). paper.tex Tables 6 and 7 have been fully rewritten against the unified 370-set (see "Cross-Condition Silence Rate Summary" and "Attribution Analysis" above for the new canonical numbers).

**Before/after summary (Sil/GT, Recall):**

| Condition | Before (pre-unification, mixed denominators) | After (unified, ÷370) |
|-----------|-----------------------------------------------|------------------------|
| Oracle | 120/351=34.2%, recall 45.3% | 158/370=**42.7%**, recall 37.6% |
| A-gptoss | dual-sourced: 26/361=7.2%, *or* 26/178-config, *or* 32/351=9.1% (canonical, unpropagated) | **41/370=11.1%**, recall 42.2% |
| H-gptoss | 23/338=6.8%, recall 48.8% | 37/370=10.0%, recall 47.6% |
| M-gptoss | 27/?=7.7%, recall 68.0% | 30/370=8.1%, recall 64.9% |
| G-gptoss | 1/339=0.3%, recall 44.5% | 1/370=0.3%, recall 45.4% |
| A-Claude | 14/369(own)=3.8%, or 14/351=4.0%, recall 94.3% | **16/370=4.3%**, recall 95.7% |
| H-Claude | 16/351=4.6% (mixed convention), recall 93.2% (345/370) | 16/370=4.3%, recall 93.2% (unchanged) |
| M-Claude | 11/370=3.0–3.1% (already ≈370-based), recall 96.2% | 11/370=3.0%, recall 96.2% (unchanged) |

The Wilcoxon test for "sacrifice instruction irrelevant" (H≈A, gptoss) was recomputed on the unified per-function silence rates (n=31 paired functions): one-sided $p=0.090$ (mean A=0.0529, mean H=0.0403), vs. the previous $p=1.0$ — the **conclusion is unchanged** (n.s., H≈A) but the margin is now more borderline and should be reported as such (paper.tex now says "statistically indistinguishable" rather than "identical").

### Issue #1 — A-gptoss dual-source: RESOLVED (2026-06-10)

A-gptoss had three historically-circulating figures (26/7.2%, 26/178-config, 32/351-canonical-unpropagated). The full rerun under the canonical script against the unified 1,233-mutant corpus produces **41 silenced / 370 GT-fail = 11.1% Sil/GT, recall 42.2%** — this is now the adopted figure in paper.tex Tables 6/7 and the Cross-Condition Silence Rate Summary above. Note this *does* widen the H-vs-A gptoss gap slightly (10.0% vs 11.1%, Δ=1.1pp) relative to the pre-unification 6.8%-vs-7.2% framing, but the Wilcoxon recomputation above confirms the "irrelevant at the safety level" conclusion still holds (p=0.090, n.s.).

### Issue #2 — own/own vs canonical-351 recall definition: FULLY RESOLVED (2026-06-10)

The own/own-vs-canonical ambiguity (and the associated "M improves Claude's recall by +1.9pp under own/own but -4.6pp under canonical-351" reversal risk flagged on 2026-06-09) **no longer exists**: every condition's GT-fail set is now the identical 370-mutant set, so there is only one denominator and one recall figure per condition. Under the unified set, M improves Claude's recall by **+0.5pp (95.7%→96.2%)** — a smaller but unambiguous (single-definition) improvement, and M *also* improves Sil/GT (4.3%→3.0%), strengthening rather than complicating the M-as-actionable-fix narrative. No further author sign-off needed on this point.

### Issue #3 — action-level SacRatio confound (STILL OPEN, unaffected by unification)

**The "action-level SacRatio" finding (A=94.4% / H=81.0% / M=47.8%, all UNK%=3.6%) computed from `iteration_log.json` files is very likely a confound — do not add to the paper as-is.** `SacRatio = (SacDel+SacWkn)/(SacDel+SacWkn+CorrDel+CorrWkn)`, but this Sac+Corr denominator covers only ~70% of all delete/weaken events for A, vs. **~17% for H and ~8% for M** — the rest fall in neither bucket. H/M have roughly 2x A's iter-1 COMPILE_ERROR rate (30%/34% vs 17%), producing many "wholesale harness rewrite" events (`fix_compile` context, `triggered_violation=False`) that dilute the Sac/Corr denominator with unrelated noise. H and M each have 4 functions stuck in 14-15-round non-converging compile-error loops that contribute a large share of these "neither" events. *Recommendation*: either exclude non-converging functions from all three conditions and recompute, or drop this metric for H/M and rely on the oracle-level attribution (Table 7) instead, which uses a methodologically cleaner mutation-oracle signal. This issue is orthogonal to the GT-fail-set unification and remains open.

### M-eliminates-SAC causal claim — now corroborated by a second independent route (H-Claude, 2026-06-10)

The original within-function spot-check (2026-06-10, on the pre-unification 14/11-mutant A/M-Claude sets) found: of A-Claude's 2 SAC-attributed functions, `aws_byte_buf_secure_zero` has its **same 4 mutants** flip from silenced (A) to caught (M) under M — direct within-function causal evidence that resolving UNKNOWN pressure restored detection. `aws_ring_buffer_buf_belongs_to_pool`'s 2 mutants instead flip to COMPILE_ERROR under M (excluded, not "fixed"). M also introduces 5 *new* silenced mutants in 2 functions never silenced under A (`aws_string_eq_byte_cursor`, KG; `aws_string_eq_byte_buf`, AOC) — so the net A→M reduction is a mix of genuine SAC-resolution and compositional churn elsewhere. (A's +2 unification-era additions are both in `aws_byte_buf_cat`, unrelated to these functions, so this spot-check remains valid under the unified 16/11-mutant sets.)

This spot-check is now **independently corroborated at the aggregate level by H-Claude's attribution** (new 2026-06-10): H-Claude's SAC=0% despite retaining UNKNOWN events at A's rate (4.3% Sil/GT, identical to A) shows that resolving UNKNOWN (M's mechanism) is not the *only* way to reach SAC=0 — removing the deletion-permitting instruction (H's mechanism) independently suffices too. Combined, the two routes support a **conjunctive causal model**: SAC requires *both* the deletion-permitting instruction *and* unresolved UNKNOWN pressure; A is the only condition with both, hence the only one with SAC>0. This is now the framing used in paper.tex §4.3/§5 and contribution #7.

---

## Target Venue

**FSE 2027** (submission deadline approximately October 2026). The combination of an industrial corpus with expert ground truth, formal confirmation via bounded model checking, and the controlled per-category feedback strategy comparison in RQ3 positions this as an empirical study with formal backing, which is well suited to the FSE research track. The central finding — that specification gaps in LLM-assisted BMC are structural, safety-relevant, and partially addressable through feedback design but with category-dependent costs — is of direct relevance to practitioners deploying LLMs in formal verification pipelines.

**Fallback: ASE 2027**, if RQ3 results are not stable by early October 2026. No scope reduction is planned under the fallback; the paper submitted to ASE would be identical in scope to the FSE submission. Thesis submission target remains December 2027 under either track.

---

## Scope and Prioritisation for FSE 2027

Given the October 2026 submission deadline, the following scope decisions apply:

**In scope (required for submission):**
- **H_GT oracle framing** (no separate annotation needed): recall scoped as lower bound on safety-relevant content; silenced-mutant count is the primary circularity-free result; RQ2 reports count of distinct H_GT assertions certified by ≥1 silenced mutant
- **Oracle Setup condition**: added to ablation set to separate conformance pressure from setup incapacity (see Design Rationale)
- **RQ1**: full run, 1 primary LLM × conditions A, B, D, F, G, H (core) + I, J, K (ablations); two κ gates; active-sacrifice fraction per category (H_GT-entailed, ever-generated denominator); H vs A manipulation-check interpretation
- **RQ2**: ~1,900 mutants, CBMC mutation oracle (CBMC 5.95.1); assertion-level kill attribution; informative residual decomposition as co-primary result; harness disposition table
- **RQ3**: stratified subset from RQ2, three arms (A / B-strict / B-relaxed); predicate-weakening detection; all-arm vacuity audit; per-category (SR, PR)
- **Drop Pipeline C** (agentic arm): scope trap; increases infrastructure cost without increasing claim strength; reserved for future work

**Ablation conditions I, J, K are necessary hypothesis tests, not optional scope.** Each ablation isolates a specific alternative explanation for the active-sacrifice finding: I tests whether the problem is a knowledge gap (not conformance pressure); J tests whether it is driven by LLM statelessness/forgetting; K tests whether it is a generation-order artifact. The result of each ablation — including null results — must be reported. If a single condition fails to complete before Oct 2026, it is reported as "pending" in the submission, but the paper must pre-specify what result in that condition would refute the conformance-pressure hypothesis. Core RQ1 (conditions A–H) is the minimum viable scope.

**Confirmatory replication (second LLM, conditions A/G/H):** the second LLM (DeepSeek V4 Flash, `deepseek/deepseek-v4-flash` via OpenRouter) runs the three core sacrifice-measurement conditions (A: baseline iterative; G: zero-feedback; H: strategy-neutral) rather than A/B/E. This selection directly replicates the active-sacrifice comparison rather than the prompt-content effect. Run in parallel with RQ1 main analysis; included in submission if active-sacrifice direction replicates; reported as "pending replication" in ASE fallback if not complete.

**Bound-sufficiency pre-study:** before the full RQ1 run, all 108 H_GT harnesses are run against their corresponding original functions under the study's CBMC unwinding configuration. A harness passes the pre-study if CBMC returns UNSAT. Any function where H_GT fails to achieve UNSAT under study bounds is excluded from the main corpus and reported separately. This pre-study ensures that every SAT result the LLM encounters during iterative generation reflects a genuine assertion violation, not a bound-configuration artefact.

**Critical path:** RQ1 iteration logger → RQ1 full run + annotation → RQ2 setup + soundness-parity validation → RQ2 batch run → RQ3 subject selection + pipeline run → writing. The RQ1 annotation is the only manually-gated step; all other transitions are script-driven.

**Trigger for ASE fallback:** if RQ1 annotation is not complete with κ ≥ 0.8 by 2026-08-01, or if RQ3 pipeline runs are not stable by 2026-09-01, shift to ASE 2027. No scope reduction under the fallback — same paper, later deadline.

---

---

## Design Rationale — Key Decisions and Why

This section records the reasoning behind non-obvious design choices so that future collaborators (human or LLM) can understand the intent, not just the specification.

### Why silenced-mutant count is the primary result, not recall

Early versions of the plan used recall against H_GT as the headline metric. A reviewer (correctly) flagged that this creates circularity: "LLMs fail to match H_GT → defined as incomplete." AWS engineers write harnesses for verification tractability, not specification completeness; H_GT may omit properties they judged unnecessary. Using H_GT to define completeness and then measuring LLM alignment against it is circular.

The fix: promote the **silenced-mutant count** (GT SAT / LLM UNSAT per category, with concrete CEX confirmation) as the primary result. This depends only on the mutation oracle, not on H_GT's completeness. Recall is retained as a descriptive lower bound — a useful signal for understanding the gap's structure, but not a completeness claim. RQ2's mutation oracle also serves as validation of H_GT's safety relevance: the subset of H_GT assertions that are empirically certified by silenced mutants is reported explicitly.

### Why three RQ3 arms (A / B-strict / B-relaxed) instead of two

The original two-arm design (Pipeline A: delete, Pipeline B: refine) had a critical flaw discovered by reading the actual `feedback_loop.py` code: the `fix_verification_prompt` explicitly tells the LLM "if assertion too strong: weaken or remove it, **or add `__CPROVER_assume` to constrain the input**." AWS ground-truth harnesses never add `__CPROVER_assume` in response to assertion failures — assumes are structural preconditions set before the function call. So a Pipeline B that prohibits deletion but still allows assume-tightening has the same bug-silencing effect as Pipeline A via a different mechanism.

The three-arm design separates the two mechanisms: A (delete) → B-strict (predicate refinement only, frozen domain) → B-relaxed (refinement + justified assumes). The A→B-strict gap measures the causal effect of deletion prohibition; B-strict→B-relaxed measures how much assume-tightening contributes when permitted.

### Why Pipeline C (agentic arm) was dropped

A ReAct-style agent with persistent memory and CBMC as a callable tool was proposed as an additional arm to test whether "agency" closes the specification gap. This was rejected as scope creep: it doubles infrastructure, invites reviewers to treat the paper as a tool paper (with tool-paper baselines), and the scientific question ("does the phenomenon persist under stronger scaffolding?") can be addressed in future work. The ablation conditions I/J/K (category label, deletion log, spec-first) test specific agency-adjacent hypotheses at much lower cost.

### Why Condition H (strategy-neutral repair prompt) was added

The central claim "LLMs actively sacrifice assertions under conformance pressure" could be reduced to "LLMs follow the repair prompt's instructions." Condition H tests this: it runs the same iterative loop as A but the repair prompt provides no strategy — the LLM receives only the violated assertion and counterexample trace and must decide how to respond without being told it can delete/weaken/add assumes. If active sacrifice occurs at comparable rates under H as under A (after the manipulation check passes), the behavior is emergent in the LLM, not merely instructed. The manipulation check: run both A and H on 20 correct-code functions (where no H_GT-entailed deletion is ever necessary) and verify A produces more gratuitous deletions than H.

### Why the active sacrifice construct is restricted to H_GT-entailed assertions

An early version counted all assertion removals as potential "sacrifices." A reviewer identified that this conflates two different behaviors: (1) removing a correct, H_GT-entailed property under verifier pressure (genuine sacrifice), and (2) removing an incorrect or ill-formed assertion the LLM wrote itself (self-correction). Only (1) is evidence of conformance pressure. The fix: active sacrifice is counted only for assertions that are entailed by H_GT (H_GT contains a corresponding assertion for the same property). Self-corrections have no H_GT counterpart and are excluded.

### Why recall is reported per category with separate denominators

Frame conditions (unchanged-field assertions) are predominantly never-generated — LLMs rarely save old state before the function call, so they rarely generate `assert(to_old.allocator == to.allocator)`. The denominator of the per-category sacrifice fraction must be "H_GT-entailed assertions in category c that were ever generated" (not all H_GT assertions), because an assertion that never appeared cannot be sacrificed. If a pooled fraction is used without this per-category structure, frame conditions (small denominator, low sacrifice rate) and length invariants (larger denominator, higher sacrifice rate) are averaged together in a way that makes the narrative threshold (>30%/<10%) uninterpretable.

### Why the Oracle Setup condition was added (and what it tests)

The Devil's Advocate (round 5) raised that "active sacrifice" is not falsifiable against "incapacity": when a model deletes a H_GT-entailed assertion after a CBMC violation, this is consistent with both (a) knowing the assertion is correct but removing it to achieve UNSAT (conformance pressure), and (b) not knowing how to set up the preconditions correctly and mistakenly treating the postcondition as the problem. The Condition H manipulation check on correct code does not resolve this because it only tests whether the prompt instruction matters on easy code, not whether deletions on hard/buggy code were *avoidable*.

The Oracle Setup condition resolves this by removing setup difficulty as a confound: the correct H_GT preconditions are provided, so the model only needs to write postconditions. If the model still deletes postcondition assertions when CBMC triggers on a mutant, setup difficulty cannot explain the deletion — conformance pressure is the only remaining explanation. If deletions drop, prior rounds' deletions were partly incapacity-driven. This condition is cheap (prompt change only) and runs alongside I/J/K.

### Why the FSE 2027 deadline was chosen over ASE 2027

FSE 2027 deadline ≈ October 2026, giving ~5 months from May 2026. This is tight but feasible for the minimum viable scope (RQ1 conditions A–H + RQ2 silenced-mutant count). RQ3 is a stretch goal. ASE 2027 is the explicit fallback with the same scope (no reduction), triggered if RQ1 annotation κ < 0.8 by Aug 1 2026 or RQ3 pipeline is unstable by Sep 1 2026.

---

## Paper Draft — Abstract and Introduction Skeleton (2026-06-09)

### Draft Abstract (target 250 words)

**(Superseded by paper.tex's abstract, which is the current source of truth — see paper.tex L60-80. The text below is kept for historical reference and has been updated only for numeric consistency with the 2026-06-10 unified 370-mutant set; it predates the 2026-06-10 "Oracle = negative control" reframing and conjunctive-SAC restructuring, both reflected in paper.tex but not back-ported here.)**

Large language models are increasingly used to generate CBMC proof harnesses for production C libraries. When guided by iterative verifier feedback, LLMs produce harnesses that often achieve CBMC SUCCESS — but does *convergence* imply *safety*? We present the first formal study of safety gaps in LLM-assisted bounded model checking, using a circularity-free CBMC mutation oracle on 83 functions from aws-c-common (Amazon's formally verified C common library).

We identify **three mechanistically distinct conformance behaviours** through which LLM harnesses silently miss bugs that expert-written harnesses catch: (1) *knowledge gaps* (KG) — assertions that the LLM never generates, dominating lower-capability models (90.2% of silenced bugs under gptoss-120b); (2) *assertion sacrifice* (SAC) — correct assertions generated and then removed under CBMC UNKNOWN pressure to escape state-space explosion, dominating higher-capability models (37.5% under Claude Sonnet); and (3) *assumption over-constraint* (AOC) — explicit `__CPROVER_assume(x ≤ bound)` bounds that restrict the symbolic state space, excluding bug-triggering inputs even when all assertions are correct.

Across five feedback conditions on 1,233 mutants, we find: the CBMC feedback loop is critical (without it: 0.3% silence rate vs 11.1% with it); the sacrifice-permitting prompt instruction is irrelevant at the safety level (strategy-neutral Condition H achieves 10.0% ≈ 11.1%, p=0.090, n.s.), meaning sacrifice is *emergent* under verifier pressure regardless of prompt wording; and even the best-quality model (Claude: 1% compile error, 95.7% oracle recall) silences only 4.3% of GT-detectable bugs vs 42.7% for the expert oracle — an **≈10× gap**. All three mechanisms are forms of conformance under verifier pressure, not specification ignorance, and together constitute a concrete safety argument against deploying unaudited LLM harnesses in production formal verification pipelines.

---

### Introduction Skeleton

**P1 — Hook (1 para):** AWS runs CBMC on 238 production C functions as part of its CI pipeline. Recent work deploys LLMs to generate the CBMC proof harnesses automatically. When a harness achieves CBMC SUCCESS, the verifier reports: "no bugs found within these bounds." But does LLM SUCCESS mean verified safety, or merely that the harness satisfies CBMC?

**P2 — Problem (1 para):** We call this the *conformance gap*: under iterative CBMC feedback, LLMs learn to make their harnesses pass the verifier — by omitting assertions they cannot specify, deleting assertions when CBMC times out, or narrowing the symbolic input space to avoid state-space explosion. Each strategy produces a harness that CBMC accepts, while silencing bugs that an expert-written harness would catch.

**P3 — Approach (1 para):** We ground this claim using a circularity-free mutation oracle: for each pair (GT harness, LLM harness, mutant), we report a *silenced bug* when CBMC(GT, m) = FAIL and CBMC(LLM, m) = SUCCESS. This certificate makes the safety cost concrete and independently verifiable, without relying on completeness claims about the GT corpus.

**P4 — Contributions (bullets):**
- First formal evidence of three conformance mechanisms (KG/SAC/AOC) in production CBMC harness generation
- Circularity-free CBMC mutation oracle: best LLM condition (Claude A) silences only 4.3% of GT-detectable bugs (95.7% recall, no GT-precondition access), vs 42.7% for the Oracle negative control (highest of all eight conditions, "laziness effect" from free GT preconditions)
- Causal ablation: CBMC feedback loop is critical (G: 0.3%); sacrifice instruction is irrelevant at the safety level (H≈A, p=0.090, n.s.), confirming sacrifice is emergent
- AOC as a new finding: correct assertions + over-constrained inputs = masked bugs
- 41 formally grounded safety failures (gptoss A), 16 (Claude A), all identified by mechanism

**P5 — Paper organisation (1 para):** §2 Background, §3 Study Design, §4 RQ1 Results (taxonomy), §5 RQ2 Results (oracle), §6 Discussion, §7 Related Work, §8 Conclusion.

---

*Last updated: 2026/06/10 (later pass) — FULL UNIFICATION on the "370 cluster": all eight oracle conditions (Oracle, A/H/M/G-gptoss, A/H/M-Claude) reverified to share an identical per-mutant 370/1,233 GT-fail set (28/28 pairwise symdiff=0), eliminating Issue #1 (A-gptoss dual-source, now 41/370=11.1%, recall 42.2%) and Issue #2 (own/own-vs-canonical-351, no longer applicable — single denominator for all). H-Claude attribution completed (16 silenced: KG 56.2%/SAC 0.0%/AOC 25.0%/Unknown 18.8%), giving a second independent SAC=0% route alongside M-Claude and supporting a conjunctive (instruction ∧ unresolved-UNKNOWN) causal model for SAC. H-vs-A gptoss Wilcoxon recomputed on unified set: p=1.0→p=0.090 (n.s., conclusion unchanged but more borderline). paper.tex Tables 6/7 fully rewritten plus all numeric mentions in abstract/intro/contributions/RQ2 prose/conclusion/practical-implications/threats-to-validity updated to match (42.7% oracle ceiling, ≈10× gap, 4.3%/95.7% Claude-A, etc.); paper compiles cleanly (10pp, pdflatex, no errors/undefined refs). Issue #3 (action-level SacRatio confound) remains open and is orthogonal to this unification. M_gptoss attribution updated (30 silenced, 93.3% KG / 6.7% AOC).*

*Last updated: 2026/06/10 (reframing pass) — Two independent reviewer personas (EIC + devil's-advocate, via the academic-paper-reviewer skill) flagged a framing tension: "oracle ceiling (42.7%) / ≈10× gap" reads as an aspirational target LLMs fall short of, but Oracle is empirically the **worst**-performing of all eight conditions (lowest recall 0.251/37.6%, highest Sil/GT 42.7%) — a "laziness effect" from free GT preconditions, already established elsewhere in the abstract ("pass rate is an inverted proxy for specification quality"). Per user decision, reframed Oracle throughout paper.tex as a **negative control** (not a ceiling): removed all "ceiling"/"≈10×"/"gap" language (abstract, intro contributions, RQ2 key findings, practical implications, limitations, threats, conclusion); added a missing "Oracle" condition-details paragraph to Methodology (root cause of the original ambiguity — Table~\ref{tab:conditions} previously described Oracle only as "GT harness used as LLM harness (ceiling)"); restructured the abstract to foreground the conjunctive SAC finding (Conditions H and M independently driving Claude's SAC 37.5%→0%, n=16→0/11→0) ahead of the Oracle/Claude-A comparison; added an explicit Claude-only/small-n hedge to Contribution #7 and a new Limitations paragraph on cross-model generalizability (gptoss has SAC=0% under all measured conditions, so provides no SAC to disentangle). The "10×-vs-34–36%-adjusted" inconsistency (DA Issue #5) is now moot since "10×" is removed entirely. Also fixed an unrelated but blocking compile regression: pdflatex failed with a fatal "font expansion: auto expansion is only possible with scalable fonts" error during page shipout (acmart's internal `\RequirePackage{microtype}` enables font expansion regardless of the user's own `\usepackage{microtype}` line, and a font-shape substitution on the RQ1/Limitations pages triggered it); fixed via `\microtypesetup{expansion=false}` after `\documentclass`. paper.tex recompiles cleanly (pdflatex ×3 + bibtex, 10pp, exit 0, no undefined refs) — see paper.pdf.*

---

## Follow-up Phase: Generalization, Robustness, Mechanism (2026-06-13 → 2026-06-15)

Since 2026-06-10 the paper grew 10pp → **17pp** and was repositioned around the **circularity-free differential oracle as a reusable audit tool** and the **"knowledge gaps, not gaming" reversal**. Five follow-up experiments + an external no-memory review panel are done. All numbers map to `paper_numbers.py` (server): **177 entries, 0 mismatch**. paper.tex is on Overleaf (origin/master), all changes pushed.

### F1 — s2n-tls cross-corpus replication (DONE)
25 `s2n_stuffer` functions, 650 mutants, GT-FAIL set 253. Gap **replicates and is larger**: Claude-A 57/253 = 22.5% Sil/GT, gpt-oss-A 42/253 = 16.6% (> aws 4.3%, 11.1%). Mechanism **100% KG both models** (57/57, 42/42), SAC=0, AOC=0, spread over 9/5 functions — defuses the `aws_byte_buf_cat` concentration concern. Scripts `s2n_{gen_mutants,dataset,oracle,attrib}.py`. → §sec:s2n + Threats upgrade.

### F2 — Reverse cell (DONE)
Oracle run symmetrically proves expert GT **incomplete**: `string_eq`/`byte_cursor_eq` GTs assert validity + byte-match but omit the return-value postcondition (51/81 reverse mutants). Validates the oracle's non-circularity premise. → §sec:reverse.

### F3 — Cloze test (DONE)
Blank one expert assertion, CBMC-score the model's fill. **33/34** correct → gap is **generative, not (purely) epistemic** (causal wording softened per review). → §sec:cloze.

### F4 — Mutation-guided repair B2 (DONE)
Feed the silenced mutant back: **Claude recovers 69%**, **gpt-oss 0%** → generative and model-dependent. → §sec:discussion.

### F5 — Multi-run variability + pinned reproducibility (DONE)
Counts swing (gpt-oss A 41/34/36, M 30/37/45, G 1/40/43, H 37/40/5; Oracle 158/168) but mechanism stays **KG-dominated 89–100% every run**. Low H=5 validated as genuinely stronger (fewest CE 109, most caught 256), not degenerate. **Pinned re-run** (DeepInfra/bf16, fallbacks off): A → 21/370, **KG 100%**; phenomenon survives, only the rate is precision-sensitive (5.7% vs 11.1%). `call_openrouter_api.py` gained `OPENROUTER_PIN_PROVIDER`/`_QUANT`.

### External ARS no-memory panel review (2026-06-14) — Major Revision (accept-leaning)
EIC + Methodology + Domain + Perspective + Devil's-Advocate. None dispute core findings; fire on attribution rigour + framing/external-validity. Roadmap: **`REVISION_ROADMAP_ars_followup.md`** (C1–C10). Done:
- **P0 framing** — softened cloze; reframed Background (gaming = popular hypothesis the data refute; KG present even single-pass G); demoted "scaffolding-first" to hypothesis; Clopper–Pearson upper bounds on SAC zeros (0/41 ≤ 8.6%, 0/16 ≤ 20.6%, s2n 0/57 ≤ 6.3%) + H-vs-A MDE caveat.
- **C1 behavioural rename-immune KG** (`scripts/behavioural_kg.py`) — CBMC on every iteration; KG unless an iteration verifies on the original AND fails on the mutant. **111/115 = 96.5% KG**, per-cond 90.2–100%; genuine sacrifice only 4/115 (all gpt-oss A). Rebuts the DA CRITICAL. 5th cross-validation leg.
- **(D) Sil/GT-among-compilable** — ordering unchanged/sharper (Oracle 53.2%, gpt-oss A 20.8%, M 11.1%, Claude ~4%); gpt-oss all-mutant figures are **conservative**. `recall` → `assertion recall` at the RQ1 inversion.
- **Related work** — integrated **Helff et al. 2026** (RLVR reward hacking): gaming is RLVR-specific / absent in non-RLVR models, which **explains** why gaming is rare in our inference-time setting. Complementary, not contradictory.

### IN PROGRESS
- **(A) Pinned RQ1 re-run, 8 gpt-oss conditions** (G/H/M/I/J/K/Oracle pinned bf16; A_pin done) — generating (`run_pin_rq1.sh`, watcher active). Confirms whether the pass-rate↔assertion-recall inversion (ρ=−0.26, computed over these 8 mixed-precision conditions) survives fixed precision = load-bearing RQ1 reproducibility fix. Next: `analyze_rq1.py` on pinned datasets, compare ρ.

### OPEN / PENDING DECISION
- **(C) scaffolding-first minimal eval** — PENDING go/no-go. If done: a genuine two-phase condition N (emit snapshot+frame structure, then fill), reported honestly even if negative. Converts the headline takeaway from hypothesis → preliminary evidence.
- **(C4) third verifier / non-AWS corpus** (ESBMC or non-AWS C) — hardest external-validity item; both corpora share the AWS proof-helper idiom.
- **(C5b) retitle / rename "conformance gap"** — judgment call (title vs the KG finding).
- **Pure writing (no new data):** abstract slimming (conclusions first, move p-values/ranges out, keep one SAC self-correction sentence); run-to-run number-wall → table; 3 data figures (pass-rate/recall scatter; run-to-run interval plot; mechanism stacked bar); §4 roadmap sentence.
- **C8** report CBMC unwind bounds; **C10** missing refs (s2n CBMC-CI, TCE) + define "vacuity check" + audit-cost figure.

*Last updated: 2026/06/15 — follow-up phase: F1–F5 + ARS panel review + P0/C1/(D)/Helff fixes done (paper 17pp, registry 177/0, pushed to Overleaf); (A) pinned RQ1 re-run generating; (C) scaffolding-first awaiting decision.*
