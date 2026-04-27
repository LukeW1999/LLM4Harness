// === STEP 1: SUCCESS PATH ===
// aws_byte_buf_from_array always returns a struct (not an int), so there's no success/failure path.
// The function creates a new aws_byte_buf from a given bytes pointer and len.
// On return:
//   - result.buffer: SET to (len > 0) ? (uint8_t *)bytes : NULL
//   - result.len: SET to len
//   - result.capacity: SET to len
//   - result.allocator: SET to NULL
//
// === STEP 2: FAILURE PATH ===
// No failure path - function always succeeds (returns struct by value).
//
// === STEP 3: FRAME CONDITIONS ===
// result (struct aws_byte_buf, returned by value):
//   - result.buffer: SET to (len > 0) ? (uint8_t *)bytes : NULL
//   - result.len: SET to len
//   - result.capacity: SET to len
//   - result.allocator: SET to NULL
// The input bytes array is not modified.
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&result): YES (must hold after call, per postcondition)
//   - result.len == len: YES
//   - result.capacity == len: YES
//   - result.allocator == NULL: YES
//   - result.buffer == NULL iff len == 0: YES

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness(void) {
    /* Non-deterministic inputs */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a writable buffer of size len (or NULL if len == 0) */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    struct aws_byte_buf result = aws_byte_buf_from_array(bytes, len);

    /* === STEP 1 assertions: verify all fields of result === */
    /* buffer: (len > 0) ? (uint8_t *)bytes : NULL */
    if (len > 0) {
        assert(result.buffer == (uint8_t *)bytes);
    } else {
        assert(result.buffer == NULL);
    }

    /* len field equals input len */
    assert(result.len == len);

    /* capacity equals len */
    assert(result.capacity == len);

    /* allocator is NULL */
    assert(result.allocator == NULL);

    /* === STEP 4: validity invariant === */
    assert(aws_byte_buf_is_valid(&result));
}
