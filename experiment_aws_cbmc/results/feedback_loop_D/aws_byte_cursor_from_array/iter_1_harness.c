// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_from_array returns AWS_OP_SUCCESS (or the successful value):
//   - cur.ptr: CHANGES to (uint8_t *)bytes
//   - cur.len: CHANGES to len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_from_array returns AWS_OP_ERR (or fails):
//   - cur.ptr: UNCHANGED (not applicable, as there is no failure path)
//   - cur.len: UNCHANGED (not applicable, as there is no failure path)
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   cur (struct aws_byte_cursor):
//     - ptr: CHANGED on success, UNCHANGED on failure (not applicable)
//     - len: CHANGED on success, UNCHANGED on failure (not applicable)
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&cur): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_from_array_harness() {
    const void *bytes = (const void *)nondet_ptr();
    size_t len = nondet_size_t();

    struct aws_byte_cursor cur;
    cur.ptr = (uint8_t *)nondet_ptr();
    cur.len = nondet_size_t();

    struct aws_byte_cursor old_cur = cur;

    cur = aws_byte_cursor_from_array(bytes, len);

    // Frame conditions
    assert(cur.ptr == (uint8_t *)bytes);
    assert(cur.len == len);

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&cur));
}
