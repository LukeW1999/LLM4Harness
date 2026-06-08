# Annotation Tutorial — Learning to Classify Assertions
## Step-by-step guide for new annotators

**Before starting:** Read this tutorial completely before opening the CSV. It takes about 15 minutes and will prevent most classification mistakes.

---

## What you are looking at

Each row in your CSV is one C `assert(...)` statement that an Amazon expert engineer included in a formal verification test, but an AI language model **forgot to write**.

Your job: classify *what kind of property* this assertion checks.

There are exactly three types. Each one has a different meaning in formal verification:

---

## The three types — intuitive version

### Type 1: `validity_predicate` — "Did the function produce the right result?"

This checks that the function's **output or return value** is correct.

Think of it as: *"After calling the function, is the result what it should be?"*

### Type 2: `length_invariant` — "Are the sizes/lengths correct?"

This checks a **number** related to how much data is in a buffer, list, or memory region.

Think of it as: *"After calling the function, how many elements/bytes are there now?"*

### Type 3: `frame_condition` — "Was something correctly left alone?"

This checks that a field or pointer was **not accidentally changed** by the function.

Think of it as: *"After calling the function, is this thing still the same as before?"*

---

## The single most reliable test

**Look at the right-hand side of the assertion.**

```
assert( FIELD  ==  RIGHT_HAND_SIDE )
         ↑              ↑
    what's checked    compare to what?
```

| Right-hand side is... | Classification |
|----------------------|----------------|
| `old.something` or `old_X` (a snapshot before the call) | → `frame_condition` (checking something is unchanged) |
| `NULL` or `null` or `0` (a constant zero value) | → `validity_predicate` (checking a result) |
| An input parameter (same name as a function argument) | → `validity_predicate` (checking result stored correctly) |
| A `len`, `length`, `capacity`, `size` field or variable | → `length_invariant` |
| `SIZE_MAX`, `UINT32_MAX`, `UINT64_MAX`, or arithmetic | → `validity_predicate` |
| A named function like `aws_X_is_valid(...)` | → `validity_predicate` |

---

## Worked examples — learn by doing

Read each example, guess the answer, then check.

---

### Example 1

```
Function: aws_byte_buf_append
assert: old_from.len == from.len
```

**Ask:** What is the right-hand side? → `from.len`  
**Ask:** Is `old_from.len` a snapshot? → Yes, `old_` prefix = snapshot before the call  
**Ask:** Expected delta = 0? → Yes, `from` buffer should not be modified by `append`  
→ **`frame_condition`** ✓

---

### Example 2

```
Function: aws_byte_buf_from_empty_array
assert: buf.len == 0
```

**Ask:** What is the right-hand side? → `0` (a constant)  
**Ask:** Is it comparing to an old snapshot? → No  
**Ask:** Is it about a length/capacity field? → `len` → Yes  
→ **`length_invariant`** ✓

