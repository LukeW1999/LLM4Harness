/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 64
#endif

void aws_ring_buffer_release_harness() {
    /* 1. Set up the ring buffer */
    struct aws_ring_buffer ring_buffer;

    /* Allocate the ring buffer's internal allocation non-deterministically */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);

    ring_buffer.allocator = (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    ring_buffer.allocation = (uint8_t *)malloc(alloc_size);
    __CPROVER_assume(ring_buffer.allocation != NULL);
    ring_buffer.allocation_end = ring_buffer.allocation + alloc_size;

    /* Set up head and tail atomics to valid positions within the allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);
    aws_atomic_init_ptr(&ring_buffer.head, (void *)(ring_buffer.allocation + head_offset));
    aws_atomic_init_ptr(&ring_buffer.tail, (void *)(ring_buffer.allocation + tail_offset));

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* 2. Set up the byte buf to release */
    struct aws_byte_buf buf;

    /* buf.buffer must point within the ring buffer's allocation */
    size_t buf_offset;
    size_t buf_capacity;
    __CPROVER_assume(buf_offset < alloc_size);
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_capacity <= alloc_size - buf_offset);

    buf.buffer = ring_buffer.allocation + buf_offset;
    buf.capacity = buf_capacity;
    buf.len = 0;
    buf.allocator = NULL;

    /* buf.buffer + buf.capacity must be within bounds */
    __CPROVER_assume((uintptr_t)(buf.buffer + buf.capacity) <= (uintptr_t)ring_buffer.allocation_end);

    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Ensure buf is a valid ring buffer acquisition */
    __CPROVER_assume(aws_ring_buffer_buf_belongs_to_pool(&ring_buffer, &buf));

    /* 3. Save old state */
    uint8_t *expected_new_tail = buf.buffer + buf.capacity;
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    struct aws_allocator *old_allocator = ring_buffer.allocator;
    uint8_t *old_allocation = ring_buffer.allocation;
    uint8_t *old_allocation_end = ring_buffer.allocation_end;

    /* 4. Call function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* 5. Assert postconditions */

    /* The tail should now point to buf->buffer + buf->capacity */
    uint8_t *new_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(new_tail == expected_new_tail);

    /* buf should be zeroed out after release */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* Ring buffer fields other than tail should be unchanged */
    assert(ring_buffer.allocator == old_allocator);
    assert(ring_buffer.allocation == old_allocation);
    assert(ring_buffer.allocation_end == old_allocation_end);

    /* Head should be unchanged */
    uint8_t *new_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(new_head == old_head);

    /* Ring buffer validity should still hold */
    assert(aws_ring_buffer_is_valid(&ring_buffer));

    /* buf validity after zeroing */
    assert(aws_byte_buf_is_valid(&buf));
}
