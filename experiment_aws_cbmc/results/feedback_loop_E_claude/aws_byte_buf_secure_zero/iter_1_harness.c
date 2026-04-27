/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness() {
    /* data structure */
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Precondition: buf must be valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state before calling */
    struct aws_byte_buf old = buf;

    /* Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /*
     * Postconditions from Doxygen:
     * "Sets all bytes of buffer to zero and resets len to zero."
     *
     * 1. Changed fields:
     *    - buf.len is set to 0
     *    - buf.buffer contents are zeroed (if buffer is non-null)
     *
     * 2. Unchanged fields:
     *    - buf.capacity remains the same
     *    - buf.allocator remains the same
     *    - buf.buffer pointer remains the same
     *
     * 3. Validity invariant must hold after the call
     */

    /* Assert len is reset to zero */
    assert(buf.len == 0);

    /* Assert capacity is unchanged */
    assert(buf.capacity == old.capacity);

    /* Assert allocator is unchanged */
    assert(buf.allocator == old.allocator);

    /* Assert buffer pointer is unchanged */
    assert(buf.buffer == old.buffer);

    /* Assert validity invariant holds */
    assert(aws_byte_buf_is_valid(&buf));
}
