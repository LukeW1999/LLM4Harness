#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_cursor old_src = src;

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0) {
            assert(dest.buffer != NULL);
            __CPROVER_assert(AWS_MEM_IS_WRITABLE(dest.buffer, src.len),
                             "dest.buffer must be writable for src.len bytes");
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(dest.buffer == NULL);
    }

    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    assert(aws_byte_cursor_is_valid(&src));
}
