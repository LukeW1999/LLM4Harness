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

    /* 2. Save a byte from the source for immutability check */
    struct store_byte_from_buffer src_byte;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_byte);
    }

    /* Save original src state */
    struct aws_byte_cursor old_src = src;

    /* 3. Declare dest buffer (uninitialized, function will initialize it) */
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
            /* Verify the copy matches the source */
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
        /* dest was zeroed at start, then buffer allocation failed */
        assert(dest.len == 0 || dest.len == src.len);
        /* We can't assert much more about dest on failure since it's partially initialized */
    }

    /* 7. Source cursor must not have been modified */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    assert(aws_byte_cursor_is_valid(&src));

    /* 8. Verify source bytes unchanged */
    if (src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_byte);
    }
}
