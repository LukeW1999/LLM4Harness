#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Initially zero dest */
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator; /* Non-deterministic pointer */

    /* Bound src and ensure it is valid */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Allocator must be non-null (as per precondition) */
    __CPROVER_assume(allocator != NULL);

    /* Save source state to verify immutability */
    struct aws_byte_buf old_src = src;

    /* Call the function */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postcondition: dest is valid */
    assert(aws_byte_buf_is_valid(&dest));

    /* Source must remain valid and unchanged */
    assert(aws_byte_buf_is_valid(&src));
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    if (src.len > 0) {
        assert_bytes_match(src.buffer, old_src.buffer, src.len);
    } else {
        assert(src.buffer == old_src.buffer);
    }

    if (result == AWS_OP_SUCCESS) {
        /* Success case */
        if (old_src.buffer == NULL) {
            /* src was empty/null buffer: dest should be zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* src had content: dest must have its own copy */
            assert(dest.buffer != NULL);
            assert(dest.len == old_src.len);
            assert(dest.capacity == old_src.capacity);
            assert(dest.allocator == allocator);
            assert_bytes_match(dest.buffer, old_src.buffer, dest.len);
        }
    } else {
        /* Failure case: dest must be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
