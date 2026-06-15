#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    /* Non-deterministic but bounded curves */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));

    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);

    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call the function */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Assert unchanged fields */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);
    assert(b.len == old_b.len);
    assert(b.ptr == old_b.ptr);

    /* Assert return value relationship with lengths */
    if (a.len != b.len) {
        assert(!result);
    }

    /* Assert validity invariants */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    /* Additional safety: if lengths are zero, both pointers may be NULL (valid per spec) */
    if (a.len == 0) {
        // Nothing to check
    }
    if (b.len == 0) {
        // Nothing to check
    }
}
