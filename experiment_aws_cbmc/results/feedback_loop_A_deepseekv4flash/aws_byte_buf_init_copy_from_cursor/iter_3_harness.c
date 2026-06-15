#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_cursor src;
    size_t max_len = MAX_BUFFER_SIZE;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, max_len));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct store_byte_from_buffer src_bytes;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_bytes);
    }

    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    struct aws_byte_buf dest;
    dest.allocator = NULL;
    dest.buffer = NULL;
    dest.len = 0;
    dest.capacity = 0;

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
        if (src.len > 0) {
            assert_byte_from_buffer_matches(src.ptr, &src_bytes);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
        assert(aws_byte_cursor_is_valid(&src));
        if (src.len > 0) {
            assert_byte_from_buffer_matches(src.ptr, &src_bytes);
        }
    }

    assert(aws_byte_cursor_is_valid(&src));
}
