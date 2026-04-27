// === STEP 1: SUCCESS PATH ===
// When aws_byte_buf_append returns AWS_OP_SUCCESS (or the successful value):
//   - to.len: CHANGES to to.len + from.len
//   - to.buffer: CHANGES to have from.ptr's content appended starting at to.len
//
// === STEP 2: FAILURE PATH ===
// When aws_byte_buf_append returns AWS_OP_ERR (or fails):
//   - to.len: UNCHANGED
//   - to.buffer: UNCHANGED
//   - to.capacity: UNCHANGED
//   - to.allocator: UNCHANGED
//
// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   to (struct aws_byte_buf):
//     - len: CHANGED on success, UNCHANGED on failure
//     - buffer: CHANGED on success, UNCHANGED on failure
//     - capacity: UNCHANGED always
//     - allocator: UNCHANGED always
//   from (struct aws_byte_cursor):
//     - len: UNCHANGED always
//     - ptr: UNCHANGED always
//
// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_buf_is_valid(&to): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&from): YES

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_buf_append_harness() {
    struct aws_byte_buf to;
    struct aws_byte_cursor from;

    // Initialize to and from with arbitrary but valid values
    ensure_byte_buf_has_allocated_buffer_member(&to);
    ensure_byte_cursor_has_allocated_buffer_member(&from);

    // Save old state of to
    struct aws_byte_buf old_to = to;

    // Call the function under test
    int result = aws_byte_buf_append(&to, &from);

    // Postconditions
    if (result == AWS_OP_SUCCESS) {
        assert(to.len == old_to.len + from.len);
        assert_bytes_match(to.buffer + old_to.len, from.ptr, from.len);
    } else {
        assert(result == AWS_OP_ERR);
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
    }

    // Frame conditions
    assert(to.capacity == old_to.capacity);
    assert(to.allocator == old_to.allocator);
    assert(from.len == from.len); // Always unchanged
    assert(from.ptr == from.ptr); // Always unchanged

    // Validity invariants
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