*(Trick question: the constant `0` on the right might look like "nothing changed," but this is NOT frame_condition — the function SET the length to 0, it didn't leave it unchanged. After initialization, the len field now equals 0.)*

---

### Example 3

```
Function: aws_ring_buffer_init
assert: aws_ring_buffer_is_valid(ring_buf)
```

**Ask:** Is this a named validation function? → Yes, `aws_X_is_valid()`  
→ **`validity_predicate`** ✓

*(These library-specific predicates always check structural correctness → always validity_predicate.)*

---

### Example 4

```
Function: aws_ring_buffer_release
assert: ring_buf.allocator == ring_buf_old.allocator
```

**Ask:** Right-hand side is `ring_buf_old.allocator` — is this an old snapshot? → Yes  
**Ask:** Expected delta = 0? → Yes, `release` should not change the allocator  
→ **`frame_condition`** ✓

---

### Example 5

```
Function: aws_array_list_pop_back
assert: list.length == old.length - 1
```

**Ask:** Right-hand side is `old.length - 1` — does it reference a snapshot? → Yes  
**Ask:** Expected delta = 0? → **NO** — the length SHOULD decrease by 1 after pop  
**Ask:** Is the field a length field? → Yes, `length`  
→ **`length_invariant`** ✓

*(This is the hardest case: has `old.` prefix but is NOT frame_condition because the delta is -1. The rule: frame requires the field to be UNCHANGED. If it changed, it's length_invariant.)*

---

### Example 6

```
Function: aws_array_list_init_dynamic
assert: list.alloc == allocator
```

**Ask:** Right-hand side is `allocator` — is that an input parameter to the function? → Yes  
**Ask:** Is this checking the allocator was stored correctly? → Yes  
→ **`validity_predicate`** ✓

*(Tricky: `list.alloc == something` looks like frame_condition, but the RHS is the function's INPUT PARAMETER, not an old snapshot. The function SET this field. Checking it was set correctly → validity.)*

---

### Example 7

```
Function: aws_add_size_checked  
assert: (b > 0) && (a > (uint64_max - b))
```

**Ask:** Is this about len/capacity/size? → No  
**Ask:** Is this checking a function result? → Yes — checking the overflow condition  
→ **`validity_predicate`** ✓

---

### Example 8

```
Function: aws_byte_buf_clean_up_secure
assert: buf.allocator == null
```

**Ask:** Right-hand side is `null` (constant zero)  
**Ask:** Is this comparing to an old snapshot? → No  
**Ask:** Did the cleanup SET the allocator to null? → Yes  
→ **`validity_predicate`** ✓

*(Again: `null` on the right ≠ frame_condition. The function changed the allocator TO null. Checking the result → validity.)*

---

## The one pattern that always confuses people

**`field == old_something` — is it frame or length?**

| Assertion | Category | Why |
|-----------|----------|-----|
| `buf.capacity == old.capacity` | frame_condition | capacity should NOT change during append → delta=0 |
| `buf.len == old.len + bytes` | length_invariant | len SHOULD increase by bytes → delta≠0 |
| `list.length == old.length - 1` | length_invariant | length SHOULD decrease by 1 → delta≠0 |
| `list.alloc == old.alloc` | frame_condition | allocator should NOT change → delta=0 |

**Key question:** Was this field supposed to change during the function call?
- If NO → `frame_condition`
- If YES (even by a specific amount) → `length_invariant` (if it's a size/length field)

---

## CBMC diagnostic format

Some assertions appear as: `ring_buf != ((void *)0) check failed`

This is CBMC's output format for null-pointer checks. Read it as:
- `ring_buf != ((void *)0)` = `ring_buf != NULL`

Always classify these as `validity_predicate` (null check on a pointer).

---

## Quick reference card

```
Is it a named function like aws_X_is_valid()?  → validity_predicate

Does it contain len / length / capacity / size / offset?
  AND right-hand side is NOT an old_X snapshot?  → length_invariant
  AND right-hand side IS old_X but delta ≠ 0?    → length_invariant  
  AND right-hand side IS old_X AND delta = 0?     → frame_condition

Right-hand side is NULL / null / 0 / constant?   → validity_predicate
Right-hand side is an input parameter?            → validity_predicate
Right-hand side is arithmetic / overflow check?   → validity_predicate

Right-hand side is old_X AND field is unchanged?  → frame_condition
```

---

## 5-question warm-up (answers below)

Classify each. Don't look at the answers first.

1. `assert(buf.buffer == NULL)` — after `aws_byte_buf_clean_up`
2. `assert(list.length == 0)` — after `aws_array_list_clear`
3. `assert(dest->len == src->len)` — after `aws_byte_buf_init_copy`
4. `assert(list.data == old.data)` — after `aws_array_list_clear`
5. `assert(aws_linked_list_is_valid(&list))` — after `aws_linked_list_push_back`

---

**Answers:**

1. `validity_predicate` — `NULL` is a constant; buffer was SET to null by cleanup
2. `length_invariant` — `len` field, value is 0 (constant, not old snapshot)
3. `length_invariant` — `len` field, comparing to src's length (input parameter)
4. `frame_condition` — `old.data` is a snapshot; list data pointer should be unchanged after clear
5. `validity_predicate` — named validity predicate `aws_linked_list_is_valid()`

---

## You are ready

If you got all 5 warm-up questions right, you are ready to start the annotation.
If you missed any, re-read the relevant section above.

**Reminder:** Use `notes` column for any assertion you are genuinely unsure about. It is better to note your uncertainty than to guess silently.

Valid labels: `validity_predicate` | `length_invariant` | `frame_condition`
