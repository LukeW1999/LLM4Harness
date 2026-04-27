#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_init_copy_harness(void) {
    /* Set up src */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save src state to verify it is not modified */
    struct aws_byte_buf src_old = src;

    /* Use a valid allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Declare dest (uninitialized) */
    struct aws_byte_buf dest;

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator must be set to the provided allocator */
        assert(dest.allocator == allocator);

        if (src_old.buffer == NULL) {
            /* src->buffer was NULL: dest should be zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* src->buffer was non-NULL: dest should be a copy */
            assert(dest.len == src_old.len);
            assert(dest.capacity == src_old.capacity);
            assert(dest.buffer != NULL);
        }
    } else {
        /* Failure: dest should be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must not have been modified */
    assert(src.buffer == src_old.buffer);
    assert(src.len == src_old.len);
    assert(src.capacity == src_old.capacity);
    assert(src.allocator == src_old.allocator);

    /* src must still be valid */
    assert(aws_byte_buf_is_valid(&src));
}
