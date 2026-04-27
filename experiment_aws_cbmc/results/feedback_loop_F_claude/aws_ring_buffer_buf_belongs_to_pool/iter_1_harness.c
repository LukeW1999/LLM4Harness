/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* Allocate a backing allocation for the ring buffer */
    size_t ring_buf_size;
    __CPROVER_assume(ring_buf_size > 0 && ring_buf_size <= MAX_BUFFER_SIZE);

    uint8_t *allocation = malloc(ring_buf_size);
    __CPROVER_assume(allocation != NULL);

    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + ring_buf_size;

    /* Set up head and tail atomics to valid positions within the buffer */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= ring_buf_size);
    __CPROVER_assume(tail_offset <= ring_buf_size);

    aws_atomic_init_ptr(&ring_buffer.head, (void *)(allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(allocation + tail_offset));

    /* Set up allocator */
    struct aws_allocator *allocator = malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(allocator != NULL);
    ring_buffer.allocator = allocator;

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buffer */
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 4. Assert postconditions */

    /* The function returns true if buf->buffer is within the ring buffer's allocation range */
    if (result) {
        /* buf->buffer must be within [allocation, allocation_end) */
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    } else {
        /* Either buf->buffer is NULL or outside the ring buffer's allocation range */
        assert(buf.buffer == NULL ||
               buf.buffer < ring_buffer.allocation ||
               buf.buffer >= ring_buffer.allocation_end);
    }

    /* 5. Assert unchanged fields - ring_buffer must not be modified */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* buf must not be modified */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert validity invariants */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
    assert(aws_byte_buf_is_valid(&buf));
}
