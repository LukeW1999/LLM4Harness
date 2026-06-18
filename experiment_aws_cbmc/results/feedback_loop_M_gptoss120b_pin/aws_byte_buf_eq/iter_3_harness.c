#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf a = {0};
    struct aws_byte_buf b = {0};

    /* Bound the buffers */
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));

    /* Ensure buffers have allocated memory */
    ensure_byte_buf_has_allocated_buffer_member(&a);
    ensure_byte_buf_has_allocated_buffer_member(&b);

    /* Assume buffers are valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&a));
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* 2. Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* 3. Postconditions */
    if (result) {
        /* Buffers must be equal in length and contents */
        assert(a.len == b.len);
        if (a.len > 0) {
            assert_bytes_match(a.buffer, b.buffer, a.len);
        }
    }

    /* Buffers must remain unchanged */
    assert(a.buffer == old_a.buffer);
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);

    assert(b.buffer == old_b.buffer);
    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);

    /* 4. Validity invariants */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));
}
