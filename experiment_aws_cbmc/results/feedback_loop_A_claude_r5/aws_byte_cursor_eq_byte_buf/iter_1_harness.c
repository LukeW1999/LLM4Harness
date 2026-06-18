#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_byte_buf_harness(void) {
    /* 1. Declare and set up aws_byte_cursor */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* 2. Declare and set up aws_byte_buf */
    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* 3. Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* 5. Assert postconditions */

    /* The function returns true iff the contents are equivalent */
    /* Both inputs are read-only — no fields should change */

    /* Cursor fields unchanged */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);

    /* Buffer fields unchanged */
    assert(b.len == old_b.len);
    assert(b.buffer == old_b.buffer);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* Validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* If lengths differ, result must be false */
    if (a.len != b.len) {
        assert(result == false);
    }

    /* If both lengths are 0, result must be true (empty arrays are equal) */
    if (a.len == 0 && b.len == 0) {
        assert(result == true);
    }
}
