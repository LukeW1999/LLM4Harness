# Annotation Guide: CBMC Property Taxonomy

**Task**: Classify each CBMC assertion into one of 10 categories.

A "missed property" is an assertion present in the ground-truth (manually written) harness
but absent from the LLM-generated harness. Your job is to categorize *why* the LLM missed it.

---

## Categories and Definitions

### 1. VALIDITY_PRED — Structural validity predicate

**Definition**: The assertion calls a named validity/invariant predicate specific to the
data structure family (e.g., `aws_linked_list_is_valid`, `aws_ring_buffer_is_valid`).

**When to use**: The required property can only be written by knowing the exact predicate name.
The LLM would need to look up this predicate from the library headers; it cannot derive it
from the NL description alone.

**Examples**:
```c
assert(aws_linked_list_is_valid(list));
assert(aws_linked_list_node_next_is_valid(node));
assert(aws_ring_buffer_is_valid(&ring));
```

---

### 2. LEN_INVARIANT — Length/size invariant across the object

**Definition**: An assertion that constrains the `len` or `capacity` field to stay consistent
with some other field or to remain bounded after the operation.

**When to use**: The assertion checks that a length field has a specific post-operation value
or a relationship like `len <= capacity` or `len == old_len + 1`.

**Examples**:
```c
assert(list->length == initial_length + 1);
assert(buf->len <= buf->capacity);
assert(cursor->len == original_len - advance_amount);
```

---

### 3. FRAME_COND — Frame condition (unchanged fields)

**Definition**: An assertion that explicitly checks that a field or memory region was NOT
modified by the function call (i.e., it is unchanged/preserved).

**When to use**: The property says "this field should still have its old value" after calling
the function. Often expressed as `__CPROVER_old(x) == x` or direct equality checks.

**Examples**:
```c
assert(buf->allocator == old_allocator);
assert(list->data == old_data_ptr);
assert(buf->capacity == initial_capacity);  // after a non-resize operation
```

---

### 4. STRUCT_PTR — Pointer/reference validity check

**Definition**: An assertion that checks a pointer within a struct is non-null or points to
valid memory, beyond simple null-checks of the top-level argument.

**When to use**: The assertion dereferences or checks an internal pointer field of a struct
(e.g., `node->next`, `buf->buffer`, `list->data`), not just the function argument itself.

**Examples**:
```c
assert(node->next != NULL);
assert(list->data != NULL);
assert(buf->buffer != NULL);
assert(cursor->ptr != NULL);
```

---

### 5. LEN_CHANGE — Quantitative length/size delta

**Definition**: An assertion that checks the exact numeric change in a length field after
an operation (e.g., push adds 1, pop removes 1).

**When to use**: The property encodes the precise arithmetic relationship between old and new
length values. Distinct from LEN_INVARIANT which covers static bounds; this covers deltas.

**Examples**:
```c
assert(aws_array_list_length(&list) == initial_length + 1);  // after push
assert(aws_array_list_length(&list) == initial_length - 1);  // after pop
assert(buf->len == initial_len + n_appended);
```

---

### 6. TYPE_VARIANT — Type or path variant assertion

**Definition**: An assertion that only appears in a specific type variant or code path (e.g.,
`uint32_t` vs `uint64_t`, overflow vs non-overflow case, or a specific enum branch).

**When to use**: The GT harness tests multiple type variants, and the LLM only covers one.
The missed assertion belongs to a variant the LLM did not generate.

**Examples**:
```c
// GT tests both:
assert(result_u64 == expected_u64);  // LLM generates this
assert(result_u32 == expected_u32);  // LLM misses this variant
```

---

### 7. OVERFLOW_ARITH — Arithmetic overflow or wrapping assertion

**Definition**: An assertion about overflow behavior: either that overflow did NOT occur
(result is bounded) or that the result correctly implements saturating/wrapping arithmetic.

**When to use**: The property checks the arithmetic range of the result, typically for
checked math functions (`add_size_checked`, `mul_size_saturating`, etc.).

