/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 32
#endif

void aws_byte_cursor_from_c_str_harness() {
    /* Non-deterministically choose between NULL and non-NULL input */
    int use_null;
    const char *c_str;

    if (use_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded null-terminated string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);
        char *buf = malloc(len + 1);
        __CPROVER_assume(buf != NULL);
        buf[len] = '\0';
        c_str = buf;
    }

    /* Call the function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* Postcondition: result must be a valid cursor */
    assert(aws_byte_cursor_is_valid(&cur));

    if (c_str == NULL) {
        /* When input is NULL, expect empty cursor */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* When input is non-NULL, ptr should point to c_str and len should be strlen */
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
    }
}
