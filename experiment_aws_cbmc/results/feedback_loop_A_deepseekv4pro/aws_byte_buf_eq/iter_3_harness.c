#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_eq_harness() {
    /* 1. Declare and bound two byte buffers */
    struct aws_byte_buf lhs;
    struct aws_byte_buf rhs;

    __CPROVER_assume(aws_byte_buf_is_bounded(&lhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&lhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&lhs));

    __CPROVER_assume(aws_byte_buf_is_bounded(&rhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&rhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&rhs));

    /* Save old states for immutability checks */
    struct aws_byte_buf old_lhs = lhs;
    struct aws_byte_buf old_rhs = rhs;

    struct store_byte_from_buffer lhs_old_byte;
    struct store_byte_from_buffer rhs_old_byte;

    if (lhs.buffer != NULL) {
        save_byte_from_array(lhs.buffer, lhs.capacity, &lhs_old_byte);
    }
    if (rhs.buffer != NULL) {
        save_byte_from_array(rhs.buffer, rhs.capacity, &rhs_old_byte);
    }

    /* 2. Call function under test */
    bool result = aws_byte_buf_eq(&lhs, &rhs);

    /* 3. Assert postconditions */
    if (result) {
        /* Equality implies same length and content */
        assert(lhs.len == rhs.len);
        if (lhs.len > 0) {
            assert_bytes_match(lhs.buffer, rhs.buffer, lhs.len);
        }
    } else {
        /* Inequality: either lengths differ or bytes differ */
        if (lhs.len == rhs.len && lhs.len > 0) {
            /* If lengths equal but result false, bytes must differ */
            assert(!__CPROVER_forall { int i; (0 <= i && i < lhs.len) ==> lhs.buffer[i] == rhs.buffer[i] });
        }
    }

    /* 4. Assert immutability of both buffers */
    assert(old_lhs.len == lhs.len);
    assert(old_lhs.capacity == lhs.capacity);
    assert(old_lhs.allocator == lhs.allocator);
    assert(old_lhs.buffer == lhs.buffer);
    if (lhs.buffer != NULL) {
        assert_byte_from_buffer_matches(lhs.buffer, &lhs_old_byte);
    }

    assert(old_rhs.len == rhs.len);
    assert(old_rhs.capacity == rhs.capacity);
    assert(old_rhs.allocator == rhs.allocator);
    assert(old_rhs.buffer == rhs.buffer);
    if (rhs.buffer != NULL) {
        assert_byte_from_buffer_matches(rhs.buffer, &rhs_old_byte);
    }

    /* 5. Validity invariants still hold */
    assert(aws_byte_buf_is_valid(&lhs));
    assert(aws_byte_buf_is_valid(&rhs));
}
