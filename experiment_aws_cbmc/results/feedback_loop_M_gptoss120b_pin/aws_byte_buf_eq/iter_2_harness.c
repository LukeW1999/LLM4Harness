#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Declare inputs */
    struct aws_byte_buf dest = {0};
    struct aws_allocator *alloc = aws_default_allocator();

    struct aws_byte_cursor src;

    /* Bound the source cursor */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save old state of src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* 2. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, alloc, src);

    /* 3. Postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest fields must reflect src */
        assert(dest.allocator == alloc);
        assert(dest.capacity == src.len);
        assert(dest.len == src.len);
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }
        /* src must be unchanged */
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    } else {
        /* on failure dest should be zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
        /* src must be unchanged */
        assert(src.ptr == old_src.ptr);
        assert(src.len == old_src.len);
    }

    /* 4. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_cursor_is_valid(&src));
}
