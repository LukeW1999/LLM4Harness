#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Allocator must not be NULL; use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Set up a valid source byte cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save the original cursor state (by value, it won't change) and
     * a byte from the source buffer to verify that it is not modified */
    struct aws_byte_cursor old_src = src;
    struct store_byte_from_buffer storage;
    if (src.len > 0) {
        save_byte_from_array(src.ptr, src.len, &storage);
    }

    /* Prepare the destination buffer (uninitialised, will be set by the call) */
    struct aws_byte_buf dest;

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    /* Postcondition checks */
    if (result == AWS_OP_SUCCESS) {
        /* Success path: dest fields must reflect the src cursor */
        assert(dest.len == src.len);
        assert(dest.capacity == src.len);
        assert(dest.allocator == allocator);

        if (src.len > 0) {
            /* Buffer must be allocated and contain a copy of the source data */
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* Zero-length src results in NULL buffer */
            assert(dest.buffer == NULL);
        }
    } else {
        /* Failure path (out of memory): dest remains zeroed as set by AWS_ZERO_STRUCT */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == NULL);
    }

    /* Dest must always be valid after the call */
    assert(aws_byte_buf_is_valid(&dest));

    /* Source cursor must not have been modified (immutability) */
    assert(src.len == old_src.len);
    assert(src.ptr == old_src.ptr);
    if (old_src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &storage);
    }

    /* Source cursor must remain valid */
    assert(aws_byte_cursor_is_valid(&src));
}
