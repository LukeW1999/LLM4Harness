/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    /* Parameters */
    size_t len;
    uint8_t *bytes;

    /* Bound the size to keep verification tractable */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* If len > 0, bytes must be writable for len bytes (precondition from implementation) */
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        bytes = NULL;
    }

    /* Call function under test */
    struct aws_byte_buf buf = aws_byte_buf_from_array(bytes, len);

    /* Postconditions from Doxygen and implementation:
     * - buf.buffer is set to bytes if len > 0, else NULL
     * - buf.len == len
     * - buf.capacity == len
     * - buf.allocator == NULL
     * - result is a valid aws_byte_buf
     */

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));

    /* Changed fields */
    assert(buf.len == len);
    assert(buf.capacity == len);

    /* Buffer pointer behavior */
    if (len > 0) {
        assert(buf.buffer == (uint8_t *)bytes);
    } else {
        assert(buf.buffer == NULL);
    }

    /* Allocator is always NULL for this function */
    assert(buf.allocator == NULL);
}
