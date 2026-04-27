/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_ring_buffer_release_harness() {
    /* Declare and set up the ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Use a fixed small size to keep state space manageable */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= 8);
    uint8_t *allocation = malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);

    ring_buffer.allocator = allocator;
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + alloc_size;

    /* Set up head and tail atomics to valid positions within the allocation */
    size_t head_offset;
    size_t tail_offset;
    __CPROVER_assume(head_offset <= alloc_size);
    __CPROVER_assume(tail_offset <= alloc_size);

    uint8_t *head_ptr = allocation + head_offset;
    uint8_t *tail_ptr = allocation + tail_offset;

    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    /* Ensure the ring buffer is valid before we start */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up the byte buf to release */
    /* buf.buffer must equal current tail (release starts from tail) */
    size_t buf_capacity;
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(tail_offset + buf_capacity <= alloc_size);

    struct aws_byte_buf buf;
    buf.buffer = tail_ptr;
    buf.capacity = buf_capacity;
    buf.len = 0;
    buf.allocator = NULL;

    /* The new tail will be buf.buffer + buf.capacity */
    uint8_t *new_tail = buf.buffer + buf.capacity;

    /* Ensure that after updating tail to new_tail, the ring buffer remains valid */
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)new_tail);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));
    /* Restore original tail */
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    /* Save state before call */
    uint8_t *saved_head = head_ptr;
    uint8_t *saved_allocation = allocation;
    uint8_t *saved_allocation_end = allocation + alloc_size;

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions */
    /* Check that tail was updated correctly */
    uint8_t *actual_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(actual_tail == new_tail);

    /* Check that buf is zeroed out */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Check that ring buffer fields other than tail are unchanged */
    assert(ring_buffer.allocator == allocator);
    assert(ring_buffer.allocation == saved_allocation);
    assert(ring_buffer.allocation_end == saved_allocation_end);

    /* Head should be unchanged */
    uint8_t *actual_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    assert(actual_head == saved_head);

    /* Ring buffer validity */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
