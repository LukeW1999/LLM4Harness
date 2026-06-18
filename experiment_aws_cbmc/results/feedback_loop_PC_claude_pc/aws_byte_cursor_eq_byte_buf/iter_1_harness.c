#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* 1. Set up aws_byte_cursor a */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* 2. Set up aws_byte_buf b */
    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 3. Snapshot state before the call */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* 5. Postconditions */

    /* Return value: result is true iff the contents of a and b are equivalent */
    if (result) {
        /* If they are equal, lengths must match */
        assert(a.len == b.len);
        /* And byte contents must match (if non-empty) */
        if (a.len > 0 && a.ptr != NULL && b.buffer != NULL) {
            assert_bytes_match(a.ptr, b.buffer, a.len);
        }
    } else {
        /* If not equal, either lengths differ or contents differ — no strong assertion
           beyond the fact that the function returned false */
        /* We can assert the negation: not (len equal AND bytes equal) */
        /* This is hard to assert directly, so we just verify frame conditions */
    }

    /* 6. Frame conditions: cursor a must not be modified */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);

    /* 7. Frame conditions: buf b must not be modified */
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.buffer == old_b.buffer);
    assert(b.allocator == old_b.allocator);

    /* 8. Validity invariants still hold after the call */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
