#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Set up the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save a copy of src for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* 2. Set up dest (uninitialized output buffer) */
    struct aws_byte_buf dest;

    /* 3. Get allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - dest->len == src.len
         * - dest->capacity == src.len
         * - dest->allocator == allocator
         * - dest->buffer is a copy of src.ptr (if src.len > 0)
         * - dest is valid
         */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0) {
            assert(dest.buffer != NULL);
            /* Verify the contents were copied */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On failure:
         * - dest should have been zeroed (AWS_ZERO_STRUCT was called)
         * - The function returns AWS_OP_ERR
         */
        assert(result == AWS_OP_ERR);
        /* After AWS_ZERO_STRUCT and failed allocation, dest fields are zeroed */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == allocator);
    }

    /* 6. Assert src cursor was not modified (it's passed by value, but ptr contents should be unchanged) */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);

    /* 7. Assert src cursor is still valid */
    assert(aws_byte_cursor_is_valid(&src));
}
