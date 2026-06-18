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

    /* 3. Save old state */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_buf old_b = b;

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&a, &b);

    /* 5. Assert postconditions */

    /* The function returns true iff the contents are equivalent */
    if (result) {
        /* If equal, lengths must match */
        assert(a.len == b.len);
    }

    /* Neither the cursor nor the buffer should be modified */
    assert(a.len == old_a.len);
    assert(a.ptr == old_a.ptr);

    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.buffer == old_b.buffer);
    assert(b.allocator == old_b.allocator);

    /* 6. Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
