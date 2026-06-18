#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_cursor old_src = src;

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf dest;
    AWS_ZERO_STRUCT(dest);

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
        assert(aws_byte_cursor_is_valid(&src));

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);

        assert(src.len == old_src.len);
        assert(src.ptr == old_src.ptr);
        assert(aws_byte_cursor_is_valid(&src));

        assert(aws_byte_buf_is_valid(&dest));
    }
}
