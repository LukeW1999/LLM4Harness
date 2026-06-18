#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness(void) {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf dest;

    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer old_src_bytes = {0};
    if (src.buffer != NULL && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &old_src_bytes);
    }

    int result = aws_byte_buf_init_copy(&dest, alloc, &src);

    assert(aws_byte_buf_is_valid(&dest));

    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.buffer == old_src.buffer);
    assert(src.allocator == old_src.allocator);
    assert(aws_byte_buf_is_valid(&src));

    if (result == AWS_OP_SUCCESS) {
        if (old_src.buffer == NULL) {
            assert(dest.len == 0);
            assert(dest.capacity == 0);
            assert(dest.buffer == NULL);
            assert(dest.allocator == alloc);
        } else {
            assert(dest.len == old_src.len);
            assert(dest.capacity == old_src.capacity);
            assert(dest.buffer != NULL);
            assert(dest.allocator == alloc);
            if (old_src.len > 0) {
                assert_bytes_match(dest.buffer, old_src.buffer, old_src.len);
            }
        }
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }
}
