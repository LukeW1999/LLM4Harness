#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf src;
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = malloc(sizeof(*allocator));

    /* Preconditions */
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save old state of src for immutability check */
    struct aws_byte_buf old_src = src;

    /* Call function under verification */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest is valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->allocator equals the passed allocator */
        assert(dest.allocator == allocator);

        /* dest->len and dest->capacity match src */
        assert(dest.len == src.len);
        assert(dest.capacity == src.capacity);

        /* Buffer content matches if src had content */
        if (src.buffer != NULL) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        } else {
            /* src had null buffer, dest must also have null buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src is unchanged */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert(src.buffer == old_src.buffer);
    assert(aws_byte_buf_is_valid(&src));
}
