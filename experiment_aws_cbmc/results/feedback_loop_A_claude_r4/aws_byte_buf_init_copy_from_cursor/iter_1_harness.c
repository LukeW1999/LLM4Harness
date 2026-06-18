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

    /* Save a byte from src for later comparison */
    struct store_byte_from_buffer src_byte;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &src_byte);
    }

    /* 2. Set up dest (uninitialized output buffer) */
    struct aws_byte_buf dest;

    /* 3. Get allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 4. Call function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - dest is a valid aws_byte_buf
         * - dest->len == src.len
         * - dest->capacity == src.len
         * - dest->allocator == allocator
         * - dest->buffer contains a copy of src.ptr (if src.len > 0)
         */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0) {
            assert(dest.buffer != NULL);
            /* Verify the copy: check that the saved byte matches */
            assert_byte_from_buffer_matches(dest.buffer, &src_byte);
        } else {
            /* If src.len == 0, buffer should be NULL */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure:
         * - dest should have been zeroed (AWS_ZERO_STRUCT was called)
         * - dest->buffer == NULL
         * - dest->len == 0
         * - dest->capacity == 0
         * - dest->allocator == allocator (set before failure)
         *   Actually, looking at the implementation: AWS_ZERO_STRUCT zeroes everything,
         *   then buffer allocation fails, so allocator was set after the failed alloc check.
         *   Let's check: dest->allocator is set AFTER the NULL check returns AWS_OP_ERR.
         *   So on failure, dest is zeroed (allocator == NULL, len == 0, capacity == 0, buffer == NULL).
         */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        /* allocator field: AWS_ZERO_STRUCT sets it to NULL, and we return before setting it */
        assert(dest.allocator == NULL);
    }

    /* 6. Source cursor must remain valid and unchanged */
    assert(aws_byte_cursor_is_valid(&src));
}
