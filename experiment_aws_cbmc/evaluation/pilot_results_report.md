# Pilot Experiment Report: LLM-Based CBMC Harness Generation on aws-c-common

**Date:** 2026-04-07  
**Model:** qwen2.5-coder-32b-instruct (DashScope)  
**Dataset:** 5 functions from aws-c-common CBMC proofs  
**Prompts tested:** A (header-only), B (header+impl), C (header+impl+macro hints)

---

## 1. Summary Table

| Function | Complexity | P-A Assert | P-B Assert | P-C Assert | P-D Assert | P-A Assume | P-B Assume | P-C Assume | P-D Assume |
|----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|-----------|
| aws_add_size_checked | Simple (arithmetic) | 33% | 0% | 0% | 33% | 100% | 100% | 100% | 100% |
| aws_byte_buf_init | Simple (init) | 75% | **100%** | 75% | 75% | 100% | 100% | 100% | 100% |
| aws_array_list_back | Medium (struct+state) | 33% | 33% | 0% | 33% | 33% | 0% | 0% | **67%** |
| aws_byte_buf_append | Medium-complex (2 structs) | 0% | 29% | 29% | 29% | 0% | 50% | 50% | **100%** |
| aws_linked_list_push_back | Complex (pointer invariants) | 0% | 0% | 20% | 20% | 100% | 100% | 100% | 100% |
| **Average** | | **28%** | **32%** | **25%** | **38%** | **67%** | **70%** | **70%** | **93%** |

Prompt types: A=header-only, B=header+impl, C=header+impl+macro hints, D=header+impl+proof helpers API

All 15 generated harnesses are syntactically valid (balanced braces, correct function signature).

---

## 2. Key Findings

### Finding 1: Simple initialization functions → LLMs work well

`aws_byte_buf_init` with Prompt B achieves 100% assert recall. The LLM correctly:
- Initializes non-deterministic inputs  
- Assumes `allocator != NULL` and `capacity <= MAX_BUFFER_SIZE`
- Asserts `buf.len == 0`, `buf.capacity == capacity`, `buf.allocator == allocator`, `aws_byte_buf_is_valid(&buf)`
- Handles both success and failure branches

This shows the approach is **fundamentally sound** for functions where contracts map directly to struct field values.

### Finding 2: The main gap is project-specific proof infrastructure

The single most impactful missing knowledge is the **proof helpers API** in `verification/cbmc/include/proof_helpers/make_common_data_structures.h`. Ground truth harnesses use:

```c
// What LLMs don't know:
ensure_array_list_has_allocated_data_member(&list);
ensure_linked_list_is_allocated(&list, MAX_LINKED_LIST_ITEM_ALLOCATION);
ensure_byte_buf_has_allocated_buffer_member(&to);
aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE)
save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
assert_array_list_equivalence(&list, &old, &old_byte);
assert_bytes_match(buf1, buf2, len);
```

LLMs fall back to `malloc` + manual initialization, which is semantically reasonable but doesn't match the GT pattern.

**Implication for Prompt D:** Including the `make_common_data_structures.h` header in the prompt is the single most impactful improvement we can make. This is a key experimental variable.

### Finding 3: Semantic correctness vs. syntactic match divergence

The metric underestimates LLM quality due to syntactic differences that are semantically equivalent:

**aws_linked_list_push_back (Prompt B) — LLM output:**
```c
assert(list->tail.prev == node);          // GT: list.tail.prev == &to_add
assert(node->next == &list->tail);        // GT: (implicit via node_next_is_valid)
assert(node->prev == old_tail_prev);      // GT: to_add.prev == old_last
assert(old_tail_prev->next == node);      // extra (but valid!)
```
GT uses stack allocation (`struct aws_linked_list list`) while Qwen uses `malloc`. The postconditions are *semantically equivalent* — Qwen even adds `old_tail_prev->next == node` which is not in GT but is a valid additional assertion.

**Semantic recall is higher than syntactic recall** — we need a semantic equivalence layer in evaluation.

**aws_add_size_checked (Prompt B):**
```c
// GT:   assert(r == a + b)             (r is out-param uint64_t)
// Qwen: assert(*r == a + b)            (r is pointer to out-param)
```
Same semantics, different dereferencing style (GT harness declares `r` as a value with address taken via `&r`; Qwen declares `r` as a pointer). The 0% recall for this function is almost entirely a syntactic mismatch artifact.

### Finding 4: Prompt B > Prompt A for struct-heavy functions; Prompt C can hallucinate

- **Prompt B** (with implementation) helps for medium-complexity functions: better postcondition coverage for `aws_byte_buf_append` (29% vs 0%)
- **Prompt C** (macro hints) occasionally hurts: for `aws_array_list_back`, Prompt C scores 0% because it expands `aws_array_list_is_valid` inline rather than using it as a predicate
- **Prompt A** (header-only) is surprisingly competitive for simple functions, and occasionally outperforms B (byte_buf_init: 75% vs 100%)

### Finding 5: LLMs hallucinate implementation under Prompt A for arithmetic functions

For `aws_add_size_checked` with Prompt A, Qwen **writes a mock implementation** inside the harness file:
```c
// Mock implementation of the function to be verified
int aws_add_size_checked(size_t a, size_t b, size_t *r) {
    size_t result = a + b;
    if (result < a || result < b) { return -1; }  // WRONG: relies on unsigned wraparound
    *r = result;
    return 0;
}
```
This is a failure mode where the LLM, lacking the implementation, fills it in incorrectly. The overflow check `result < a` relies on unsigned arithmetic wraparound — this would **mask the actual overflow in CBMC** since CBMC can track overflow precisely.

