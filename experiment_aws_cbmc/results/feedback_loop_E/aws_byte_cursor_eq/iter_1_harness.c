#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    /* Declare and bound data structures */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Assert postconditions for BOTH success and failure paths */
    /* For aws_byte_cursor_eq, the result is a boolean, so we don't have AWS_OP_SUCCESS/AWS_OP_ERR paths */
    /* Instead, we assert based on the expected behavior of the function */
    assert(result == aws_array_eq(a.ptr, a.len, b.ptr, b.len));

    /* Assert unchanged fields (implied — Doxygen rarely lists these) */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    /* Assert validity invariants always holds */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
