/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Precondition: buf must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call function under test */
    aws_byte_buf_clean_up(&buf);

    /* Postconditions:
     * From the implementation:
     *   buf->allocator = NULL;
     *   buf->buffer = NULL;
     *   buf->len = 0;
     *   buf->capacity = 0;
     */
    assert(buf.allocator == NULL);
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);

    /* Validity invariant must hold after clean_up */
    assert(aws_byte_buf_is_valid(&buf));
}
