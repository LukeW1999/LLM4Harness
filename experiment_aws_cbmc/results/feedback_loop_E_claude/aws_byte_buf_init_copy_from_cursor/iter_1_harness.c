/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_from_cursor_harness() {
    /* Set up the source cursor */
    struct aws_byte_cursor src;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_cursor_is_valid(&src));

    /* Save a byte from the source for immutability check */
    struct store_byte_from_buffer src_byte;
    save_byte_from_array(src.ptr, src.len, &src_byte);

    /* Save original src state */
    struct aws_byte_cursor old_src = src;

    /* Destination buffer (uninitialized) */
    struct aws_byte_buf dest;

    /* Use the default allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Call the function under test */
    int result = aws_byte_buf_init_copy_from_cursor(&dest, allocator, src);

    if (result == AWS_OP_SUCCESS) {
        /* 1. Changed fields: dest is now a valid buffer */
        assert(aws_byte_buf_is_valid(&dest));

        /* dest->len == src.len */
        assert(dest.len == src.len);

        /* dest->capacity == src.len */
        assert(dest.capacity == src.len);

        /* dest->allocator == allocator */
        assert(dest.allocator == allocator);

        /* If src.len > 0, dest->buffer is non-null and contents match */
        if (src.len > 0) {
            assert(dest.buffer != NULL);
            assert_bytes_match(dest.buffer, src.ptr, src.len);
        } else {
            /* If src.len == 0, buffer should be NULL */
            assert(dest.buffer == NULL);
        }
    } else {
        /* On failure, dest should have been zeroed (AWS_ZERO_STRUCT was called) */
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.buffer == NULL);
        assert(dest.allocator == allocator);
    }

    /* Source cursor must not have been modified */
    assert(src.ptr == old_src.ptr);
    assert(src.len == old_src.len);

    /* Source data must not have been modified */
    if (old_src.len > 0) {
        assert_byte_from_buffer_matches(src.ptr, &src_byte);
    }

    /* Source cursor remains valid */
    assert(aws_byte_cursor_is_valid(&src));
}
