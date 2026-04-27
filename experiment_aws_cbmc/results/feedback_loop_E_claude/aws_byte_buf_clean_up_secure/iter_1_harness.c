/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_secure_harness() {
    /* Declare and set up the buffer */
    struct aws_byte_buf buf;

    /* Bound the buffer size for tractable verification */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /*
     * aws_byte_buf_clean_up_secure:
     * 1. Calls aws_byte_buf_secure_zero: zeros all bytes and sets len to 0
     * 2. Calls aws_byte_buf_clean_up: frees the buffer and resets all fields
     *
     * After clean_up, the buffer fields are reset:
     * - buf.len == 0
     * - buf.buffer == NULL
     * - buf.capacity == 0
     * - buf.allocator == NULL
     */

    /* Assert postconditions after clean_up_secure */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The resulting buffer should be valid (a zeroed/null buffer is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
