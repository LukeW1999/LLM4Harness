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

    /* 2. Save old cursor state for immutability checks */
    struct aws_byte_cursor old_src = src;

    /* 3. Set up dest buffer (uninitialized, will be written by function) */
    struct aws_byte_buf dest;

    /* 4. Use default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 5. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 6. Assert postconditions */
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
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            assert(dest.buffer == NULL);
        }

        assert(aws_byte_buf_is_valid(&dest));
    } else {
        /* On failure:
         * - dest should have been zeroed (AWS_ZERO_STRUCT was called before failure)
         * - The function zeros dest at the start, then fails on allocation
         * - So dest.len == 0, dest.capacity == 0, dest.buffer == NULL
         */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
    }

    /* 7. Assert src cursor is unchanged (function should not modify src) */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    assert(aws_byte_cursor_is_valid(&src));
}
