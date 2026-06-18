#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf dest;

    struct store_byte_from_buffer src_store;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_store);
    }

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    assert(aws_byte_cursor_is_valid(&src));

    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_store);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len == 0) {
            assert(dest.buffer == NULL);
        } else {
            assert(dest.buffer != NULL);
            assert(memcmp(dest.buffer, src.ptr, src.len) == 0);
        }
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(aws_byte_buf_is_valid(&dest));
    }
}
