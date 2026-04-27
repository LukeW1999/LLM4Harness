/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_acquire_harness() {
    /* Non-deterministic parameters. */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    size_t requested_size;
    struct aws_byte_buf dest;

    /* Preconditions. */
    __CPROVER_assume(ring_buf != NULL);
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buf));
    __CPROVER_assume(requested_size != 0);
    __CPROVER_assume(requested_size < MAX_MALLOC);

    /* Save old state. */
    struct aws_ring_buffer old_ring_buf = *ring_buf;

    /* Operation under verification. */
    int result = aws_ring_buffer_acquire(ring_buf, requested_size, &dest);

    /* Postconditions. */
    if (result == AWS_OP_SUCCESS) {
        /* On success, dest should be valid and have the requested size. */
        assert(aws_byte_buf_is_valid(&dest));
        assert(dest.len == 0);
        assert(dest.capacity == requested_size);
        assert(dest.buffer != NULL);

        /* The buffer should be within the ring buffer's allocation. */
        assert(dest.buffer >= ring_buf->allocation);
        assert(dest.buffer + dest.capacity <= ring_buf->allocation_end);

        /* Ring buffer structural fields that must not change. */
        assert(ring_buf->allocator == old_ring_buf.allocator);
        assert(ring_buf->allocation == old_ring_buf.allocation);
        assert(ring_buf->allocation_end == old_ring_buf.allocation_end);

        /* Ring buffer must remain valid. */
        assert(aws_ring_buffer_is_valid(ring_buf));
    } else {
        /* On failure, ring buffer fields must be unchanged. */
        assert(ring_buf->allocator == old_ring_buf.allocator);
        assert(ring_buf->allocation == old_ring_buf.allocation);
        assert(ring_buf->allocation_end == old_ring_buf.allocation_end);

        /* Ring buffer must remain valid. */
        assert(aws_ring_buffer_is_valid(ring_buf));
    }
}
