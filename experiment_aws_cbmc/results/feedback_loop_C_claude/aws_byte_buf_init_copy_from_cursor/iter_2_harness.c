#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* Setup inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = aws_default_allocator();
    struct aws_byte_cursor src;

    /* Make src a valid cursor with bounded size */
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));

    /* Precondition: src must be valid */
    assert(aws_byte_cursor_is_valid(&src));

    /* Save old src state (passed by value, but let's record for clarity) */
    struct aws_byte_cursor old_src = src;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* src is passed by value, so the original cursor is unchanged */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest is a valid byte_buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->len == src.len */
        assert(dest.len == src.len);

        /* dest->capacity == src.len */
        assert(dest.capacity == src.len);

        /* dest->allocator == allocator */
        assert(dest.allocator == allocator);

        /* If src.len > 0, buffer is non-NULL and contains copy of src data */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* If src.len == 0, buffer is NULL */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure: AWS_ZERO_STRUCT was called, so dest is zeroed */
        assert(result == AWS_OP_ERR);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        assert(dest.buffer == NULL);
    }
}
