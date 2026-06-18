#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE (1024)
#endif

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_byte_buf src;
    struct aws_allocator *allocator = aws_default_allocator();

    /* bound and initialize src */
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;

    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    /* src must not be modified */
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);

    /* dest must be valid after the call */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        if (old_src.buffer == NULL) {
            /* src had null buffer → dest zeroed except allocator */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.allocator == allocator);
        } else {
            /* src had non-null buffer, allocation succeeded */
            assert(dest.allocator == allocator);
            assert(dest.len == old_src.len);
            assert(dest.capacity == old_src.capacity);
            assert(dest.buffer != old_src.buffer);
            if (old_src.len > 0) {
                assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
            }
        }
    } else {
        /* allocation failed, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }
}
