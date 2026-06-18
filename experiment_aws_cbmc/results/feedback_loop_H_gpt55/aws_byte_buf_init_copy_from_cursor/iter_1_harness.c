#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer old_src_byte;
    save_byte_from_array(src.ptr, src.len, &old_src_byte);

    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == old_src.len);
        assert(dest.capacity == old_src.len);
        assert(dest.allocator == allocator);

        if (old_src.len == 0) {
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            assert(dest.buffer != NULL);
            assert(dest.buffer != old_src.ptr);
            assert(AWS_MEM_IS_READABLE(dest.buffer, dest.len));
            assert(AWS_MEM_IS_WRITABLE(dest.buffer, dest.capacity));
            assert_bytes_match(dest.buffer, old_src.ptr, old_src.len);
        }
    } else {
        assert(result == AWS_OP_ERR);
        assert(old_src.len != 0);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    assert_byte_from_buffer_matches(src.ptr, &old_src_byte);

    assert(aws_byte_cursor_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));

    aws_byte_buf_clean_up(&dest);
}
