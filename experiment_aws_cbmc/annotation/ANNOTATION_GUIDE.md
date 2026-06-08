# Annotation Guide
## Study: "Do LLMs Verify or Conform?"
### Inter-rater reliability pilot — 30 functions, 101 assertions

**Estimated time:** 60–90 minutes  
**Your task:** Classify each missed assertion into one of three semantic categories.

---

## Background

### What is CBMC and what is a harness?

**CBMC** (C Bounded Model Checker) is a formal verification tool used at Amazon to prove that C library functions are correct. To verify a function, an engineer writes a **harness** — a small C program that:

1. Creates nondeterministic inputs (representing *all possible* inputs)
2. Calls the function under test
3. Checks postconditions using `assert(...)` statements

If CBMC proves no assertion ever fails (result: **VERIFICATION SUCCESSFUL**), the function is formally verified. If an assertion fails, CBMC produces a concrete counterexample showing what input triggers it.

### What is the research about?

We gave an LLM the same task: generate CBMC harnesses for 83 Amazon C library functions. The LLM works iteratively — it generates a harness, CBMC checks it, and if CBMC cannot verify it (result: **UNKNOWN**), the LLM tries to fix the harness.

We found that LLM-generated harnesses often **miss assertions** that Amazon's expert engineers included. This study classifies *what kind* of property each missed assertion checks.

### The annotation task (taxonomy only)

For each row in the CSV, you are shown an assertion that was in the **expert harness** but **absent from the LLM harness**. You classify what kind of property it checks. This is a semantic judgment — you are not evaluating whether the LLM was right or wrong, just what the assertion is *about*.

> **Note:** A second question (why did the LLM miss it?) is answered separately through automated log analysis, not through this annotation. Your task is only the taxonomy classification.

---

## The CSV columns

| Column | Description |
|--------|-------------|
| `id` | Row identifier (P001, P002, …) |
| `func_name` | The C function being verified |
| `func_description` | Plain-English description of what the function does |
| `assert_text` | **The assertion the LLM missed** — this is what you classify |
| `gt_harness_assertions` | All `assert(...)` lines in the expert harness for this function — use these for context |
| `suggested_taxonomy` | Auto-generated suggestion — treat as a hint, not an answer |
| **`your_taxonomy`** | **← Fill this in** (see categories below) |
| `notes` | Optional: write any uncertainty or reasoning here |

---

## The three categories

### `validity_predicate`

The assertion checks that a **value, pointer, or data structure state is correct** — i.e., that the function produced the right result.

**Includes:**
- Pointer / reference is non-null: `buf != NULL`, `list.data != null`, `node->next != NULL`
- Function return value is correct: `result == AWS_OP_SUCCESS`, `ret == 0`
- Named validity predicate: `aws_ring_buffer_is_valid(ring_buf)`, `aws_linked_list_is_valid(list)` — these are library-specific functions that check a data structure's internal consistency
- Arithmetic / overflow result: `r == SIZE_MAX`, `(a > UINT64_MAX - b)`, overflow flags
- Type-path variant: the expert harness tests both `uint32_t` and `uint64_t` paths; the LLM only tested one

**Key question:** Does the assertion say *"this value / pointer / result is correct"*?

---

### `length_invariant`

The assertion checks a **length, capacity, size, or offset field** of a data structure.

**Includes:**
- Bound invariant: `buf->len <= buf->capacity`, `cursor->len >= 0`
- Exact delta after an operation: `list->length == old_length + 1` (after push), `buf->len == initial_len + n_bytes`
- Size after cleanup: `buf->len == 0` (after clean_up or reset)
- Capacity relationships: `ring_buf->allocation_end - ring_buf->allocation == size`

**Key question:** Does the assertion mention `len`, `length`, `capacity`, `size`, or `offset`?

---

### `frame_condition`

The assertion checks that something was **not modified** by the function call — it still has the same value it had before.

**Precise test — requires BOTH:**
1. The comparison is to a **pre-call snapshot** value (`old_X`, `prev_X`, `old.field`, `X_old`)
2. The field is **expected to be unchanged** (delta = zero)

