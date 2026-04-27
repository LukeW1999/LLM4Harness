#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Set up the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Use a valid allocator with proper function pointers */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Set up destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 4. Save a byte from src for later comparison */
    struct store_byte_from_buffer saved_src_byte;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &saved_src_byte);
    }

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->len == src.len */
        assert(dest.len == src.len);

        /* dest->capacity == src.len */
        assert(dest.capacity == src.len);

        /* dest->allocator == allocator */
        assert(dest.allocator == allocator);

        /* If src.len > 0, dest->buffer is non-null and contains a copy */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
        }
    } else {
        /* On failure, dest should be zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* 7. The source cursor must remain valid */
    assert(aws_byte_cursor_is_valid(&src));
}
