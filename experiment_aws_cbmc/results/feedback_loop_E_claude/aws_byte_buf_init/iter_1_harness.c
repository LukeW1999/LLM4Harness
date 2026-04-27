/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_init_harness() {
    /* data structure */
    struct aws_byte_buf buf; /* Precondition: buf is non-null */

    /* parameters */
    struct aws_allocator *allocator = aws_default_allocator(); /* Precondition: allocator is non-null */
    size_t capacity;

    int result = aws_byte_buf_init(&buf, allocator, capacity);

    if (result == AWS_OP_SUCCESS) {
        /* On success: buf is valid, len == 0, capacity matches, allocator matches */
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.len == 0);
        assert(buf.capacity == capacity);
        assert(buf.allocator == allocator);
        /* If capacity > 0, buffer must be non-NULL */
        if (capacity > 0) {
            assert(buf.buffer != NULL);
        } else {
            assert(buf.buffer == NULL);
        }
    } else {
        /* On failure: buf is zeroed out (AWS_ZERO_STRUCT) */
        assert(buf.len == 0);
        assert(buf.capacity == 0);
        assert(buf.buffer == NULL);
        assert(buf.allocator == NULL);
        /* Validity: capacity == 0 and buffer == NULL is valid */
        assert(aws_byte_buf_is_valid(&buf));
    }
}
