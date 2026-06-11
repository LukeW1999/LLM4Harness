# Pre-registration: SAC capability-threshold & conjunction generalization

**Committed before any screening result is known.** Purpose: convert the cross-model
SAC experiment from exploratory to confirmatory. Timestamp = git commit time.

## Established facts (prior to this experiment)
- SAC (assertion sacrifice) under condition A: Claude Sonnet 4.6 = 37.5% (6/16); gpt-oss-120b = 0%; DeepSeek-V4-Flash = 0%.
- Conjunctive mechanism (Claude only): H (remove deletion instruction) → SAC 0/16; M (resolve UNKNOWN) → SAC 0/11. A is the only Claude condition with both factors and SAC>0.
- Open question reviewers name: the conjunction rests on a single model (Claude), n=16→0/11→0.

## Screening design (running)
- Models: deepseek-v4-pro, qwen3.7-plus, qwen3.7-max, openai/gpt-5.5 (4 models, via OpenRouter).
- Conditions A/G/H, ALL 83 functions (not a narrow subset → no model-specific screening bias).
- Same 83 functions / same mutants → canonical 370 GT-FAIL denominator (model-independent), directly comparable to existing conditions. NO mutant-corpus expansion → no two-table problem.
- SAC signature (machine-checkable, per function): iteration k harness contains a GT assertion; CBMC returns UNKNOWN at iter k; that assertion is absent at iter k+1.

## Predictions (registered)
1. **Cross-family existence**: at least one non-Claude model among the 4 shows SAC>0 under A. (If none → threshold is higher than all 4 frontier models; this is a publishable negative result strengthening the threshold narrative, NOT a failure.)
2. **Qwen within-family threshold**: if SAC appears in the Qwen family, it appears in qwen3.7-max but not qwen3.7-plus (same training pipeline, pure capability difference → cleanest threshold datapoint).
3. **Highest SAC prior**: gpt-5.5 is the most likely single model to show SAC>0 (strongest general capability + cross-family).
4. **Conjunction**: for any model with SAC>0 under A, both knockouts independently drive SAC→0 (H removes instruction; M removes UNKNOWN), replicating the Claude pattern.

## Upgrade rules (locked before results)
"Upgrade" = run full condition M (+ confirm H) for the conjunction test. Screening already gives A (SAC?) and H (knockout 1).
- **Exactly one model shows SAC signature** → upgrade it (run M). No deliberation.
- **Multiple show signature** → upgrade by most signature-functions; ties broken toward open-weights models (reproducibility). Max 2 upgrades; the 2nd only if the 1st's H/M knockout is ambiguous.
- **Zero show signature** → upgrade none. Reframe threshold as a main result ("screened N=4 frontier models; SAC signature only in Claude"), and queue claude-opus-4.8 screening (almost certainly SAC>0) to locate where on the capability line the threshold sits. Conjunction-generalization limitation stays as-is, plus an honest informative negative result.

## Interaction with the temp-0 multi-seed probe (running)
- If cross-seed two-way knockout is stable (across all seeds: SAC>0 iff condition A) → conjunction evidence upgrades from single-run to seed-stable; second model becomes a *reinforcement*, not a necessity → upgrade strictly one, reallocate budget to s2n-tls.
- If silenced sets drift wildly across seeds → conjunction needs a second model to rescue it → spend the two-upgrade budget.

## Reproducibility caveats (to record in paper, not claims)
- Only gpt-oss-120b is locally reproducible (H100). Claude/DeepSeek/Qwen/GPT are API; OpenRouter provider routing may vary backend/quantization → document provider or pin.
- Do NOT claim Qwen plus/max are open-weights / locally reproducible without verifying license (historically API-proprietary).
