/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_empty_array_harness() {
    /* parameters */
    size_t capacity;
    uint8_t *bytes = NULL;

    /* If capacity > 0, we need a valid writable buffer */
    if (capacity > 0) {
        bytes = malloc(capacity);
        __CPROVER_assume(bytes != NULL);
    }

    /* Precondition: bytes must be writable up to capacity bytes */
    __CPROVER_assume(capacity == 0 || AWS_MEM_IS_WRITABLE(bytes, capacity));

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_empty_array(bytes, capacity);

    /* Postconditions from Doxygen and implementation:
     * - buf.buffer is bytes if capacity > 0, else NULL
     * - buf.len is 0
     * - buf.capacity is capacity
     * - buf.allocator is NULL
     */

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* Changed fields */
    assert(buf.len == 0);
    assert(buf.capacity == capacity);
    assert(buf.allocator == NULL);

    /* buffer pointer behavior */
    if (capacity > 0) {
        assert(buf.buffer == bytes);
    } else {
        assert(buf.buffer == NULL);
    }
}
