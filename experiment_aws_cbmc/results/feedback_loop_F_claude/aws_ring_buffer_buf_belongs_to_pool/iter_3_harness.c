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

    /* Set up allocator using aws_default_allocator */
    ring_buffer.allocator = aws_default_allocator();

    /* Ensure ring_buffer is valid */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buffer with a nondet buffer pointer */
    struct aws_byte_buf buf;
    
    /* Use nondet values for buf fields */
    uint8_t *buf_ptr;
    size_t buf_len;
    size_t buf_capacity;
    
    /* buf.buffer can be NULL or any pointer */
    buf.buffer = buf_ptr;
    buf.len = buf_len;
    buf.capacity = buf_capacity;
    buf.allocator = NULL;

    /* Ensure buf is valid per aws_byte_buf_is_valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

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

    /* Check correctness of return value:
     * The function returns true iff buf->buffer >= allocation && buf->buffer < allocation_end
     * Note: if buf->buffer is NULL, it cannot be >= allocation (assuming allocation != NULL)
     */
    if (result) {
        /* If result is true, buf must be within range */
        assert(buf.buffer >= ring_buffer.allocation);
        assert(buf.buffer < ring_buffer.allocation_end);
    }
}
