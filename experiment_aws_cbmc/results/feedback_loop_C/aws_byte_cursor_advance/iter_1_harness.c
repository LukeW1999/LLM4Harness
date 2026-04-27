// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_advance returns true (or the successful value):
//   - cursor.ptr: CHANGES to cursor.ptr + len
//   - cursor.len: CHANGES to cursor.len - len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_advance returns false (or fails):
//   - cursor.ptr: UNCHANGED
//   - cursor.len: UNCHANGED
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

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    size_t len = nondet_size_t();

    // Ensure the cursor is bounded and valid
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    // Save old state
    struct aws_byte_cursor old_cursor = cursor;

    // Call the function
    bool result = aws_byte_cursor_advance(&cursor, len);

    // Assertions for success path
    if (result) {
        assert(cursor.ptr == old_cursor.ptr + len);
        assert(cursor.len == old_cursor.len - len);
    } else {
        // Assertions for failure path
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    // Frame conditions
    assert(aws_byte_cursor_is_valid(&cursor));
}
