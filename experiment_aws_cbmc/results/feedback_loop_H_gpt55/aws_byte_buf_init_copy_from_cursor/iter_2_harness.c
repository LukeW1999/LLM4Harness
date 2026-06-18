#include <aws/common/allocator.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_cursor old_src = src;

    size_t saved_index;
    uint8_t saved_byte = 0;
    if (old_src.len > 0) {
        __CPROVER_assume(saved_index < old_src.len);
        saved_byte = old_src.ptr[saved_index];
    }

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (old_src.len > 0) {
        assert(src.ptr[saved_index] == saved_byte);
    }

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_src.len);
        assert(dest.capacity == old_src.len);

        if (old_src.len == 0) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.allocator == allocator);
            assert(dest.buffer != NULL);
            assert(dest.buffer != old_src.ptr);
            assert(AWS_MEM_IS_READABLE(dest.buffer, dest.len));
            assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
            assert(dest.buffer[saved_index] == saved_byte);
        }
    } else {
        assert(old_src.len != 0);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    assert(aws_byte_cursor_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));

    aws_byte_buf_clean_up(&dest);
}
