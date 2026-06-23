# Green-field auditing PoC

Does the differential oracle still work **without an expert harness**?

## Question
The certified silenced-bug test needs a fail-side reference:
`CBMC(H_GT, m)=FAIL ∧ CBMC(H_LLM, m)=SUCCESS`. The reference `H_GT` is the
expert harness, so the tool only serves *regression* auditing (teams that
already have expert harnesses), not the *green-field* case (new code, no
ground truth). This PoC tests whether a **self-generated** reference can
replace the expert one while keeping every certificate formal.

## Method (`scripts/gf_reference.py`)
The certificate's soundness comes only from the reference assertion being a
true property of the original function, **not** from the reference being an
"expert". So any harness `H` qualifies if:
1. **Fidelity gate:** `CBMC(H, original) = SUCCESS` (H is a valid spec), and
2. `CBMC(H, m) = FAIL` (H separates m from the original).
Both ⇒ `m` is a sound, witnessed in-bound counterexample — a formal
certificate, regardless of where `H` came from. This is **not** a vote: a
single fidelity-passed member firing is one independent certificate.

We build the reference `R` = union of fidelity-passed candidates from a pool
of independently generated **green-field** harnesses (condition A: source +
NL only, no expert harness, no preconditions). `R` catches `m` iff any member
catches it. We measure how much of the expert **GT-FAIL set** (377 (func,
mutant) pairs the expert harness kills, over 32 functions) `R` recovers.

Pool: 14 independent generators across **8 distinct models** (Claude ×5 seeds,
gpt-oss-120b ×3, DeepSeek-V4-flash/pro, gpt-5.5, Llama-3.3-70B, Qwen ×2).
3,731 candidate×GT-FAIL-mutant CBMC runs; fidelity gate 320/410 pass.

## Results (`evaluation/gf_reference.json`)
- **Full pool (K=14): 375/377 = 99.5%** reference-recall (measurable = all 377;
  no denominator shrinkage).
- **recall vs K:** single harness 88.6% → K=5 98.0% → saturates ≈99.5% by K≈13.
- **Diversity is the lever:** matched K=8, **cross-model 99.3% vs single-model
  91.0%**. One model (even with extra seeds) plateaus at ~91%; crossing models
  is what closes 91→99.5. ⇒ LLM blind spots are **partially independent** —
  different models miss different bugs.

## Leave-one-out peer audit (`scripts/gf_audit.py`, `evaluation/gf_audit.json`)
Removes the expert from the *measurement* too. For each harness `h`: hold it out,
build `R` from the other 13 fidelity-passed harnesses, and ask whether `R` flags
`h`'s **own** silenced bugs (GT-FAIL mutants `h` lets pass). No expert anywhere.
- **352/370 = 95.1%** micro audit-recall (macro 94.3%); every one of the 14
  hold-outs is covered **88–100%** by its peers — no harness is a blind spot the
  others cannot audit.
- **Residual = 2 mutants** that *no* pool member catches (same 2 the full pool
  misses vs the expert): `aws_byte_buf_cat/mutant_0061` and
  `aws_ring_buffer_buf_belongs_to_pool/mutant_0013`.
- Heavy catch matrix dumped to `evaluation/catch_matrix.json` for reuse
  (`--matrix` skips the 3,731 CBMC runs).

## Honest scope (do NOT overclaim)
- The single-harness 88.6% ≈ `1 − Sil/GT` — it just restates the paper's low
  per-condition silence rate, so it is **not new**. The new content is (a)
  pooling + cross-model diversity close the residual, (b) the fidelity gate keeps
  every certificate formally sound with **zero expert input**, and (c) the LOO
  audit shows a self-generated reference audits a *peer* (95.1%), not just the
  expert benchmark.
- This is a proof-of-concept that green-field auditing is *viable* on one corpus,
  not a claim that green-field is solved.

Run:
- `python3 scripts/gf_reference.py --exclude feedback_loop_A_v3,feedback_loop_A_gptoss120b_pin --timeout 90 --workers 8`
- `python3 scripts/gf_audit.py     --exclude feedback_loop_A_v3,feedback_loop_A_gptoss120b_pin --timeout 90 --workers 8`
