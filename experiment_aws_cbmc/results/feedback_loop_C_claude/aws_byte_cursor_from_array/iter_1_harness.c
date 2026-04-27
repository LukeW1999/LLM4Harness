// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_from_array always succeeds (no error return).
// It returns a struct aws_byte_cursor by value with:
//   - result.ptr: SET to (uint8_t *)bytes
//   - result.len: SET to len
//
// === STEP 2: FAILURE PATH ===
// There is no failure path. The function always returns a cursor.
//
// === STEP 3: FRAME CONDITIONS ===
// The function takes const void *bytes and size_t len as inputs.
// It returns a new aws_byte_cursor by value.
// No input parameters are modified.
// result (struct aws_byte_cursor):
//   - result.ptr: CHANGED to (uint8_t *)bytes
//   - result.len: CHANGED to len
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&result): YES (must hold after call)
//   - The returned cursor's ptr equals the input bytes pointer
//   - The returned cursor's len equals the input len

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

void aws_byte_cursor_from_array_harness(void) {
    /* Non-deterministic inputs */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a buffer of the given length (or NULL if len == 0) */
    uint8_t *bytes;
    if (len == 0) {
        /* NULL is allowed when len is 0 per the spec */
        bytes = NULL;
    } else {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* === Assertions from Step 1 === */
    /* result.ptr must equal the input bytes pointer */
    assert(result.ptr == (uint8_t *)bytes);

    /* result.len must equal the input len */
    assert(result.len == len);

    /* === Assertions from Step 4 === */
    /* The returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}
