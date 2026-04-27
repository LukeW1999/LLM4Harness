/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_MALLOC
#define MAX_MALLOC (SIZE_MAX >> 1)
#endif

void aws_ring_buffer_init_harness() {
    /* Non-deterministic parameters. */
    struct aws_ring_buffer *ring_buf = malloc(sizeof(*ring_buf));
    struct aws_allocator *allocator = aws_default_allocator();
    size_t size;

    /* Preconditions. */
    __CPROVER_assume(ring_buf != NULL);
    __CPROVER_assume(allocator != NULL);
    __CPROVER_assume(size > 0);
    __CPROVER_assume(size <= MAX_MALLOC);

    /* Initialize ring_buf to zero to avoid undefined behavior */
    memset(ring_buf, 0, sizeof(*ring_buf));

    /* Operation under verification. */
    int result = aws_ring_buffer_init(ring_buf, allocator, size);

    /* Post-conditions: result must be either success or error */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* Postconditions on success. */
        assert(ring_buf->allocator == allocator);
        assert(ring_buf->allocation != NULL);
        assert(ring_buf->allocation_end != NULL);
        assert(ring_buf->allocation_end == ring_buf->allocation + size);
        /* head and tail should be valid pointers within the allocation */
        assert(aws_atomic_load_ptr(&ring_buf->head) != NULL);
        assert(aws_atomic_load_ptr(&ring_buf->tail) != NULL);
    }
    /* Both branches are reachable since allocation can fail */
}
