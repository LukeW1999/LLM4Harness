#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_buf dest;
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    struct aws_byte_cursor src;

    /* Assume allocator is not null */
    __CPROVER_assume(allocator != NULL);

    /* Bound and prepare src */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src */
    struct aws_byte_cursor old_src = src;

    /* Call the function */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
    } else {
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* Unchanged fields */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    /* Validity invariants */
    assert(aws_byte_cursor_is_valid(&src));
    assert(aws_byte_buf_is_valid(&dest));
}
