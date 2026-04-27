// === STEP 1: SUCCESS PATH ===
// When aws_byte_cursor_eq returns true (or the successful value):
//   - a.ptr: UNCHANGED
//   - a.len: UNCHANGED
//   - b.ptr: UNCHANGED
//   - b.len: UNCHANGED

// === STEP 2: FAILURE PATH ===
// When aws_byte_cursor_eq returns false (or fails):
//   - a.ptr: UNCHANGED
//   - a.len: UNCHANGED
//   - b.ptr: UNCHANGED
//   - b.len: UNCHANGED

// === STEP 3: FRAME CONDITIONS ===
// For EVERY field of EVERY struct parameter, mark CHANGED or UNCHANGED:
//   a (struct aws_byte_cursor):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always
//   b (struct aws_byte_cursor):
//     - ptr: UNCHANGED always
//     - len: UNCHANGED always

// === STEP 4: VALIDITY INVARIANTS ===
//   - aws_byte_cursor_is_valid(&a): YES (must hold after call)
//   - aws_byte_cursor_is_valid(&b): YES (must hold after call)

#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_eq_harness() {
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    // Initialize cursors with allocated buffers
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    // Save old state
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    // Assume cursors are valid
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    // Call the function under test
    bool result = aws_byte_cursor_eq(&a, &b);

    // Assert frame conditions
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    // Assert validity invariants
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
