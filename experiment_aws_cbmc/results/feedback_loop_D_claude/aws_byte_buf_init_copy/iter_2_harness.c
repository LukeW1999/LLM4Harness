#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_init_copy_harness() {
    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Source buffer - nondeterministic but valid */
    struct aws_byte_buf src;
    /* Bound the buffer size for tractability */
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));

    /* Destination buffer - uninitialized */
    struct aws_byte_buf dest;

    /* Save old src state */
    struct aws_byte_buf old_src = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* src must be unchanged */
    assert(src.len == old_src.len);
    assert(src.buffer == old_src.buffer);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* Case A: src had no buffer */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* Case B: src had a buffer, allocation succeeded */
            assert(dest.buffer != NULL);
            assert(dest.len == src.len);
            assert(dest.capacity == src.capacity);
            /* Verify bytes were copied correctly */
            assert_bytes_match(dest.buffer, src.buffer, src.len);
        }
    } else {
        /* Failure path: dest is zeroed */
        assert(result == AWS_OP_ERR);
        /* src must have had a buffer (otherwise we'd have succeeded) */
        assert(src.buffer != NULL);
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
