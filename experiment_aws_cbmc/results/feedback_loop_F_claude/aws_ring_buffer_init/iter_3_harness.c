/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif

void aws_ring_buffer_init_harness() {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    size_t size;

    /* Use a non-deterministic allocator to allow both success and failure paths */
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    /* 2. Set up preconditions */
    /* Bound size to keep state space manageable */
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Assert postconditions - these must be reachable */
    /* The result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success, verify the ring buffer is properly initialized */
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        /* On failure, result must be AWS_OP_ERR */
        assert(result == AWS_OP_ERR);
    }
}
