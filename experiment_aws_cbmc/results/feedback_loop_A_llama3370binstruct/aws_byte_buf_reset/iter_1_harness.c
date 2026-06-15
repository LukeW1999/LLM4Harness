#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_buf src;

    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_dest = dest;
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        assert_bytes_match(dest.buffer, src.buffer, src.len);
    } else {
        assert_bytes_match(dest.buffer, old_dest.buffer, old_dest.len);
        assert(dest.len == old_dest.len);
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
    }

    assert(dest.allocator == allocator);
    assert(aws_byte_buf_is_valid(&dest));
}
