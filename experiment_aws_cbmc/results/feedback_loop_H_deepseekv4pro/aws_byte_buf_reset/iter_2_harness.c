#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Source cursor */
    struct aws_byte_cursor src;
    size_t src_len;
    __CPROVER_assume(src_len <= MAX_BUFFER_SIZE);
    src.len = src_len;
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Destination buffer (deliberately uninitialized) */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions: validity invariants hold for dest and src */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));

    /* Source cursor must remain unchanged (passed by value) */
    assert(src.len == src_len);

    if (result == AWS_OP_SUCCESS) {
        /* Success path */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* If src is empty, dest must have a null buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* Failure path – only possibility is allocation failure when src.len > 0 */
        assert(result == AWS_OP_ERR);
        assert(src.len > 0);

        /* Dest must be zeroed out */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        /* Allocator is NULL because AWS_ZERO_STRUCT was applied */
        assert(dest.allocator == NULL);
    }
}
