/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_copy_harness() {
    /* Set up src */
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    /* Save a byte from src buffer for immutability check */
    struct store_byte_from_buffer src_byte;
    if (src.buffer && src.len > 0) {
        save_byte_from_array(src.buffer, src.len, &src_byte);
    }

    /* Save old src state to verify it is not modified */
    struct aws_byte_buf old_src = src;

    /* dest is uninitialized (output parameter) */
    struct aws_byte_buf dest;

    /* allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Call function under test */
    int result = aws_byte_buf_init_copy(&dest, allocator, &src);

    if (result == AWS_OP_SUCCESS) {
        /* dest must be valid */
        assert(aws_byte_buf_is_valid(&dest));

        /* allocator is set to the passed allocator */
        assert(dest.allocator == allocator);

        if (src.buffer == NULL) {
            /* If src->buffer is null, dest is zeroed with allocator set */
            assert(dest.buffer == NULL);
            assert(dest.len == 0);
            assert(dest.capacity == 0);
        } else {
            /* dest capacity and len match src */
            assert(dest.capacity == src.capacity);
            assert(dest.len == src.len);

            /* dest has a new backing buffer (not the same pointer as src) */
            assert(dest.buffer != NULL);

            /* Contents of dest->buffer match src->buffer for src->len bytes */
            if (src.len > 0) {
                assert_bytes_match(dest.buffer, src.buffer, src.len);
            }
        }
    } else {
        /* On failure, dest is zeroed */
        assert(dest.buffer == NULL);
        assert(dest.len == 0);
        assert(dest.capacity == 0);
        assert(dest.allocator == NULL);
    }

    /* src must not be modified */
    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);

    /* src remains valid */
    assert(aws_byte_buf_is_valid(&src));

    /* Verify a byte from src buffer was not modified */
    if (old_src.buffer && old_src.len > 0) {
        assert_byte_from_buffer_matches(src.buffer, &src_byte);
    }
}
