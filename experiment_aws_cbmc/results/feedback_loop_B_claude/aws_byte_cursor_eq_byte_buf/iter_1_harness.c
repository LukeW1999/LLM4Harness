#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* 1. Declare and set up the cursor */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* 2. Declare and set up the byte buf */
    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 3. Save old state for immutability checks */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* 5. Assert postconditions */

    /* The function returns true iff the arrays are equal in content and length */
    if (result) {
        /* If true, lengths must match (aws_array_eq returns false if lengths differ) */
        assert(a.len == b.len);
    }

    /* The function is read-only — neither cursor nor buf should be modified */
    /* Cursor fields unchanged */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);

    /* Buf fields unchanged */
    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* 6. Validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
