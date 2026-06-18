#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness() {
    struct aws_byte_buf lhs;
    __CPROVER_assume(aws_byte_buf_is_bounded(&lhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&lhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&lhs));

    struct aws_byte_buf rhs;
    __CPROVER_assume(aws_byte_buf_is_bounded(&rhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&rhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&rhs));

    struct aws_byte_buf old_lhs = lhs;
    struct aws_byte_buf old_rhs = rhs;

    struct store_byte_from_buffer lhs_storage;
    if (old_lhs.capacity > 0) {
        save_byte_from_array(old_lhs.buffer, old_lhs.capacity, &lhs_storage);
    }

    struct store_byte_from_buffer rhs_storage;
    if (old_rhs.capacity > 0) {
        save_byte_from_array(old_rhs.buffer, old_rhs.capacity, &rhs_storage);
    }

    bool expected = false;
    if (lhs.len == rhs.len) {
        expected = true;
        for (size_t i = 0; i < lhs.len; ++i) {
            if (lhs.buffer[i] != rhs.buffer[i]) {
                expected = false;
            }
        }
    }

    bool result = aws_byte_buf_eq(&lhs, &rhs);

    __CPROVER_assert(result == expected, "aws_byte_buf_eq returns whether the byte buffers are equal");

    __CPROVER_assert(lhs.buffer == old_lhs.buffer, "lhs buffer is unchanged");
    __CPROVER_assert(lhs.len == old_lhs.len, "lhs len is unchanged");
    __CPROVER_assert(lhs.capacity == old_lhs.capacity, "lhs capacity is unchanged");
    __CPROVER_assert(lhs.allocator == old_lhs.allocator, "lhs allocator is unchanged");

    __CPROVER_assert(rhs.buffer == old_rhs.buffer, "rhs buffer is unchanged");
    __CPROVER_assert(rhs.len == old_rhs.len, "rhs len is unchanged");
    __CPROVER_assert(rhs.capacity == old_rhs.capacity, "rhs capacity is unchanged");
    __CPROVER_assert(rhs.allocator == old_rhs.allocator, "rhs allocator is unchanged");

    if (old_lhs.capacity > 0) {
        assert_byte_from_buffer_matches(lhs.buffer, &lhs_storage);
    }

    if (old_rhs.capacity > 0) {
        assert_byte_from_buffer_matches(rhs.buffer, &rhs_storage);
    }

    __CPROVER_assert(aws_byte_buf_is_valid(&lhs), "lhs remains valid");
    __CPROVER_assert(aws_byte_buf_is_valid(&rhs), "rhs remains valid");
}
