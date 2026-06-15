#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator;

    /* Preconditions: allocator and dest must not be NULL */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save src state for immutability checks */
    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* src must remain unchanged */
    assert(aws_byte_buf_is_valid(&src));
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest is valid and correctly initialized */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);

        if (src.buffer == NULL) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* On failure, dest is zeroed (allocator set to NULL) */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
