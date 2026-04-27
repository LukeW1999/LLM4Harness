#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* data structure */
    struct aws_byte_buf dest;
    struct aws_byte_cursor src;

    /* parameters */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Bound and allocate data structures */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state */
    struct aws_byte_cursor old_src = src;

    /* Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.allocator == allocator);
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        if (src.len > 0) {
            assert(AWS_MEM_IS_READABLE(dest.buffer, src.len));
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        }
    } else {
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == allocator);
    }

    /* Assert fields that must NOT change regardless of result */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    /* Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
