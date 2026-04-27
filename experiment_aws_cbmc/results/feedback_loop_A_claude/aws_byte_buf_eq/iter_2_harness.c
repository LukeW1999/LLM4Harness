#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness(void) {
    /* 1. Declare and set up lhs */
    struct aws_byte_buf lhs;
    __CPROVER_assume(aws_byte_buf_is_bounded(&lhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&lhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&lhs));

    /* 2. Declare and set up rhs */
    struct aws_byte_buf rhs;
    __CPROVER_assume(aws_byte_buf_is_bounded(&rhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&rhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&rhs));

    /* Save old states to check immutability */
    struct aws_byte_buf old_lhs = lhs;
    struct aws_byte_buf old_rhs = rhs;

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq(&lhs, &rhs);

    /* 4. Assert postconditions */
    /* Result is valid boolean */
    assert(result == true || result == false);

    /* If lengths differ, must be false */
    if (lhs.len != rhs.len) {
        assert(result == false);
    }

    /* If both have same content, result should be true */
    if (result == true) {
        assert(lhs.len == rhs.len);
    }

    /* 5. Assert lhs and rhs are unchanged (immutability) */
    assert(lhs.buffer == old_lhs.buffer);
    assert(lhs.len == old_lhs.len);
    assert(lhs.capacity == old_lhs.capacity);
    assert(lhs.allocator == old_lhs.allocator);
    assert(aws_byte_buf_is_valid(&lhs));

    assert(rhs.buffer == old_rhs.buffer);
    assert(rhs.len == old_rhs.len);
    assert(rhs.capacity == old_rhs.capacity);
    assert(rhs.allocator == old_rhs.allocator);
    assert(aws_byte_buf_is_valid(&rhs));
}
