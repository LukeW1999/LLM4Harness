#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_eq_harness(void) {
    /* Set up first buffer */
    struct aws_byte_buf lhs;
    __CPROVER_assume(aws_byte_buf_is_bounded(&lhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&lhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&lhs));

    /* Set up second buffer */
    struct aws_byte_buf rhs;
    __CPROVER_assume(aws_byte_buf_is_bounded(&rhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&rhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&rhs));

    /* Save old states to verify buffers are not modified */
    struct aws_byte_buf old_lhs = lhs;
    struct aws_byte_buf old_rhs = rhs;

    /* Call function under test */
    bool result = aws_byte_buf_eq(&lhs, &rhs);

    /* Postconditions */
    /* If lengths differ, result must be false */
    if (lhs.len != rhs.len) {
        assert(!result);
    }

    /* lhs must not be modified */
    assert(lhs.buffer == old_lhs.buffer);
    assert(lhs.len == old_lhs.len);
    assert(lhs.capacity == old_lhs.capacity);
    assert(lhs.allocator == old_lhs.allocator);

    /* rhs must not be modified */
    assert(rhs.buffer == old_rhs.buffer);
    assert(rhs.len == old_rhs.len);
    assert(rhs.capacity == old_rhs.capacity);
    assert(rhs.allocator == old_rhs.allocator);

    /* Both buffers must still be valid */
    assert(aws_byte_buf_is_valid(&lhs));
    assert(aws_byte_buf_is_valid(&rhs));
}