**Examples**:
```c
assert(!overflow_occurred);
assert(result <= SIZE_MAX);
assert(result == a + b || result == SIZE_MAX);  // saturating
```

---

### 8. ZERO_CHECK — Null/zero-initialized field assertion

**Definition**: An assertion that a field or output is zero-initialized or null after
the operation (cleanup, initialization, or reset).

**When to use**: The property checks that a field was explicitly zeroed/cleared. Often
seen in `_clean_up` or initialization functions.

**Examples**:
```c
assert(buf->len == 0);
assert(buf->buffer == NULL);
assert(list->length == 0);  // after clean_up
```

---

### 9. RETVAL_SPEC — Return value specification

**Definition**: An assertion about the function's return value — not about the side-effected
struct fields, but the actual return code or boolean result.

**When to use**: The property checks what the function returns (AWS_OP_SUCCESS, true/false,
a computed value), and the LLM omitted this return-value check.

**Examples**:
```c
assert(result == AWS_OP_SUCCESS);
assert(return_value == true);
assert(aws_add_size_checked(a, b, &out) == 0);
```

---

### 10. CBMC_BUILTIN — CBMC built-in or verification-only construct

**Definition**: An assertion using a CBMC-specific built-in that requires knowing the CBMC
API: `__CPROVER_same_object`, `__CPROVER_POINTER_OBJECT`, `__CPROVER_valid_pointer`, etc.

**When to use**: The property uses a construct from the CBMC model-checking API that is
not standard C. The LLM would need explicit knowledge of CBMC's intrinsics.

**Examples**:
```c
assert(__CPROVER_same_object(buf->buffer, old_buffer));
assert(__CPROVER_POINTER_OBJECT(ptr) == CPROVER_POINTER_OBJECT(orig));
```

---

## Decision guide

Use this flowchart when unsure:

1. Does it call a named `*_is_valid()` predicate? → **VALIDITY_PRED**
2. Is it checking a pointer field inside a struct for non-null? → **STRUCT_PTR**
3. Does it use `__CPROVER_*` intrinsics? → **CBMC_BUILTIN**
4. Does it check the *return value* of the function call? → **RETVAL_SPEC**
5. Does it check a field equals zero/NULL after cleanup? → **ZERO_CHECK**
6. Does it check *exact change* in length (±N)? → **LEN_CHANGE**
7. Does it check a field is *unchanged* from before? → **FRAME_COND**
8. Does it check a bound/invariant on len/capacity? → **LEN_INVARIANT**
9. Does it handle overflow/saturation arithmetic? → **OVERFLOW_ARITH**
10. Is it from a type-specific variant (u32/u64/etc.)? → **TYPE_VARIANT**

---

## Worked examples

| Property text | Category | Reasoning |
|---|---|---|
| `aws_linked_list_is_valid(list)` | VALIDITY_PRED | Named validity predicate |
| `node->next != NULL` | STRUCT_PTR | Internal pointer field check |
| `buf->len == 0` after `clean_up` | ZERO_CHECK | Zero after cleanup |
| `buf->capacity == initial_cap` | FRAME_COND | Unchanged field |
| `list->length == old_len + 1` | LEN_CHANGE | Exact delta |
| `result == AWS_OP_SUCCESS` | RETVAL_SPEC | Return value |
| `buf->len <= buf->capacity` | LEN_INVARIANT | Invariant bound |
| `__CPROVER_same_object(a,b)` | CBMC_BUILTIN | CBMC intrinsic |

---

## Instructions for annotators

1. Read `property_text` (the C assertion text).
2. Read `gt_snippet` (the ground-truth harness for context).
3. Choose **one** category from the 10 above.
4. Fill in `your_category` in the CSV.
5. If genuinely unclear between two categories, pick the more specific one
   (e.g., VALIDITY_PRED > STRUCT_PTR if a named validity predicate is present).
6. Do not consult the original annotations — work independently.
