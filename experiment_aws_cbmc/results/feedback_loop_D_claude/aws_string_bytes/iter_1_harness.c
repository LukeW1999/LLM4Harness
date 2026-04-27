// === STEP 1: SUCCESS PATH ===
// aws_string_bytes returns str->bytes (a pointer to the bytes array inside the string)
// This is a simple field access, no state changes occur.
// Return value: pointer equal to str->bytes
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always returns str->bytes as long as str is valid (non-null)
// No state changes in any case.
//
// === STEP 3: FRAME CONDITIONS ===
// param: str (const struct aws_string *)
//   - allocator: UNCHANGED always
//   - len: UNCHANGED always
//   - bytes: UNCHANGED always
// The function is purely a getter, no mutations occur.
//
// === STEP 4: VALIDITY INVARIANTS ===
// - aws_string_is_valid(str): YES (must hold before and after call)
// - Return value must equal str->bytes

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <assert.h>

void aws_string_bytes_harness(void) {
    /* Allocate a non-deterministic aws_string */
    /* We need a valid aws_string with allocated bytes */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate the string structure with enough space for bytes[len+1] */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Set up the string fields - cast away const for initialization */
    *(size_t *)&str->len = len;
    /* allocator can be anything (including NULL for static strings) */

    /* Assume the string is valid */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state */
    const struct aws_allocator *old_allocator = str->allocator;
    const size_t old_len = str->len;

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* === ASSERTIONS === */

    /* Return value must equal str->bytes */
    assert(result == str->bytes);

    /* Frame conditions: nothing changed */
    assert(str->allocator == old_allocator);
    assert(str->len == old_len);

    /* Validity invariant still holds */
    assert(aws_string_is_valid(str));
}
