#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_cursor a;
    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* 3. Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert_bytes_match(a.ptr, b.ptr, a.len);
    } else {
        /* No changes expected on failure */
        assert(a.ptr == old_a.ptr);
        assert(a.len == old_a.len);
        assert(b.ptr == old_b.ptr);
        assert(b.len == old_b.len);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    /* No additional fields to check for cursors */

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
