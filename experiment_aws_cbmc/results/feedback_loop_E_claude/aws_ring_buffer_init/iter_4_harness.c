/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
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
    if (ring_buf == NULL) return;
    if (allocator == NULL) return;
    if (size == 0) return;
    if (size > MAX_MALLOC) return;

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
        void *head = aws_atomic_load_ptr(&ring_buf->head);
        void *tail = aws_atomic_load_ptr(&ring_buf->tail);
        assert(head != NULL);
        assert(tail != NULL);
        assert(head >= (void *)ring_buf->allocation);
        assert(head <= (void *)ring_buf->allocation_end);
        assert(tail >= (void *)ring_buf->allocation);
        assert(tail <= (void *)ring_buf->allocation_end);
    }
}
