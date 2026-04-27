/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_c_str_harness() {
    /* parameters */
    const char *c_str;

    /* assumption: c_str is either NULL or a valid null-terminated string */
    /* We use a bounded string to keep the state space manageable */
    size_t max_len;
    __CPROVER_assume(max_len <= MAX_BUFFER_SIZE);

    if (nondet_bool()) {
        /* NULL case */
        c_str = NULL;
    } else {
        /* Non-NULL: allocate a valid null-terminated string */
        c_str = malloc(max_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Ensure there's a null terminator somewhere within bounds */
        c_str[max_len] = '\0';
    }

    /* operation under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* assertions */
    assert(aws_byte_cursor_is_valid(&cur));

    if (c_str == NULL) {
        /* When c_str is NULL, ptr should be NULL and len should be 0 */
        assert(cur.ptr == NULL);
        assert(cur.len == 0);
    } else {
        /* When c_str is non-NULL, ptr should equal c_str and len should equal strlen(c_str) */
        assert(cur.ptr == (uint8_t *)c_str);
        assert(cur.len == strlen(c_str));
        /* The cursor's bytes should match the original string */
        assert_bytes_match(cur.ptr, (const uint8_t *)c_str, cur.len);
    }
}
