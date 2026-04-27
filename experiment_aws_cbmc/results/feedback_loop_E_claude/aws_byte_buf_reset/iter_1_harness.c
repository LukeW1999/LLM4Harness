/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_reset_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* Bound and initialize the buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state before calling */
    struct aws_byte_buf old = buf;

    /* Non-deterministic parameter */
    bool zero_contents = nondet_bool();

    /* Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* Postconditions:
     * 1. len is reset to 0
     * 2. capacity is unchanged
     * 3. buffer pointer is unchanged
     * 4. allocator is unchanged
     * 5. validity invariant holds
     */

    /* Changed fields */
    assert(buf.len == 0);

    /* Unchanged fields */
    assert(buf.capacity == old.capacity);
    assert(buf.buffer == old.buffer);
    assert(buf.allocator == old.allocator);

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
