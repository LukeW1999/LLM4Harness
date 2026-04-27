/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_buf_belongs_to_pool_harness() {
    /* 1. Set up the ring buffer with a concrete allocation */
    struct aws_ring_buffer ring_buffer;

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

    /* 2. Set up the byte buffer with a nondet buffer pointer */
    struct aws_byte_buf buf;
    buf.allocator = NULL;
    buf.len = 0;
    buf.capacity = 0;

    /* Make buf.buffer nondet - could be NULL, inside, or outside the ring buffer */
    uint8_t *buf_ptr;
    buf.buffer = buf_ptr;

    /* 3. Save old state */
    uint8_t *old_buf_buffer = buf.buffer;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;
    struct aws_allocator *old_allocator = ring_buffer.allocator;

    /* 4. Call function under test */
    bool result = aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* The ring buffer must not be modified */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    /* The buf must not be modified */
    assert(buf.buffer == old_buf_buffer);

    /* Check correctness of return value */
    if (buf.buffer != NULL &&
        buf.buffer >= ring_buffer.allocation &&
        buf.buffer < ring_buffer.allocation_end) {
        /* buf is within range, result must be true */
        assert(result == true);
    }

    if (result) {
        /* If result is true, buf must be within range */
        assert(buf.buffer != NULL);
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    } else {
        /* If result is false, buf must be outside range */
        assert(buf.buffer == NULL ||
               buf.buffer < ring_buffer.allocation ||
               buf.buffer >= ring_buffer.allocation_end);
    }
}
