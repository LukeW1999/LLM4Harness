# Manual adjudication of attribution_v2 (construct validity)

**Method (honest framing)**: the rater here is the agent applying *expert judgment to raw
evidence* (GT catching assertion; whether the LLM wrote it exactly in any iteration; the
iteration it was last present and the CBMC verdict there; the assume-envelope comparison),
**blind to the v2 automated label while judging**. This is NOT an independent human rater —
the author (Luke) should spot-check the disagreements + a sample of agreements before this
is cited. Agreement = (agent expert label == v2 rule label).

Decision rule under audit (v2): SAC = GT assertion written EXACTLY in iter k, CBMC verdict
at k is UNKNOWN, absent in final. KG = catching GT assertion never written exactly, or
removed under FAIL (self-correction). AOC = all GT assertions present in final + assume
over-constrained. Priority SAC > KG > AOC > Unresolved.

## Adjudicated groups (all silenced (condition, function) groups; n=28 groups)

| # | cond/func | xN | key evidence | expert label | v2 label | agree |
|---|---|---|---|---|---|---|
| 1 | A-gptoss/byte_buf_cat | 33 | CA capacity assert never_exact | KG | KG | ✓ |
| 2 | A-gptoss/clean_up | 4 | all 4 GT in_final; assume cap<=MAX | AOC | AOC | ✓ |
| 3 | A-gptoss/nospec_mask | 2 | bound+rval never_exact | KG | KG | ✓ |
| 4 | A-gptoss/ptr_eq | 2 | rval==(p1==p2) never_exact | KG | KG | ✓ |
| 5 | M-gptoss/byte_buf_advance | 15 | catching assert never_exact; output.* removed under FAIL | KG | KG | ✓ |
| 6 | M-gptoss/byte_buf_cat | 4 | CA never_exact | KG | KG | ✓ |
| 7 | M-gptoss/byte_buf_eq_c_str | 1 | len assert never_exact (AOC also fires) | KG (borderline) | KG | ✓ |
| 8 | M-gptoss/byte_buf_from_c_str | 3 | all never_exact | KG | KG | ✓ |
| 9 | M-gptoss/byte_buf_init_copy | 1 | catching never_exact | KG | KG | ✓ |
| 10 | M-gptoss/nospec_mask | 3 | never_exact | KG | KG | ✓ |
| 11 | M-gptoss/ptr_eq | 2 | rval never_exact | KG | KG | ✓ |
| 12 | **M-gptoss/string_destroy** | 1 | **NO GT asserts parsed**; 8×CE→SUCCESS; AOC fired | **UNRESOLVED** | **AOC** | **✗** |
| 13 | A-DeepSeek/clean_up_secure | 1 | allocator==NULL never_exact | KG | KG | ✓ |
| 14 | A-DeepSeek/from_array | 2 | all never_exact | KG | KG | ✓ |
| 15 | A-DeepSeek/nospec_mask | 1 | never_exact | KG | KG | ✓ |
| 16 | A-DeepSeek/ptr_eq | 1 | rval never_exact | KG | KG | ✓ |
| 17 | A-Claude/byte_buf_cat | 3 | CA never_exact | KG | KG | ✓ |
| 18 | A-Claude/from_array | 2 | len/cap==length never_exact (AOC also fires) | KG (borderline) | KG | ✓ |
| 19 | A-Claude/from_empty_array | 2 | capacity==capacity never_exact (AOC also fires) | KG (borderline) | KG | ✓ |
| 20 | A-Claude/reset | 3 | frame ==old never_exact | KG | KG | ✓ |
| 21 | **A-Claude/secure_zero** | 4 | **buf.len==0 NEVER written (LLM wrote buf.len==old, wrong)** | **KG** | **KG** | ✓ |
| 22 | **A-Claude/ring_buffer_belongs** | 2 | **is_valid(&buf) exact in iter1(UNKNOWN), removed** | **SAC** | **SAC** | ✓ |
| 23 | H-Claude/byte_buf_cat | 9 | CA never_exact | KG | KG | ✓ |
| 24 | H-Claude/byte_buf_eq_c_str | 1 | len never_exact | KG | KG | ✓ |
| 25 | H-Claude/from_empty_array | 2 | all 4 GT in_final; assume cap<=MAX | AOC | AOC | ✓ |
| 26 | H-Claude/reset | 3 | frame ==old never_exact | KG | KG | ✓ |
| 27 | H-Claude/string_destroy | 1 | NO GT asserts parsed; AOC fired | UNRESOLVED | AOC | ✗ |
| 28 | M-Claude (5 funcs) | 11 | catching asserts never_exact (cat/from_empty/reset/eq_byte_buf/eq_byte_cursor) | KG | KG | ✓ |

## Result
- **Agreement: 26/28 groups (92.9%).**
- **The two headline-divergence cases are both VALIDATED**: secure_zero (v2=KG, correcting the old Jaccard SAC false-positive — agent agrees KG, since the LLM never wrote `buf.len==0`, it wrote the semantically-opposite `buf.len==old_buf.len`); ring_buffer (v2=SAC, genuine — exact `aws_byte_buf_is_valid(&buf)` present under UNKNOWN then removed). The SAC/KG boundary (the headline) is 100% concordant on the contested cases.

## Disagreements (both same root → one detector fix)
- **#12, #27 string_destroy (AOC vs Unresolved)**: v2 labels AOC but `get_gt_asserts` returned NO assertions for these functions, so "all GT assertions present in final" is vacuously true and AOC fires on an empty set. **Detector fix (before paper)**: AOC must require that GT assertions were actually parsed AND are present in final; if no GT assertions parsed, route to Unresolved (and investigate why the GT harness parsed empty — likely an assertion-extraction gap for string_destroy's macro-based checks).

## Judgment-dependent (agree with v2 but flag)
- **#7, #18, #19 (KG vs AOC borderline)**: these functions BOTH lack a catching assertion (never_exact) AND over-constrain (AOC fires). v2's explicit priority KG>AOC routes them to KG. This is defensible (the missing concrete assertion is the proximate cause) but a reviewer could argue AOC. **Document the priority rule + these borderline cases explicitly in the construct-validity paragraph.** A more precise resolution would need per-mutant catching-assertion linkage (gt_fail_properties data) to decide whether the silencing is due to the missing assertion or the over-constraint.

## Actions before paper
1. Fix AOC detector: require GT-asserts-parsed & present (routes string_destroy to Unresolved); re-run all conditions; the corrected AOC counts go into Table 7.
2. Luke spot-checks: the 2 disagreements + ~5 agreements (incl. secure_zero, ring_buffer) → upgrade "agent expert adjudication" to "two-rater agreement" for the paper.
3. Cite this file's agreement (≈93%, SAC/KG boundary 100%) in §construct-validity.
