/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/ring_buffer.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_ring_buffer_clean_up_harness() {
    struct aws_ring_buffer ring_buf;

    /* Use a real allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Non-deterministic buffer size */
    size_t capacity;
    __CPROVER_assume(capacity > 0 && capacity <= MAX_BUFFER_SIZE);

    /* Initialize the ring buffer using the proper init function */
    int init_result = aws_ring_buffer_init(&ring_buf, allocator, capacity);

    /* Only proceed if initialization succeeded */
    __CPROVER_assume(init_result == AWS_OP_SUCCESS);
    __CPROVER_assume(aws_ring_buffer_is_valid(&ring_buf));

    /* Record that allocation was non-NULL before clean_up */
    uint8_t *allocation_before = ring_buf.allocation;
    assert(allocation_before != NULL);

    /* Call the function under test */
    aws_ring_buffer_clean_up(&ring_buf);

    /* Assert postconditions after clean_up:
     * AWS_ZERO_STRUCT zeroes out the entire structure.
     */
    assert(ring_buf.allocation == NULL);
    assert(ring_buf.allocation_end == NULL);
    assert(ring_buf.allocator == NULL);
}
