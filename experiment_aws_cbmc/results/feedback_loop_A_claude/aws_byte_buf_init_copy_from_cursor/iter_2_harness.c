#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Set up the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Set up the destination buffer (uninitialized, will be written by function) */
    struct aws_byte_buf dest;

    /* 3. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Save a byte from src for later comparison */
    struct store_byte_from_buffer src_byte;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_byte);
    }

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_byte_from_buffer_matches(dest.buffer, &src_byte);
        }

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        assert(result == AWS_OP_ERR);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 7. Source cursor must remain valid and unchanged */
    assert(aws_byte_cursor_is_valid(&src));
}
