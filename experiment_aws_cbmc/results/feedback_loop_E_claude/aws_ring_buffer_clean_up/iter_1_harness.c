/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* Allocate and initialize a valid ring buffer. */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Preconditions. */
    __CPROVER_assume(ring_buf != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0 && size < MAX_MALLOC);

    /* Initialize the ring buffer so it is valid before clean_up. */
    if (aws_ring_buffer_init(ring_buf, allocator, size) != AWS_OP_SUCCESS) {
        return;
    }

    /* Precondition: ring buffer must be valid before clean_up. */
    __CPROVER_assume(aws_ring_buffer_is_valid(ring_buf));

    /* Operation under verification. */
    aws_ring_buffer_clean_up(ring_buf);

    /*
     * Postconditions:
     * AWS_ZERO_STRUCT zeroes out the entire structure, so:
     * - allocator becomes NULL
     * - allocation becomes NULL
     * - allocation_end becomes NULL
     * - head and tail atomics are zeroed
     */
    assert(ring_buf->allocator == NULL);
    assert(ring_buf->allocation == NULL);
    assert(ring_buf->allocation_end == NULL);

    /* After zeroing, the ring buffer is no longer valid in the traditional sense,
     * but we can verify the zero state is consistent. */
    /* The head and tail should be zeroed (NULL pointers stored as atomics). */
    uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->head);
    uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf->tail);
    assert(head_ptr == NULL);
    assert(tail_ptr == NULL);
}
