/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_c_str_harness() {
    /* parameters */
    /* c_str may be NULL or a valid C string */
    const char *c_str;

    /* Non-deterministically choose whether c_str is NULL or a bounded C string */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        c_str = ensure_c_str_is_allocated(MAX_BUFFER_SIZE);
        __CPROVER_assume(c_str != NULL);
    }

    /* operation under verification */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_c_str(c_str);

    /* assertions */
    /* 1. Changed fields: ptr and len are set based on c_str */
    if (c_str == NULL) {
        /* If c_str is NULL, ptr is NULL and len is 0 */
        assert(cursor.ptr == NULL);
        assert(cursor.len == 0);
    } else {
        /* If c_str is non-NULL, ptr points to c_str and len is strlen(c_str) */
        assert(cursor.ptr == (uint8_t *)c_str);
        assert(cursor.len == strlen(c_str));
    }

    /* 2. Validity invariant: the returned cursor must be valid */
    assert(aws_byte_cursor_is_valid(&cursor));
}
