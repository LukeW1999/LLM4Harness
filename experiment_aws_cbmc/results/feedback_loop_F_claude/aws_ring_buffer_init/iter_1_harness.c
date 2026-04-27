/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_ring_buffer_init_harness() {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator;
    size_t size;

    /* 2. Set up preconditions */
    /* allocator must be valid (non-null) */
    __CPROVER_assume(allocator != NULL);
    /* Bound size to keep state space manageable */
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success:
         * - allocation is non-null
         * - allocator is set to the provided allocator
         * - head and tail atomics point to allocation
         * - allocation_end == allocation + size
         * - ring buffer is valid
         */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        /* head and tail should point to allocation (ring buffer is empty) */
        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        /* Ring buffer should be empty after init */
        assert(aws_ring_buffer_is_empty(&ring_buf));

        /* Ring buffer validity invariant */
        assert(aws_ring_buffer_is_valid(&ring_buf));
    } else {
        /* On failure:
         * - result must be AWS_OP_ERR
         * - allocation should be NULL (zeroed struct, allocation failed)
         */
        assert(result == AWS_OP_ERR);
        /* After AWS_ZERO_STRUCT and failed allocation, allocation is NULL */
        assert(ring_buf.allocation == NULL);
    }
}
