// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_from_array returns AWS_OP_SUCCESS (or the successful value):
//   - cur.ptr: CHANGES to (uint8_t *)bytes
//   - cur.len: CHANGES to len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_from_array returns AWS_OP_ERR (or fails):
//   - cur.ptr: UNCHANGED
//   - cur.len: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   cur (struct aws_byte_cursor):
//     - ptr: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&cur): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_array_harness() {
    const void *bytes = nd;
    size_t len = nd;
    struct aws_byte_cursor cur_old;
    cur_old.ptr = nd;
    cur_old.len = nd;

    struct aws_byte_cursor cur = aws_byte_cursor_from_array(bytes, len);

    // Success path assertions
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    // Validity invariant
    assert(aws_byte_cursor_is_valid(&cur));
}
