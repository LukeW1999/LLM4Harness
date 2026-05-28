# Research Plan

## Title

**Do LLMs Verify or Conform? A Formal Study of Harness Quality in LLM-Assisted Bounded Model Checking**

---

## Central Hypothesis

When LLMs generate CBMC proof harnesses under iterative verifier feedback, they are driven by a conformance objective — producing a harness that satisfies the verifier on the given code — rather than a specification objective — producing a harness that completely characterises function behaviour. This conformance pressure creates a systematic gap between verification success and specification completeness. The gap is structural, not incidental: it concentrates in tool-idiomatic assertion categories that the LLM cannot satisfy for the code under test, and it causes real bugs to go undetected when the code under test is incorrect.

---

## Research Questions

**RQ1.** What properties do LLM-generated harnesses systematically fail to specify, and do these omissions arise from the LLM never generating the property (*knowledge gaps*) or from the LLM generating and then removing it to achieve UNSAT (*active sacrifices*)?

**RQ2.** Do the specification gaps identified in RQ1 correspond to real verification failures on functionally incorrect code — that is, are the missed assertions precisely the ones that would have caught injected bugs?

**RQ3.** Does the feedback strategy given to the LLM during iterative harness generation determine whether bugs are silenced or detected, and does this effect vary across assertion categories in ways that expose the boundary of LLM refinement capability?

---

## Corpus and Setup

The study uses two production C libraries from Amazon's formal verification programme: aws-c-common (83 functions across 7 data-structure families) and the `s2n_stuffer` module of s2n-tls (25 functions), giving 108 functions in total. Amazon's formal verification engineers have written 238 proof harnesses for these functions as part of a continuously maintained verification pipeline; these constitute the expert ground truth corpus $\mathcal{H}_\mathrm{GT}$. The corpus has two properties that make it suitable as a research substrate: the underlying codebase is production-correct and formally verified against $\mathcal{H}_\mathrm{GT}$, and the harnesses are written by engineers with deep knowledge of both the library internals and the CBMC toolchain, making them the strongest available oracle for what a complete harness should assert.

Two LLMs generate harnesses under six prompt conditions: (A) source code only; (B) source code with natural language documentation; (C) source code with chain-of-thought elicitation; (D) source code with documentation and chain-of-thought; (E) source code with a same-family harness as a one-shot example; (F) source code with a wrong-family harness as an ablation control. Conditions E and F isolate the effect of in-context exemplar family match, motivated by the pilot finding that same-family examples improve recall by 9 pp ($p < 0.04$) while wrong-family examples do not.

All harnesses are generated and evaluated under CBMC, which is the tool used natively by AWS. Compatible harnesses are subsequently migrated to ESBMC to provide a second verification backend; migration is verified by confirming that ESBMC($\mathcal{H}_\mathrm{GT}$, $f$) remains UNSAT after migration. Incompatible cases are retained under CBMC and reported separately as a validity threat.

---

## RQ1: Specification Gaps and Their Origin

### Goal

Characterise what LLM-generated harnesses miss relative to expert harnesses, quantify the gap, and determine whether each omission is a knowledge gap or an active sacrifice. The distinction matters because knowledge gaps indicate that prompting or in-context information could in principle close the gap, whereas active sacrifices indicate that the LLM understood the property was required but discarded it under conformance pressure — a problem that prompting alone cannot solve.

### Iterative Generation Protocol

LLMs generate harnesses under each of the six prompt conditions in a CBMC feedback loop. The iteration proceeds as follows. If compilation fails, the LLM receives the compiler error and retries. If CBMC returns SAT, the LLM receives the full verifier output, including the specific assertion that triggered the violation and the counterexample trace, and modifies the harness to eliminate the violation before retrying. If CBMC returns UNSAT, iteration stops. Each assertion that is removed or weakened during this process is recorded along with the prompt condition and iteration number at which the removal occurred.

### Taxonomy and Classification

