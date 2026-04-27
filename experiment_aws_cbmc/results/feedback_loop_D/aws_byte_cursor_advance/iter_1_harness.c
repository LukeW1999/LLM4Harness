// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_advance returns AWS_OP_SUCCESS (or the successful value):
//   - cursor->ptr: CHANGES to cursor->ptr + len
//   - cursor->len: CHANGES to cursor->len - len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_advance returns AWS_OP_ERR (or fails):
//   - cursor->ptr: UNCHANGED
//   - cursor->len: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   cursor (struct aws_byte_cursor):
//     - ptr: CHANGED on success, UNCHANGED on failure
//     - len: CHANGED on success, UNCHANGED on failure
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&cursor): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <cbmc_proof/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    size_t len;
    struct aws_byte_cursor old_cursor = cursor;
    size_t old_len = len;

    // Ensure the cursor is valid and bounded
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    // Ensure len is within a reasonable range
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    int result = aws_byte_cursor_advance(&cursor, len);

    // Check frame conditions and postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(cursor.ptr == old_cursor.ptr + old_len);
        assert(cursor.len == old_cursor.len - old_len);
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    // Validity invariants
    assert(aws_byte_cursor_is_valid(&cursor));
}
