/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/allocator.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 16
#endif

void aws_ring_buffer_init_harness() {
    /* 1. Declare inputs */
    struct aws_ring_buffer ring_buf;
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* 2. Set up preconditions */
    /* Bound size to keep state space manageable */
    __CPROVER_assume(size > 0 && size <= MAX_BUFFER_SIZE);

    /* 3. Call function under test */
    int result = aws_ring_buffer_init(&ring_buf, allocator, size);

    /* 4. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        assert(ring_buf.allocation != NULL);
        assert(ring_buf.allocator == allocator);
        assert(ring_buf.allocation_end == ring_buf.allocation + size);

        uint8_t *head_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.head);
        uint8_t *tail_ptr = (uint8_t *)aws_atomic_load_ptr(&ring_buf.tail);
        assert(head_ptr == ring_buf.allocation);
        assert(tail_ptr == ring_buf.allocation);

        assert(aws_ring_buffer_is_valid(&ring_buf));

        aws_ring_buffer_clean_up(&ring_buf);
    } else {
        assert(result == AWS_OP_ERR);
    }
}
