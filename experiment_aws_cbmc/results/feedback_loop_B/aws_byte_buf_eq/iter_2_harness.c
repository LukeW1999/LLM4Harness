#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_eq_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf lhs;
    struct aws_byte_buf rhs;
    __CPROVER_assume(aws_byte_buf_is_bounded(&lhs, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&rhs, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&lhs);
    ensure_byte_buf_has_allocated_buffer_member(&rhs);
    __CPROVER_assume(aws_byte_buf_is_valid(&lhs));
    __CPROVER_assume(aws_byte_buf_is_valid(&rhs));

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_lhs = lhs;
    struct aws_byte_buf old_rhs = rhs;

    /* 3. Call function under test */
    bool result = aws_byte_buf_eq(&lhs, &rhs);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result) {
        assert(lhs.len == rhs.len);
        if (lhs.buffer && rhs.buffer) {
            assert_bytes_match(lhs.buffer, rhs.buffer, lhs.len);
        } else {
            assert(lhs.buffer == rhs.buffer);
        }
    } else {
        // If they are not equal, we cannot assume anything specific about their contents
        // but we can assert they are not both non-null and identical up to length
        if (lhs.buffer && rhs.buffer && lhs.len == rhs.len) {
            assert(!assert_bytes_match(lhs.buffer, rhs.buffer, lhs.len));
        }
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(lhs.len == old_lhs.len);
    assert(lhs.capacity == old_lhs.capacity);
    assert(lhs.allocator == old_lhs.allocator);
    assert(lhs.buffer == old_lhs.buffer);

    assert(rhs.len == old_rhs.len);
    assert(rhs.capacity == old_rhs.capacity);
    assert(rhs.allocator == old_rhs.allocator);
    assert(rhs.buffer == old_rhs.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&lhs));
    assert(aws_byte_buf_is_valid(&rhs));
}
