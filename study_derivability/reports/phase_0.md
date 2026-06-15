# Phase 0 Report

## What I did

- Sparse-cloned aws-c-common, s2n-tls, FreeRTOS-Plus-TCP into `study_derivability/corpora/`
- Counted GT harnesses and LoC for all three corpora
- Audited existing LLM result directories (aws-c-common, s2n-tls)
- Checked taxonomy CSV shape
- Generated `figures/phase0_loc_hist.png`

## Key numbers

### GT Harness Census

| Corpus             | Harnesses | Total LoC | Mean LoC | Median LoC |
|--------------------|:---------:|:---------:|:--------:|:----------:|
| aws-c-common       |        84 |      2381 |     28.3 |         26 |
| s2n-tls (stuffer)  |        25 |      1176 |     47.0 |         47 |
| FreeRTOS-Plus-TCP  |        79 |      7177 |     90.8 |         77 |

### LLM Generation Census — aws-c-common

| Cell (cond_model)     | Attempted | Compiled | Mean Recall |
|-----------------------|:---------:|:--------:|:-----------:|
| A_claude               |        84 |       70 |     0.4732 |
| A_qwen                 |        84 |       44 |     0.3836 |
| B_claude               |        84 |       59 |      0.466 |
| B_qwen                 |        84 |       63 |     0.3862 |
| C_claude               |        84 |       72 |      0.465 |
| C_qwen                 |        83 |       68 |        n/a |
| D_claude               |        84 |       70 |      0.432 |
| D_qwen                 |        83 |       65 |        n/a |
| E_claude               |        83 |       81 | 0.5122 (iter=1)¹ |
| E_qwen                 |        83 |       80 | 0.4659 (iter=1)¹ |
| F_claude               |        83 |       81 | 0.4660 (iter=1)¹ |

### LLM Generation Census — s2n-tls

| Cell (cond_model)     | Attempted | Compiled | Mean Recall |
|-----------------------|:---------:|:--------:|:-----------:|
| A_qwen                 |        25 |       25 |     0.4165 |
| E_qwen                 |        24 |       22 |     0.4878 |

¹ E and F recall here is iter=1 only (read from iter_recall JSON). Paper reports best-iter:
  E_claude best-iter = 55.5%, F_claude best-iter = 52.1%. The raw iter_recall files exist
  in `evaluation/` — Phase 1 will use best-iter for the per-assertion recall labels.

### FreeRTOS LLM runs: **none yet** (Phase 3 task)

### Taxonomy CSV
- File: `experiment_aws_cbmc/annotation/annotated_missed_properties.csv`
- Rows: 191
- Columns: id, func, property_text, property_kind, info_source, reasoning, category, annotation_note, gt_snippet
- Category distribution: {'TYPE_VARIANT': 10, 'VALIDITY_PRED': 39, 'LEN_INVARIANT': 36, 'LEN_CHANGE': 24, 'FRAME_COND': 32, 'STRUCT_PTR': 27, 'CBMC_BUILTIN': 3, 'RETVAL_SPEC': 5, 'OVERFLOW_ARITH': 9, 'ZERO_CHECK': 6}

### FreeRTOS modules found (79 unique harnesses):
  - ARP: 12
  - CheckOptions: 1
  - CheckOptionsInner: 1
  - CheckOptionsOuter: 1
  - DHCP: 3
  - DHCPv6: 6
  - DNS: 10
  - DNS_ParseDNSReply: 1
  - ICMP: 2
  - IP: 4
  - IPUtils: 5
  - ND: 2
  - ProcessDHCPReplies: 1
  - RA: 2
  - ReadNameField: 1
  - Routing: 1
  - SkipNameField: 1
  - Socket: 5
  - TCP: 7
  - TCPWin: 1
  - UDP: 2
  - parsing: 7
  - prvChecksumIPv6Checks: 1
  - xRecv_Update_IPv4: 1
  - xRecv_Update_IPv6: 1

## Interesting observations

1. **s2n per-assertion match data is missing** [likely real]: The s2n evaluation
   only has aggregate recall numbers (`s2n_recall_condA.json`), not the
   `unmatched_gt_harness` lists that aws-c-common's `cross_verify_results_*.json`
   provides. Building the per-assertion dataset for s2n will require re-running
   the matcher. This is a Phase 1 gap, not a blocker.

2. **FreeRTOS has 3 duplicate harnesses** [likely real]: `OutputARPRequest` appears
   in 3 variant directories (buffer_alloc1, buffer_alloc2, FreeRTOS_OutputARPRequest)
   and `vSocketBind` in 3 config variants. Deduplication keeps the first found.
   The PI's shortlist should specify which variant to use.

3. **FreeRTOS LoC spread is wider** [don't know yet]: Mean LoC for FreeRTOS
   looks comparable to aws-c-common, but the network-parser harnesses (DNS, TCP)
   are much larger (200–300 LoC) while ARP simple functions are tiny (<30 LoC).
   This may affect parse complexity and LLM generation quality differently.

## Questions for the PI

1. **FreeRTOS shortlist**: `freertos_shortlist.md` was not found in the workspace.
   I have enumerated all 79 unique FreeRTOS harnesses above.
   Please confirm the 40-function Tier A/B/C selection, especially for the
   duplicate-variant functions (OutputARPRequest, vSocketBind).

2. **s2n per-assertion data**: The existing s2n evaluation lacks per-assertion
   match detail. For Phase 1, should I re-run the assertion matcher on the
   s2n LLM outputs (existing harnesses in `experiment_s2n/results/`)?
   Or does the PI have a more detailed match file I missed?

3. **aws-c-common: 83 vs 84 harnesses**: The dataset directory has 84 `funcN_*`
   entries. The paper uses 83. Is one function excluded? If so, which one and why?
   (This matters for the assertion-level join in Phase 1.)

4. **Taxonomy join key**: The 191-row taxonomy uses normalized expression strings.
   For Phase 1's assertion-level join, should I match on normalized expression
   (fuzzy) or on (func, line_number)?

## What I propose to do next

Confirm PI answers to questions 1 and 2, then start Phase 1:
parse all GT harnesses with libclang to extract every `assert()` and
`__CPROVER_assume()` call, build `data/gt_assertions.csv`, and join with
the existing 191-row taxonomy.