**Includes:**
- Field unchanged: `buf->capacity == old.capacity`, `list->alloc == old.alloc`
- Pointer unchanged: `list->data == old_data_ptr`
- CBMC built-in unchanged: `__CPROVER_same_object(a, b)`

**Does NOT include** (common mistakes):
- `buf->allocator == NULL` → **validity_predicate** — allocator was *set to* null, not left unchanged
- `list->alloc == allocator` (where `allocator` is an input param) → **validity_predicate** — field was *initialised to* the input value
- `list->length == old.length - 1` → **length_invariant** — length *changed* by -1, not unchanged
- For **initialisation functions** (`aws_X_init`, `aws_X_new`): fields being set during init are postconditions, not frame conditions

**Key question:** Does the assertion say *"this specific field was not touched by the function, and I can prove it by showing it still equals its pre-call value"*?

---

## Decision flowchart

```
What is the assertion about?
│
├── Does it mention len / length / capacity / size / offset?
│   └── YES → length_invariant
│
├── Is it comparing to a PRE-CALL SNAPSHOT (old_X) AND the field should be UNCHANGED?
│   ├── Both YES → frame_condition
│   └── Referenced old_ but delta is non-zero (e.g., length-1) → length_invariant
│
├── Is it comparing to null, a constant, or an input parameter?
│   └── YES → validity_predicate  (it's a postcondition about the result)
│
└── Anything else (named predicate, return value, pointer check, arithmetic)?
    └── YES → validity_predicate
```

---

## Worked examples

| assert_text | Category | Reasoning |
|---|---|---|
| `aws_ring_buffer_is_valid(ring_buf)` | validity_predicate | Named validity predicate |
| `buf->capacity == old.capacity` | **frame_condition** | Capacity compared to pre-call snapshot; append does not change capacity |
| `buf->allocator == NULL` | **validity_predicate** | Allocator was *set to* null — the function modified it; not "unchanged" |
| `list->alloc == allocator` (after init) | **validity_predicate** | Field *initialised to* input param; postcondition, not frame |
| `list->length == old.length - 1` | **length_invariant** | Length *changed* by −1 after pop; references old_ but delta ≠ 0 |
| `ring_buf->allocation_end - ring_buf->allocation == size` | length_invariant | Buffer size relationship |
| `list->length == old_length + 1` | length_invariant | Exact length delta after push |
| `result == AWS_OP_SUCCESS` | validity_predicate | Return value correct |
| `node->next != NULL` | validity_predicate | Internal pointer non-null |
| `aws_linked_list_node_prev_is_valid(&node)` | validity_predicate | Named node validity predicate |
| `buf->len == 0` after `clean_up` | length_invariant | Length zeroed after cleanup |

---

## Notes on assertion text format

Some assertions appear in CBMC diagnostic format (e.g., `ring_buf != ((void *)0) check failed`) rather than raw C. This is CBMC reporting a null-pointer check failure. Treat `X != ((void *)0) check failed` as equivalent to `assert(X != NULL)`. Classify it as `validity_predicate`.

---

## Step-by-step instructions

1. Open `pilot_worksheet.csv` in Excel or Google Sheets  
   *(If columns merge: Data → Text to Columns → Delimiter: comma)*

2. For each row:
   - Read `assert_text` — **this is the assertion to classify**
   - Read `func_description` — what does this function do?
   - Read `gt_harness_assertions` — the full set of expert assertions for context
   - Apply the decision flowchart above
   - Fill in `your_taxonomy` with one of: `validity_predicate`, `length_invariant`, `frame_condition`
   - The `suggested_taxonomy` column shows an auto-generated hint — you may agree or disagree
   - Use `notes` for any uncertainty

3. **Work independently** — do not discuss labels with the other annotator until both complete their sheets

4. Save and return your filled CSV

---

## Valid values (exact spelling, lowercase, no spaces)

`your_taxonomy`:
- `validity_predicate`
- `length_invariant`  
- `frame_condition`

---

## Questions?

If an assertion is genuinely ambiguous between two categories, pick the more specific one:
- `frame_condition` beats `validity_predicate` if the assertion is clearly about an unchanged field
- `length_invariant` beats `validity_predicate` if the assertion is about a len/capacity field

If still unsure, write your reasoning in `notes` and pick your best guess.