Final UNSAT harnesses are matched against $\mathcal{H}_\mathrm{GT}$ assertion by assertion. Missed assertions are classified into three categories derived from the structural idiom of CBMC proof harnesses for memory-safe C libraries. A *validity predicate* covers pointer non-nullness, return value range checks, and error-code postconditions. A *length invariant* covers relationships between buffer lengths, capacities, and offsets that must hold after a call. A *frame condition* covers the assigns clause, specifying which memory locations the function is permitted to modify and asserting that unspecified locations are unchanged.

This taxonomy is not post-hoc: it is grounded in the CBMC Dynamic Frame Condition Checking (DFCC) mechanism used by AWS, which makes frame conditions first-class verification objects. The three categories correspond to distinct kinds of specification knowledge — memory safety, data structure invariants, and side-effect discipline — that a complete harness must capture.

Taxonomy reliability is established before full annotation proceeds. Two independent raters annotate 30 functions drawn from both libraries. Full annotation begins only if inter-rater Cohen's $\kappa \geq 0.8$; disagreements are resolved by adjudication and used to refine the codebook.

Each missed assertion is then classified as either a knowledge gap (never appeared in any iteration) or an active sacrifice (appeared in at least one iteration but was removed to achieve UNSAT). This classification is derived from the iteration log without further human judgement.

### Metrics

Postcondition recall = $|\mathcal{H}_\mathrm{LLM} \cap \mathcal{H}_\mathrm{GT}| \,/\, |\mathcal{H}_\mathrm{GT}|$, reported overall and per category. Per-category miss rate is reported separately for knowledge gaps and active sacrifices. The effect of prompt condition on recall is reported as $\Delta$ recall in percentage points with 95% confidence intervals. Pass rate is reported alongside recall for each condition to establish the baseline divergence between the two metrics.

---

## RQ2: Formal Confirmation of Verification Failures

### Goal

Determine whether the assertions that $\mathcal{H}_\mathrm{LLM}$ misses are precisely those that would have caught real functional deviations. RQ2 upgrades the gap characterisation in RQ1 from a completeness deficit against a reference harness to a safety deficit against real bugs, using bounded model checking as a formal oracle rather than random sampling.

### Dependency on RQ1

RQ2 uses the taxonomy established in RQ1 to classify mutants after the fact. Mutants are generated without category pre-filtering; the assertion triggered by the BMC counterexample determines which category the miss falls into. This preserves classification objectivity and avoids circularity.

### Mutation and Oracle Protocol

Universalmutator is applied to each of the 108 functions to generate approximately 1,900 compiled mutants. For each mutant $m$, two ESBMC invocations run in parallel: ESBMC($\mathcal{H}_\mathrm{GT}$, $m$) and ESBMC($\mathcal{H}_\mathrm{LLM}$, $m$). A SAT result with a counterexample is a bounded proof that the mutant violates at least one assertion in the harness under all inputs within the unrolling bound — not a sampled witness, but a bounded certificate of functional deviation. The primary analysis object is the case where ESBMC($\mathcal{H}_\mathrm{GT}$, $m$) returns SAT (the expert harness confirms the bug) and ESBMC($\mathcal{H}_\mathrm{LLM}$, $m$) returns UNSAT (the LLM harness silences it). The assertion named in the $\mathcal{H}_\mathrm{GT}$ counterexample is extracted automatically and used to assign the mutant to a taxonomy category.

The four outcome combinations are interpreted as follows. GT SAT / LLM SAT: both harnesses detect the bug; $\mathcal{H}_\mathrm{LLM}$ is adequate for this mutant class. GT SAT / LLM UNSAT: $\mathcal{H}_\mathrm{GT}$ formally confirms the bug; $\mathcal{H}_\mathrm{LLM}$ silences it; this is the primary finding. GT UNSAT / LLM SAT: $\mathcal{H}_\mathrm{LLM}$ detects a deviation that $\mathcal{H}_\mathrm{GT}$ does not; this is an independently reportable finding, expected to arise for functions with rich inline documentation under prompt condition B or D. GT UNSAT / LLM UNSAT: the mutant falls outside both harnesses' coverage; discarded with count reported as a validity bound.

