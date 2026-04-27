/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_byte_buf_harness() {
    /* Non-deterministic choice of whether str and buf are NULL */
    const struct aws_string *str = nondet_bool() ? NULL : ensure_allocated_string(MAX_STRING_LEN);
    struct aws_byte_buf *buf = nondet_bool() ? NULL : malloc(sizeof(struct aws_byte_buf));

    /* Set up str if non-NULL */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Set up buf if non-NULL */
    if (buf != NULL) {
        __CPROVER_assume(aws_byte_buf_is_bounded(buf, MAX_STRING_LEN));
        ensure_byte_buf_has_allocated_buffer_member(buf);
        __CPROVER_assume(aws_byte_buf_is_valid(buf));
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_buf(str, buf);

    /* Postconditions */

    /* Both NULL => true */
    if (str == NULL && buf == NULL) {
        assert(result == true);
    }

    /* One NULL, other non-NULL => false */
    if (str == NULL && buf != NULL) {
        assert(result == false);
    }
    if (str != NULL && buf == NULL) {
        assert(result == false);
    }

    /* Both non-NULL: result depends on content comparison */
    if (str != NULL && buf != NULL) {
        /* If lengths differ, must be false */
        if (str->len != buf->len) {
            assert(result == false);
        }
        /* If lengths are zero, must be true */
        if (str->len == 0 && buf->len == 0) {
            assert(result == true);
        }
    }

    /* Validity invariants: inputs must remain unchanged */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (buf != NULL) {
        assert(aws_byte_buf_is_valid(buf));
    }
}
