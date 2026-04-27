/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_array_harness() {
    /* parameters */
    size_t len;
    uint8_t *bytes;

    /* Non-deterministic length, bounded to keep state space manageable */
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate bytes array: if len > 0, we need a valid pointer */
    if (len > 0) {
        bytes = malloc(len);
        __CPROVER_assume(bytes != NULL);
    } else {
        /* NULL may be passed as the array pointer if its length is declared to be 0 */
        bytes = nondet_bool() ? NULL : malloc(0);
    }

    /* operation under verification */
    struct aws_byte_cursor result = aws_byte_cursor_from_array(bytes, len);

    /* postconditions */
    /* 1. Changed fields: ptr and len are set from the arguments */
    assert(result.ptr == (uint8_t *)bytes);
    assert(result.len == len);

    /* 2. Validity invariant */
    assert(aws_byte_cursor_is_valid(&result));
}
