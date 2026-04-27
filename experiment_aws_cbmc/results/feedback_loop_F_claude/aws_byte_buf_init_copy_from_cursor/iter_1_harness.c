#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Harness for aws_byte_buf_init_copy_from_cursor
 *
 * Preconditions:
 *   - src is a valid aws_byte_cursor
 *   - allocator is non-null (needed for allocation)
 *
 * Postconditions on success:
 *   - dest is a valid aws_byte_buf
 *   - dest->len == src.len
 *   - dest->capacity == src.len
 *   - dest->allocator == allocator
 *   - dest->buffer contains a copy of src.ptr contents (if src.len > 0)
 *
 * Postconditions on failure:
 *   - dest is zeroed (AWS_ZERO_STRUCT was called before the failure point)
 */
void aws_byte_buf_init_copy_from_cursor_harness(void) {
    /* 1. Set up the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* 2. Set up allocator (non-deterministic but non-null) */
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    /* 3. Set up destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* 4. Save a byte from src for later comparison */
    struct store_byte_from_buffer saved_src_byte;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &saved_src_byte);
    }

    /* 5. Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* 6. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* dest must be a valid aws_byte_buf */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->len == src.len */
        assert(dest.len == src.len);

        /* dest->capacity == src.len */
        assert(dest.capacity == src.len);

        /* dest->allocator == allocator */
        assert(dest.allocator == allocator);

        /* If src.len > 0, dest->buffer is non-null and contains a copy */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            /* Verify the copy matches the source */
            assert_bytes_match(dest.buffer, src.ptr, src.len);
            /* Verify a specific byte from the original src is preserved */
            assert_byte_from_buffer_matches(dest.buffer, &saved_src_byte);
        } else {
            /* If src.len == 0, buffer should be NULL */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure, dest should be zeroed (AWS_ZERO_STRUCT was called) */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);

        /* src.len must have been > 0 for allocation to fail */
        assert(src.len > 0);
    }

    /* 7. The source cursor must remain unchanged (function should not modify src) */
    assert(aws_byte_cursor_is_valid(&src));
}
