#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Nondet source cursor, valid and bounded */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* The destination buffer is always valid after the call */
    assert(aws_byte_buf_is_valid(&dest));

    if (result == AWS_OP_SUCCESS) {
        /* On success: len and capacity match src.len */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        /* Allocator is the one passed in */
        assert(dest.allocator == allocator);
        /* Buffer pointer and contents */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure: dest is left zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Source cursor remains unchanged and valid */
    assert(aws_byte_cursor_is_valid(&src));
}
