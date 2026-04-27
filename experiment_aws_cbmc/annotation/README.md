# Property Taxonomy Annotation Guide

This directory contains the annotation data and instructions for the 2D property taxonomy
used in the SpecVerify journal extension study.

---

## What We Are Annotating

We are classifying **missed GT properties** — verification properties that appear in the
Amazon CBMC ground-truth harnesses but were **not generated** by the LLM (condA-Claude).

File: `annotated_missed_properties.csv`  
Total: 191 missed properties across 74 functions (out of 83).

---

## The 2D Taxonomy

Each property is classified along two independent dimensions:

### Dimension 1: Information Source

| Code   | Meaning | Example |
|--------|---------|---------|
| **NL** | The property is **explicitly described** in the Doxygen / natural language documentation of the function. An attentive reader of the header would know this. | `buf.len == old.len + from.len` — Doxygen says "on success, len increases by from.len" |
| **CODE** | The property is **not stated in the docs** but can be **inferred by reading the C implementation** carefully. | `buf.allocator == old.allocator` — not in docs, but reading the code shows allocator is never touched |
| **DOMAIN** | The property requires **CBMC or AWS proof-library specific knowledge** that is not in the source code or docs. You need to know that certain predicates exist and should be called. | `aws_byte_buf_is_valid(&buf)` — requires knowing this predicate is a CBMC harness idiom |

### Dimension 2: Reasoning Complexity

| Code          | Meaning | Example |
|---------------|---------|---------|
| **DIRECT**    | A single, direct step from the spec or code to the property. No branching needed. | `cursor.len == str.len` — cursor wraps string, length is set directly |
| **CONDITIONAL** | Requires reasoning about **branches or conditional paths** (success/failure cases, edge cases). | Overflow condition only holds on the error path; success and failure have different assertions |
| **EXHAUSTIVE** | Requires **enumerating all fields** that are *not* modified — the "frame condition" problem. You must check every struct field and assert the unchanged ones. | `old.allocator == new.allocator`, `old.capacity == new.capacity`, `old.data == new.data` — all must be listed |
| **STRUCTURAL** | Requires understanding **linked list pointer manipulation** or **CBMC harness architecture**. The property follows from tracing through pointer wiring or knowing proof patterns. | `list.tail.prev == &to_add` after push_back — requires tracing prev/next pointer updates |

---

## Category Labels (Semantic)

| Category       | Description |
|----------------|-------------|
| FRAME_COND     | Frame condition: struct field that is **not** modified (old.X == new.X) |
| VALIDITY_PRED  | AWS proof-helper validity predicate (`aws_X_is_valid`, `aws_is_zeroed`, etc.) |
| STRUCT_PTR     | Structural pointer in linked list or ring buffer (next/prev/head/tail) |
| LEN_CHANGE     | Length or size **changes** as documented postcondition |
| LEN_INVARIANT  | Length or size **invariant** / equality check (not a frame cond) |
| TYPE_VARIANT   | Property specific to 32-bit vs 64-bit type testing (GT tests both separately) |
| OVERFLOW_ARITH | Arithmetic overflow or saturation correctness condition |
| RETVAL_SPEC    | Return value specification |
| ZERO_CHECK     | Memory zeroing postcondition |
| CBMC_BUILTIN   | CBMC-specific intrinsic (`__cprover_overflow_mult`, `aws_mem_is_writable`) |

---

## The CSV Schema

| Column           | Description |
|-----------------|-------------|
| `id`             | Unique property ID (P001–P191) |
| `func`           | Function name |
| `property_text`  | The missed property expression (normalized, lowercase) |
| `property_kind`  | `assertion` or `precondition_instance` |
| `info_source`    | **Annotate this**: NL / CODE / DOMAIN |
| `reasoning`      | **Annotate this**: DIRECT / CONDITIONAL / EXHAUSTIVE / STRUCTURAL |
| `category`       | **Annotate this**: semantic category (see table above) |
| `annotation_note`| Annotation rationale (already filled by first annotator) |
| `gt_snippet`     | First 600 chars of the ground-truth harness for context |

---

## How to Do Your Annotation (Step-by-Step)

### Setup

