// === STEP 1: SUCCESS PATH ===
// aws_byte_cursor_from_array always succeeds (no failure path).
// It returns a struct aws_byte_cursor with:
//   - cur.ptr = (uint8_t *)bytes
//   - cur.len = len
//
// === STEP 2: FAILURE PATH ===
// No failure path exists. The function always returns a cursor.
//
// === STEP 3: FRAME CONDITIONS ===
// The function takes const void *bytes and size_t len (not structs passed by pointer).
// The returned struct aws_byte_cursor:
//   - ptr: SET to (uint8_t *)bytes
//   - len: SET to len
// The input bytes pointer is not modified.
//
// === STEP 4: VALIDITY INVARIANTS ===
// The returned cursor is valid (aws_byte_cursor_is_valid) if:
//   - ptr is non-null or len is 0
//   - ptr points to at least len bytes of valid memory
// We need to ensure the input bytes array is valid for len bytes.

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

void aws_byte_cursor_from_array_harness(void) {
    /* Non-deterministic inputs */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a buffer of at least len bytes (or NULL if len == 0) */
    uint8_t *bytes = NULL;
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    }

    /* Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* === Assertions from Step 1 === */
    /* result.ptr must equal (uint8_t *)bytes */
    assert(result.ptr == (uint8_t *)bytes);
    /* result.len must equal len */
    assert(result.len == len);

    /* === Assertions from Step 4 === */
    /* The returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&result));
}