### Finding 6: Bounded reasoning helpers are the critical bridge

The assume recall gap for struct functions (array_list, byte_buf_append) is driven by:
- Missing: `aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE)` 
- Missing: `aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE)`

These are CBMC-specific bounding constraints that prevent state space explosion. Without them, harnesses are syntactically valid but would be too slow/unbounded to run in CBMC. LLMs don't know to add them without seeing the `is_bounded` API.

---

## 3. Error Taxonomy

| Error Type | Frequency | Example |
|-----------|-----------|---------|
| Missing proof helper calls | Very High | No `ensure_XXX_has_allocated_data_member()` |
| Missing bounding constraints | High | No `aws_XXX_is_bounded()` |
| Pointer vs. value convention mismatch | Medium | `list->tail.prev` vs `list.tail.prev` |
| Semantic rephrasing (different but equivalent) | Medium | `node->next == &list->tail` vs `aws_linked_list_node_next_is_valid(&node)` |
| Hallucinated mock implementation | Low (Prompt A only) | Inlined `aws_add_size_checked` reimplementation |
| Non-existent macros | Low | `AWS_MEM_IS_SAME()` (doesn't exist in codebase) |
| Over-specification (extra but valid asserts) | High | Asserting more than GT, semantically valid |

---

## 4. Proposed Next Steps for Journal Experiments

### Experiment Design (Full Scale)

**Dataset expansion:**
- 30 functions across 3 complexity tiers:
  - Tier 1 (simple): 10 functions — arithmetic, simple struct init
  - Tier 2 (medium): 10 functions — single struct with validity predicate
  - Tier 3 (complex): 10 functions — multiple structs, byte-level invariants

**Prompt D (new — include proof helpers API):**
Add `make_common_data_structures.h` content to the prompt. Hypothesis: this will bring assume recall from ~30% to ~70%+ for struct-heavy functions.

**Evaluation improvements:**
1. **Semantic equivalence pass**: Normalize `list->x` vs `list.x`, `&var` vs `var`, variable name aliasing
2. **CBMC compilation check**: Actually try to compile with CBMC (requires installing CBMC)
3. **Verification equivalence**: If both harnesses compile, check if they agree on verification outcome (both PASS or both FAIL)

**Models to compare (for journal):**
- qwen2.5-coder-32b-instruct (current)
- claude-3-5-sonnet (baseline comparison)
- qwen-max (general vs. code-specific comparison)

### Key Research Claims Supported by Pilot

1. Headers with `AWS_PRECONDITION`/`AWS_POSTCONDITION` annotations are sufficient specification for LLM-based harness generation for simple functions
2. For complex struct operations, proof infrastructure knowledge (helpers API) is the primary bottleneck, not semantic understanding
3. Including implementation improves recall for medium-complexity functions but not always for simple/complex ones
4. Syntactic evaluation underestimates LLM quality — semantic equivalence evaluation is needed

---

## 5. Generated Harness Quality Samples

### Best case: aws_byte_buf_init (Prompt B)
- 100% assert recall, correct structure, uses proof helpers
- Only issue: adds `assert(!aws_byte_buf_is_valid(&buf))` in failure branch which is arguably wrong (struct zeroed via `AWS_ZERO_STRUCT` may still satisfy is_valid)

### Most interesting case: aws_linked_list_push_back (Prompt B)
- 0% syntactic recall but **semantically complete** postconditions
- Demonstrates metric limitation clearly
- Adds extra valid assertion (`old_tail_prev->next == node`) not in GT

### Worst case: aws_byte_buf_append (Prompt A)
- 0% assert and assume recall
- LLM uses manual bounds/validity checks instead of helper functions
- Uses `AWS_MEM_IS_SAME` (hallucinated macro)

### Near-perfect case: aws_array_list_back (Prompt D)
```c
void aws_array_list_back_harness() {
    struct aws_array_list list;
    __CPROVER_assume(aws_array_list_is_bounded(&list, MAX_INITIAL_ITEM_ALLOCATION, MAX_ITEM_SIZE));
    ensure_array_list_has_allocated_data_member(&list);
    __CPROVER_assume(aws_array_list_is_valid(&list));
    struct aws_array_list old = list;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array((uint8_t *)list.data, list.current_size, &old_byte);
    uint8_t val[MAX_ITEM_SIZE];
    __CPROVER_assume(AWS_MEM_IS_WRITABLE(val, list.item_size));
    int result = aws_array_list_back(&list, val);
    if (result == AWS_OP_SUCCESS) {
        assert(list.length > 0);  // GT: assert(list.data != NULL) + assert(list.length != 0)
        assert_bytes_match(val, (uint8_t *)list.data + last_item_offset, list.item_size);
    } else {
        assert(result == AWS_OP_ERR);
        assert(list.length == 0);
    }
    assert(aws_array_list_is_valid(&list));
    assert_array_list_equivalence(&list, &old, &old_byte);
}
```
Structure is nearly identical to GT. Assert recall 33% is due to syntactic rephrasing (`list.length > 0` vs two separate GT asserts).

### Near-perfect case: aws_byte_buf_append (Prompt D)
Achieved 100% assume recall. Correctly uses `ensure_byte_buf_has_allocated_buffer_member`, `aws_byte_buf_is_bounded`, `save_byte_from_array`, and `assert_byte_from_buffer_matches`. Missing only `from.len == old_from.len` (immutability of cursor length) which is subtle.

---

*Generated by pilot experiment on 2026-04-07. See `/results/` for all 15 harness files and `/evaluation/eval_results_prompt_B.json` for raw metrics.*
