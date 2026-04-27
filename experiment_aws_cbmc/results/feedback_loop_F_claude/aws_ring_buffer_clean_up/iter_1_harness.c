/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_clean_up_harness() {
    /* 1. Declare and set up the ring buffer */
    struct aws_ring_buffer ring_buf;

    /* Non-deterministically initialize the ring buffer fields */
    ring_buf.allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(sizeof(struct aws_allocator));
    __CPROVER_assume(ring_buf.allocator != NULL);

    /* allocation can be NULL or a valid pointer */
    size_t alloc_size;
    __CPROVER_assume(alloc_size > 0 && alloc_size <= MAX_BUFFER_SIZE);

    if (nondet_bool()) {
        ring_buf.allocation = (uint8_t *)malloc(alloc_size);
        __CPROVER_assume(ring_buf.allocation != NULL);
        ring_buf.allocation_end = ring_buf.allocation + alloc_size;
    } else {
        ring_buf.allocation = NULL;
        ring_buf.allocation_end = NULL;
    }

    /* Set up atomic head and tail within valid bounds */
    size_t head_val = nondet_size_t();
    size_t tail_val = nondet_size_t();
    aws_atomic_init_int(&ring_buf.head, head_val);
    aws_atomic_init_int(&ring_buf.tail, tail_val);

    /* Assume the ring buffer is valid before calling clean_up */
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* 2. Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* 3. Assert postconditions:
     * AWS_ZERO_STRUCT zeroes out the entire structure.
     * After clean_up, all fields should be zero/NULL.
     */

    /* allocation should be NULL after clean_up */
    assert(ring_buf.allocation == NULL);

    /* allocation_end should be NULL after clean_up */
    assert(ring_buf.allocation_end == NULL);

    /* allocator should be NULL after clean_up */
    assert(ring_buf.allocator == NULL);

    /* head and tail atomics should be zero after AWS_ZERO_STRUCT */
    size_t head_after = aws_atomic_load_int(&ring_buf.head);
    size_t tail_after = aws_atomic_load_int(&ring_buf.tail);
    assert(head_after == 0);
    assert(tail_after == 0);
}
