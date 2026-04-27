/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_advance_harness() {
    /* Set up the input buffer */
    struct aws_byte_buf buffer;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buffer, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buffer);
    __CPROVER_assume(aws_byte_buf_is_valid(&buffer));

    /* Set up the output buffer - must be valid before passing to function */
    struct aws_byte_buf output;
    ensure_byte_buf_has_allocated_buffer_member(&output);
    __CPROVER_assume(aws_byte_buf_is_valid(&output));

    /* Non-deterministic length to advance */
    size_t len;

    /* Save old state */
    struct aws_byte_buf old_buffer = buffer;

    /* Call the function under test */
    bool result = aws_byte_buf_advance(&buffer, &output, len);

    if (result) {
        /* Success path:
         * - buffer->len increases by len
         * - output is assigned a sub-buffer of len bytes starting at old buffer->len
         * - output->len is set to 0
         * - output->capacity is len
         */
        assert(buffer.len == old_buffer.len + len);
        assert(output.len == 0);
        assert(output.capacity == len);
        /* output->buffer points into buffer->buffer at the old len offset */
        assert(output.buffer == old_buffer.buffer + old_buffer.len);

        /* Unchanged fields of buffer */
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
        assert(aws_byte_buf_is_valid(&output));
    } else {
        /* Failure path:
         * - buffer is unchanged
         * - output is zeroed (AWS_ZERO_STRUCT)
         */
        assert(buffer.len == old_buffer.len);
        assert(buffer.capacity == old_buffer.capacity);
        assert(buffer.allocator == old_buffer.allocator);
        assert(buffer.buffer == old_buffer.buffer);

        /* output is zeroed */
        assert(output.len == 0);
        assert(output.capacity == 0);
        assert(output.buffer == NULL);
        assert(output.allocator == NULL);

        /* Validity invariants */
        assert(aws_byte_buf_is_valid(&buffer));
    }
}
