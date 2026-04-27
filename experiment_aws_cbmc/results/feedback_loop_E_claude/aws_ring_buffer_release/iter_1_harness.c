/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_release_harness() {
    /* Declare and set up the ring buffer */
    struct aws_ring_buffer ring_buffer;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Allocate a backing allocation for the ring buffer */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size < MAX_MALLOC);
    uint8_t *allocation = malloc(alloc_size);
    __CPROVER_assume(allocation != NULL);

    ring_buffer.allocator = allocator;
    ring_buffer.allocation = allocation;
    ring_buffer.allocation_end = allocation + alloc_size;

    /* Set up head and tail atomics to valid positions within the allocation */
    uint8_t *head_ptr;
    uint8_t *tail_ptr;
    __CPROVER_assume(head_ptr >= allocation && head_ptr <= allocation + alloc_size);
    __CPROVER_assume(tail_ptr >= allocation && tail_ptr <= allocation + alloc_size);
    aws_atomic_store_ptr(&ring_buffer.head, (void *)head_ptr);
    aws_atomic_store_ptr(&ring_buffer.tail, (void *)tail_ptr);

    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buffer));

    /* Set up the byte buf to release */
    struct aws_byte_buf buf;
    /* buf.buffer must point within the ring buffer's allocation */
    size_t buf_offset;
    size_t buf_capacity;
    __CPROVER_assume(buf_offset < alloc_size);
    __CPROVER_assume(buf_capacity > 0);
    __CPROVER_assume(buf_offset + buf_capacity <= alloc_size);

    buf.buffer = allocation + buf_offset;
    buf.capacity = buf_capacity;
    buf.len = nondet_size_t();
    __CPROVER_assume(buf.len <= buf.capacity);
    buf.allocator = nondet_bool() ? allocator : NULL;

    /* The new tail will be buf.buffer + buf.capacity, which must be within bounds */
    uint8_t *new_tail = buf.buffer + buf.capacity;
    __CPROVER_assume(new_tail >= allocation && new_tail <= allocation + alloc_size);

    /* Save old ring buffer state */
    struct aws_ring_buffer old_ring_buffer = ring_buffer;

    /* Call the function under test */
    aws_ring_buffer_release(&ring_buffer, &buf);

    /* Postconditions:
     * 1. The tail of the ring buffer is updated to buf->buffer + buf->capacity
     * 2. buf is zeroed out (AWS_ZERO_STRUCT(*buf))
     * 3. Ring buffer allocator, allocation, allocation_end are unchanged
     */

    /* Check that tail was updated correctly */
    uint8_t *actual_tail = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.tail);
    assert(actual_tail == new_tail);

    /* Check that buf is zeroed */
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.allocator == NULL);

    /* Check that ring buffer fields other than tail are unchanged */
    assert(ring_buffer.allocator == old_ring_buffer.allocator);
    assert(ring_buffer.allocation == old_ring_buffer.allocation);
    assert(ring_buffer.allocation_end == old_ring_buffer.allocation_end);

    /* Head should be unchanged */
    uint8_t *actual_head = (uint8_t *)aws_atomic_load_ptr(&ring_buffer.head);
    uint8_t *old_head = (uint8_t *)aws_atomic_load_ptr(&old_ring_buffer.head);
    assert(actual_head == old_head);

    /* Ring buffer validity */
    assert(aws_ring_buffer_is_valid(&ring_buffer));
}
