# Second Annotator Instructions — 38-Item CBMC Property Classification

## Your task

Fill in the `category` column in `second_annotator_sheet.csv`.
Each row is a CBMC `assert()` statement from a proof harness that an LLM **missed**.
Classify WHY the LLM likely missed it using one of the 10 categories below.

**Time estimate: ~45–60 minutes for 38 items.**

---

## The 10 Categories

| Code | Name | One-line definition |
|------|------|---------------------|
| `VALIDITY_PRED` | Structural validity predicate | Calls a named `aws_X_is_valid()` or `s2n_X_validate()` predicate |
| `LEN_INVARIANT` | Length/size invariant | Constrains `len`, `capacity`, or `size` field relationship |
| `FRAME_COND` | Frame condition | Checks a field was NOT changed by the function |
| `STRUCT_PTR` | Struct pointer check | Checks a struct pointer is non-null or properly set |
| `LEN_CHANGE` | Length change | Checks a length/count changed by a specific amount on success |
| `TYPE_VARIANT` | Type-specific variant | Assertion for a type-specific path (e.g., u32 vs u64) |
| `OVERFLOW_ARITH` | Overflow guard | Guards against integer overflow in arithmetic |
| `ZERO_CHECK` | Zero/null check | Checks a value is zero, nonzero, or null |
| `RETVAL_SPEC` | Return value spec | Specifies the return value or return path condition |
| `CBMC_BUILTIN` | CBMC builtin | Uses `__CPROVER_r_ok`, `__CPROVER_same_object`, etc. |

---

## Decision rules (use the first that applies)

1. Does the assertion call a function named `*_is_valid*` or `*_validate*`? → **VALIDITY_PRED**
2. Does it compare `len`, `capacity`, or `size` to a computed value (unchanged or changed)? → **LEN_INVARIANT** (if unchanged/relational) or **LEN_CHANGE** (if delta: +1, -n, etc.)
3. Does it say a field equals its old/pre-call value? → **FRAME_COND**
4. Is it a null/non-null pointer check on a struct field? → **STRUCT_PTR**
5. Is it about u32 vs u64 overflow boundary conditions? → **TYPE_VARIANT** or **OVERFLOW_ARITH**
6. Does it use a `__CPROVER_` macro? → **CBMC_BUILTIN**
7. Does it check the return value / S2N_SUCCESS / AWS_OP_SUCCESS? → **RETVAL_SPEC**
8. Does it check `== 0` or `!= 0`? → **ZERO_CHECK**

---

## Example annotations

| property_text | category | reasoning |
|--------------|----------|-----------|
| `assert(aws_linked_list_is_valid(list))` | VALIDITY_PRED | Calls named validity predicate |
| `assert(buf->len <= buf->capacity)` | LEN_INVARIANT | len/capacity invariant |
| `assert(list->data == old_data_ptr)` | FRAME_COND | Field unchanged after operation |
| `assert(list->length == initial_length + 1)` | LEN_CHANGE | Length changed by +1 on success |
| `assert(buf != NULL)` | STRUCT_PTR | Null check on struct pointer |
| `assert((b > 0) && (a > (uint32_max - b)))` | TYPE_VARIANT | u32-specific overflow check |
| `assert(result == AWS_OP_SUCCESS)` | RETVAL_SPEC | Return value check |
| `assert(__CPROVER_r_ok(ptr, size))` | CBMC_BUILTIN | CBMC builtin |

---

## File to fill in

`second_annotator_sheet.csv` — add your category in the `category` column.
Save and return the file. Do NOT look up my annotations before finishing.
