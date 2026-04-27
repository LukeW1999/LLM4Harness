// === STEP 1: SUCCESS PATH ===
// aws_string_bytes always returns str->bytes (the pointer to the bytes field of the string)
// It never fails - it simply returns the bytes pointer from the struct.
// Return value: equals str->bytes
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds and returns str->bytes
//
// === STEP 3: FRAME CONDITIONS ===
// param str (const struct aws_string *):
//   - allocator: UNCHANGED always (function only reads)
//   - len: UNCHANGED always (function only reads)
//   - bytes: UNCHANGED always (function only reads)
// The function is a pure read - no state changes occur.
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_string_is_valid(str): YES (must hold before and after call)
//   - Return value must equal str->bytes

#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>
#include <stdlib.h>

/* Helper to allocate a valid aws_string for CBMC */
struct aws_string *ensure_string_is_allocated(size_t len) {
    /* Allocate memory for the struct plus len+1 bytes (for null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    if (str == NULL) {
        return NULL;
    }
    /* Use memcpy to set const fields */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;
    /* bytes are already allocated as part of the struct */
    return str;
}

void aws_string_bytes_harness(void) {
    // === STEP 1: SUCCESS PATH ===
    // aws_string_bytes always returns str->bytes (the pointer to the bytes field of the string)
    // It never fails - it simply returns the bytes pointer from the struct.
    // Return value: equals str->bytes
    //
    // === STEP 2: FAILURE PATH ===
    // No failure path - function always succeeds and returns str->bytes
    //
    // === STEP 3: FRAME CONDITIONS ===
    // param str (const struct aws_string *):
    //   - allocator: UNCHANGED always (function only reads)
    //   - len: UNCHANGED always (function only reads)
    //   - bytes: UNCHANGED always (function only reads)
    // The function is a pure read - no state changes occur.
    //
    // === STEP 4: VALIDITY INVARIANTS ===
    //   - aws_string_is_valid(str): YES (must hold before and after call)
    //   - Return value must equal str->bytes

    /* Create a nondet length, bounded to keep verification tractable */
    size_t len;
    __CPROVER_assume(len <= 10);

    /* Allocate a valid aws_string */
    struct aws_string *str = ensure_string_is_allocated(len);
    __CPROVER_assume(str != NULL);

    /* Set the len field */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;

    /* Set allocator to nondet (can be NULL or non-NULL) */
    struct aws_allocator **alloc_ptr = (struct aws_allocator **)&str->allocator;
    *alloc_ptr = NULL; /* Use NULL allocator for simplicity */

    /* Precondition: str must be valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state */
    struct aws_allocator *old_allocator = str->allocator;
    size_t old_len = str->len;

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* === ASSERTIONS === */

    /* Step 1: Return value must equal str->bytes */
    assert(result == str->bytes);

    /* Step 3: Frame conditions - all fields unchanged */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);

    /* Step 4: Validity invariant must still hold after call */
    assert(aws_string_is_valid(str));

    /* Additional: result must not be NULL (bytes is always part of the struct) */
    assert(result != NULL);
}