### Metrics

Per-category kill rate for $\mathcal{H}_\mathrm{LLM}$ relative to $\mathcal{H}_\mathrm{GT}$, computed over the set of mutants for which $\mathcal{H}_\mathrm{GT}$ returns SAT. Cross-referencing per-category kill rate against per-category recall from RQ1 tests whether the taxonomy predicts verification failure: if recall and kill rate are correlated per category, the taxonomy is a reliable diagnostic of safety-relevant gaps. The distribution of GT UNSAT / LLM SAT cases is reported separately, including the function and category characteristics that predict $\mathcal{H}_\mathrm{LLM}$ strength.

---

## RQ3: Feedback Strategy and the Cost of Refinement

### Goal

RQ1 establishes that active sacrifices — assertions removed to achieve UNSAT — are a distinct and significant source of specification gaps. RQ3 tests whether the feedback strategy given to the LLM is the cause of these sacrifices, by comparing deletion-based feedback (which permits assertion removal) against refinement-based feedback (which prohibits it). The key output is not simply whether refine beats delete — that is expected — but *where* refinement is low-cost and *where* it strains LLM capability, characterised per assertion category. This per-category breakdown is the novel contribution of RQ3: it identifies which kinds of specification the LLM can reliably tighten under counterexample guidance and which kinds it cannot, independent of whether the harness passes the verifier.

### Subject Selection

RQ3 operates on a stratified subset of the RQ2 confirmed bugs (GT SAT / LLM UNSAT cases), sampled to achieve balanced representation across the three taxonomy categories. Assertion-category labels from RQ1 are reused directly; no re-annotation is required. The same LLM is used throughout RQ3 to hold model capability constant; the feedback protocol is the sole independent variable.

### Two Pipelines

Both pipelines receive the same mutant function $f_\mathrm{buggy}$, presented to the LLM as the correct implementation. The LLM is not informed that the function is a mutant.

**Pipeline A — Delete.** The LLM receives full CBMC output after each iteration. On SAT, the LLM may respond by removing or weakening the violated assertion. Iteration continues until UNSAT. This replicates the default behaviour observed in RQ1 and is expected to produce harnesses that silence bugs, because any assertion that $f_\mathrm{buggy}$ violates will be eliminated to achieve UNSAT.

**Pipeline B — Refine.** On SAT, the LLM receives the full counterexample, including the concrete input assignment, the violated assertion, and the assertion category. The LLM is instructed to analyse the counterexample and refine the assertion to be more precise without deleting it. The LLM may additionally tighten `__CBMC_assume` constraints to exclude the counterexample input if it judges that input to be outside the function's valid domain, but must provide an explicit justification for any such exclusion. Iteration continues until UNSAT.

### Confirmation

After both pipelines complete, $f_\mathrm{original}$ is introduced for the first time. CBMC($\mathcal{H}_\mathrm{buggy\_A}$, $f_\mathrm{original}$) and CBMC($\mathcal{H}_\mathrm{buggy\_B}$, $f_\mathrm{original}$) are run. A SAT result means the harness contains an assertion that distinguishes the mutant from the original. To confirm the detection is genuine rather than an artefact of overly specific refinement, the counterexample concrete input $I$ is executed against both $f_\mathrm{original}$ and $f_\mathrm{buggy}$: a real detection requires $f_\mathrm{original}(I) \neq f_\mathrm{buggy}(I)$ on a semantically meaningful output. An UNSAT result means the bug has been silenced.

### Metrics

