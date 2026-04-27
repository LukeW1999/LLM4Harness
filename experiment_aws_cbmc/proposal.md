---
marp: true
theme: default
class: invert
paginate: true
style: |
  section {
    background-color: #1a1f2e;
    color: #e8eaf0;
    font-family: 'Segoe UI', 'Helvetica Neue', sans-serif;
    font-size: 28px;
  }
  h1 {
    color: #7eb8f7;
    font-size: 42px;
    border-bottom: 2px solid #3a5a8a;
    padding-bottom: 10px;
  }
  h2 {
    color: #a8c8f0;
    font-size: 34px;
  }
  h3 { color: #7eb8f7; font-size: 28px; }
  code {
    background-color: #0d1117;
    color: #c9d1d9;
    border-radius: 4px;
    padding: 2px 6px;
    font-size: 22px;
  }
  pre {
    background-color: #0d1117 !important;
    border: 1px solid #30363d;
    border-radius: 8px;
    padding: 16px;
  }
  pre code { font-size: 17px; line-height: 1.5; }
  table { border-collapse: collapse; width: 100%; font-size: 23px; }
  th {
    background-color: #21304a;
    color: #7eb8f7;
    padding: 10px 16px;
    border: 1px solid #3a5a8a;
  }
  td { padding: 8px 16px; border: 1px solid #2d3748; }
  tr:nth-child(even) td { background-color: #1e2a3a; }
  .good { color: #56d364; font-weight: bold; }
  .bad  { color: #f85149; font-weight: bold; }
  .note { font-size: 20px; color: #8b949e; font-style: italic; }
  section.title h1 {
    font-size: 44px;
    border: none;
    text-align: center;
    margin-bottom: 8px;
  }
  section.title { text-align: center; justify-content: center; }
---

<!-- _class: title invert -->

# LLM-Based Automatic Harness Generation
## for aws-c-common CBMC Verification

<br>

**Weiqi**
PhD Research Proposal · Supervisors: Lucas & Marie

---

# Motivation: Harness Writing is Hard

**CBMC verification requires a manually written harness for each function:**

```c
void aws_byte_buf_init_harness() {
    size_t capacity;
    __CPROVER_assume(capacity <= MAX_BUFFER_SIZE);  // precondition

    struct aws_byte_buf buf;
    int result = aws_byte_buf_init(&buf, aws_default_allocator(), capacity);

    assert(aws_byte_buf_is_valid(&buf));            // postcondition
    assert(buf.capacity == capacity);
    assert(buf.len == 0);
}
```

**The problem:**
- aws-c-common has **170 proof-equipped functions**, each requiring a hand-written harness
- Requires deep knowledge of CBMC semantics, library internals, and proof patterns
- Amazon employs formal verification engineers full-time for this task
- **Question:** Can LLMs automate harness generation at acceptable quality?

---

# Research Goal and Approach

## Pipeline

```
┌──────────────────┐    ┌─────────────────────┐    ┌──────────────┐
│  aws-c-common    │───▶│    LLM Prompt       │───▶│  Generated   │
│ (header + impl)  │    │  (variants A/B/C/D) │    │   Harness    │
└──────────────────┘    └─────────────────────┘    └──────┬───────┘
                                                            │
                                              ┌─────────────┘
                                              ▼
                                       ┌─────────────┐
                                       │    CBMC     │
                                       └──────┬──────┘
                                   PASS ◀─────┴─────▶ FAIL
                                    ✓                   │ error message
                                                        ▼
                                               ┌────────────────┐
                                               │  LLM (revised) │ ×4 rounds
                                               └────────────────┘
```

**Ground truth:** 170 official CBMC proofs written by Amazon engineers  
**Study scope:** 101 functions · 69 excluded (hash tables, priority queues, function pointers)  
**LLMs evaluated:** Claude (Sonnet) · Qwen · across 4 prompt variants

---

# Example: `aws_add_size_checked`

<div style="display:grid; grid-template-columns:1fr 1fr; gap:18px; font-size:21px">

<div>

**Ground Truth (Amazon)**
```c
void aws_add_size_checked_harness() {
  if (nondet_bool()) {
    /* Branch A: test u64 path */
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    int rv = aws_add_u64_checked(a,b,&r);
    if (!rv) assert(r == a + b);
    else     assert(b>0 && a>(UINT64_MAX-b));

  } else {
    /* Branch B: test u32 path */
    uint32_t a = nondet_uint32_t();
    uint32_t b = nondet_uint32_t();
    uint32_t r;
    if (!aws_add_u32_checked(a,b,&r))
         assert(r == a + b);
    else assert(b>0 && a>(UINT32_MAX-b));
  }
}
```

</div>

<div>

**LLM Generated (iter 1)**
```c
void aws_add_size_checked_harness() {
  /* Uses size_t only —
     misses u32/u64 dispatch */
  size_t a = nondet_size_t();
  size_t b = nondet_size_t();
  size_t r;

  int result =
    aws_add_size_checked(a, b, &r);

  if (result == AWS_OP_SUCCESS) {
    assert(r == a + b);
    assert(a <= SIZE_MAX-b || b==0);
  } else {
    assert(result == -1);
    assert(b>0 && a>SIZE_MAX-b);
  }
  /* + redundant duplicate block */
}
```

</div>
</div>

<span class="good">✓ Correct:</span> basic structure, success/failure paths, overflow condition
<span class="bad">✗ Missed:</span> `nondet_bool()` dispatch — u32 branch never tested (*TYPE_VARIANT*)

---

# Preliminary Results

| Metric | Claude | Qwen | Notes |
|--------|:------:|:----:|-------|
| CBMC pass rate | **94%** | **50%** | 84 functions, 4 prompt variants |
| Feedback loop (iter 1 → 4) | 68% → **89%** | — | genuine improvement vs over-constraining: under analysis |
| Postcondition recall (fuzzy) | **43–47%** | — | vs GT assertions |
| Preconditions per function | **2.9** | — | GT: 1.8 (+60%) |
| ESBMC portability | **84.5%** | — | direct transfer without modification |

<br>

**NL documentation effect:** providing Doxygen header comments improves recall by only ~4 pp over code-only prompts (47% vs 43%) — the implementation is the stronger specification signal.

---

# Key Finding: Pass Rate ≠ Specification Completeness

**The gap between two metrics tells the real story:**

```
CBMC pass rate:        94%   ████████████████████░░
Postcondition recall:  45%   █████████░░░░░░░░░░░░░
```

**Why does CBMC pass if recall is only ~45%?**

LLMs add **~60% more preconditions** than GT → input space is over-constrained → fewer paths for CBMC to explore → easier to pass, but less of the function's behaviour is actually verified.

**Breakdown of 191 GT properties absent from LLM harnesses:**

| Category | Count | What LLM systematically omits |
|----------|:-----:|-------------------------------|
| Validity predicates | 39 (20%) | `aws_X_is_valid()` after every call |
| Length invariants   | 36 (19%) | `buf.len <= buf.capacity` |
| Frame conditions    | 32 (17%) | unchanged fields explicitly asserted |
| Struct pointer checks | 27 (14%) | pointer relationship assertions |

**66% of missed properties are derivable from code alone** — not a knowledge gap, but a strategy gap: LLMs assert what a function *does*, not what it *leaves unchanged*.

---

# Research Plan

## Still to complete

| Task | Purpose |
|------|---------|
| Per-iteration recall analysis | Does feedback loop improve specification quality, or only pass rate? |
| CBMC `--cover branch` on GT vs LLM | Quantify over-constraining directly and objectively |
| Recall metric validation (~20 functions) | Confirm fuzzy matching is not underestimating true recall |
| Independent taxonomy annotation (191 items) | Establish inter-annotator agreement (Cohen's κ) |

## Branch coverage as a planned metric

CBMC's `--cover branch` reports which branches in the target function are **reachable** under the harness assumptions. Over-constraining preconditions make branches unreachable — CBMC passes but verifies less. This gives a more objective measure of specification completeness than string-matching on assertions.

---

# Timeline and Target Venue

## Milestones

| Phase | Work |
|-------|------|
| **Now → Month 2** | Complete analysis: branch coverage, per-iteration recall, metric validation |
| **Month 2** | Prepare annotation guide; second annotator begins taxonomy annotation |
| **Months 2–4** | Paper writing: Results, Discussion, Related Work |
| **Month 4** | First draft ready for supervisor feedback |
| **Months 5–6** | Revisions and submission |

<br>

**Target venue:** EMSE *(Empirical Software Engineering)* — primary  
**Backup:** STVR *(Software Testing, Verification and Reliability)*

<br>

<span class="note">Questions I would value your input on: (1) Is EMSE the right primary venue, or is STVR actually the stronger fit for this topic? (2) Is branch coverage the right planned metric, or is there a more standard measure used in the verification community?</span>
