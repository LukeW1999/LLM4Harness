#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(src.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(src.ptr != NULL || src.len == 0);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_cursor old_src = src;

    struct aws_byte_buf dest;

    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.allocator == alloc);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    }

    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