Bug silencing rate per pipeline and per assertion category: the proportion of subjects for which CBMC($\mathcal{H}_\mathrm{buggy}$, $f_\mathrm{original}$) returns UNSAT. Pass rate per pipeline and per category: the proportion of subjects for which the pipeline achieves UNSAT on $f_\mathrm{buggy}$ within the iteration budget. The joint distribution of silencing rate and pass rate per category is the primary result: it characterises the tradeoff between verification conformance and specification completeness as a function of assertion type, and identifies which categories support low-cost refinement (high pass rate maintained, silencing rate reduced) versus which categories force a tradeoff (pass rate degraded as refine constraint is enforced).

---

## Threats to Validity

**Internal validity.** UNSAT in RQ2 conflates two causes: semantic equivalence between the mutant and the original, and mutant inputs falling outside the harness's `assume` envelope. These are not distinguished at the ESBMC level; all GT UNSAT / LLM UNSAT cases are discarded and their proportion is reported as a lower-bound caveat on kill rate estimates. In RQ3, the concrete execution confirmation step directly addresses the risk that a Pipeline B SAT result is a false positive arising from over-specific assertion refinement rather than genuine bug detection.

**External validity.** The corpus is limited to aws-c-common and s2n-tls. These are safety-critical C libraries subject to rigorous continuous formal verification, representative of the domain where BMC is most commonly practised at scale, but results may not generalise to other C software or to languages with different memory models. The two-library design partially mitigates this by covering distinct functional domains (data structures versus TLS protocol handling).

**Construct validity.** The taxonomy categories are manually defined and require human judgement for boundary cases. Inter-rater $\kappa$ is reported with a minimum acceptance threshold of 0.8 before full annotation proceeds. In RQ2 and RQ3, classification via the triggered assertion in the BMC counterexample replaces human annotation for the majority of cases, reducing subjective influence on the primary quantitative results.

**Conclusion validity.** Results are reported per model, per prompt condition, and per assertion category rather than as single aggregates, to prevent averaging effects from obscuring category-level differences. The Pipeline A versus Pipeline B comparison in RQ3 controls for LLM capability by construction; any observed difference in silencing rate and pass rate is attributable to the feedback protocol alone.

---

## Relationship to Prior Work

Existing studies of LLM-generated specifications — including Clover, AssertLLM, and Patil et al. — evaluate quality primarily through pass-rate metrics: the proportion of generated specifications that a verifier accepts on the original code. This metric conflates specification completeness with specification conformance. A harness achieves pass rate 1.0 trivially by containing no assertions; a harness achieves high recall only by asserting properties that actually characterise function behaviour. The pilot data for this study shows a pass rate of 94% against a recall of 47%, confirming that the two metrics diverge substantially in practice. LLM4Harness grounds evaluation in recall against expert ground truth and in kill rate against formal mutation oracles, moving the field from measuring whether LLMs can satisfy a verifier to measuring whether LLMs can specify a function.

The use of BMC as a mutation oracle rather than dynamic testing is a deliberate methodological choice. A SAT result from ESBMC or CBMC on a mutant is a bounded proof of violation — it certifies the existence of a concrete input that distinguishes the mutant from the harness's specification within the unrolling bound. This provides stronger evidence than a sampled failing test, and aligns the confirmation mechanism with the verification mechanism used in production.

---

## Target Venue

**FSE 2027** (submission deadline approximately October 2026). The combination of an industrial corpus with expert ground truth, formal confirmation via bounded model checking, and the controlled per-category feedback strategy comparison in RQ3 positions this as an empirical study with formal backing, which is well suited to the FSE research track. The central finding — that specification gaps in LLM-assisted BMC are structural, safety-relevant, and partially addressable through feedback design but with category-dependent costs — is of direct relevance to practitioners deploying LLMs in formal verification pipelines.

**Fallback: ASE 2027**, if RQ3 results are not stable by early October 2026. No scope reduction is planned under the fallback; the paper submitted to ASE would be identical in scope to the FSE submission. Thesis submission target remains December 2027 under either track.

---

*Last updated: 2026/05/28*
