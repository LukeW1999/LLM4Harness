#include <stdbool.h>
#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_byte_cursor_eq_harness() {
    /* Declare and bound two cursors */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    /* Assume bounds (optional but typical for state-space reduction) */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Ensure allocated buffer members */
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    /* Ensure validity of both cursors */
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save old state for immutability checks */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Assert postconditions */

    /* 1. Result is a boolean (true or false) */
    assert(result == true || result == false);

    /* 2. Cursors are unchanged (const parameters) */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* 3. Validity invariants are preserved */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