1. Open `annotated_missed_properties.csv` in a spreadsheet (LibreOffice Calc or Excel)
2. Open a **second copy** as a blank template — **do not look at the existing annotations** while annotating
3. For each row, read `property_text` and `func`, then look at `gt_snippet` for context

### For each property, ask these questions:

**Step 1 — Info Source:**
- Is this property stated in the function's Doxygen comment? → **NL**
- Is this inferable only by reading the C implementation? → **CODE**
- Does this require knowing about CBMC predicates / AWS proof helpers? → **DOMAIN**

**Step 2 — Reasoning Complexity:**
- Can you derive this in one step from a single stated fact? → **DIRECT**
- Does it only hold on a specific code path (success/failure/edge case)? → **CONDITIONAL**
- Does it require enumerating ALL unchanged fields of a struct? → **EXHAUSTIVE**
- Does it require tracing linked list pointer updates or CBMC harness structure? → **STRUCTURAL**

**Step 3 — Category:**
Choose the best-fit semantic label from the table above.

### Tips for common cases

| If you see...                          | Likely classification |
|----------------------------------------|-----------------------|
| `old_X.field == X.field`               | CODE × EXHAUSTIVE × FRAME_COND |
| `aws_X_is_valid(...)` or `aws_is_zeroed(...)` | DOMAIN × STRUCTURAL × VALIDITY_PRED |
| `__cprover_...`                        | DOMAIN × STRUCTURAL × CBMC_BUILTIN |
| `list.head.next == &node` or `node.prev == ...` | CODE × STRUCTURAL × STRUCT_PTR |
| `buf.len == old.len + from.len`        | NL × DIRECT × LEN_CHANGE |
| `uint64_max` or `uint32_max` in result | CODE × CONDITIONAL × TYPE_VARIANT |
| Return value `rval == (expr)`          | CODE × DIRECT × RETVAL_SPEC |

---

## Inter-Rater Agreement

Once **you** complete your annotation independently (without looking at the existing annotations),
compare the two annotation sets and compute **Cohen's kappa** for each dimension:

```python
from sklearn.metrics import cohen_kappa_score

kappa_info    = cohen_kappa_score(annotator1_info, annotator2_info)
kappa_reason  = cohen_kappa_score(annotator1_reason, annotator2_reason)
kappa_cat     = cohen_kappa_score(annotator1_cat, annotator2_cat)
```

Target: κ ≥ 0.7 for each dimension (substantial agreement).  
Disagreements should be resolved through discussion and recorded.

---

## Files in This Directory

| File | Description |
|------|-------------|
| `annotated_missed_properties.csv` | First-pass annotations (191 rows, all classified) |
| `annotate_missed_properties.py`   | Script that generated the first-pass annotations |
| `README.md`                       | This file |

---

## Summary of First-Pass Annotation Results

```
Info Source:
  CODE    127  (66.5%)   ← most missed props require reading the implementation
  DOMAIN   45  (23.6%)   ← require CBMC / AWS proof-library knowledge
  NL       19   (9.9%)   ← documented but LLM still missed them

Reasoning Complexity:
  STRUCTURAL   64  (33.5%)   ← linked list pointers + validity predicates
  DIRECT       61  (31.9%)   ← simple but still missed
  CONDITIONAL  34  (17.8%)   ← branch-specific properties
  EXHAUSTIVE   32  (16.8%)   ← frame conditions (enumerate all unchanged fields)

2D Matrix:
  CODE   × DIRECT       :  42
  CODE   × STRUCTURAL   :  19
  CODE   × CONDITIONAL  :  34
  CODE   × EXHAUSTIVE   :  32  ← Frame Condition Problem
  DOMAIN × STRUCTURAL   :  45  ← Validity Predicate Problem
  NL     × DIRECT       :  19  ← documented but still missed
```

**Key finding**: The two largest failure modes are:
1. **DOMAIN × STRUCTURAL (45)**: LLMs do not know AWS CBMC proof-library predicates
2. **CODE × EXHAUSTIVE (32)**: Frame conditions — LLMs fail to enumerate all unchanged fields
3. **CODE × DIRECT (42)**: Simple code-inferable properties that LLMs still miss
