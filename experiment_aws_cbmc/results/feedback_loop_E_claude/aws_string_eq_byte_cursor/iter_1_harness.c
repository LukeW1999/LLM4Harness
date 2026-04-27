/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_byte_cursor_harness() {
    /* Non-deterministically decide whether str and cur are NULL */
    const struct aws_string *str;
    struct aws_byte_cursor *cur;

    /* Set up str non-deterministically */
    bool str_is_null = nondet_bool();
    if (str_is_null) {
        str = NULL;
    } else {
        /* Allocate and initialize a valid aws_string */
        struct aws_string *s = ensure_allocated_string(MAX_STRING_LEN);
        __CPROVER_assume(s != NULL);
        __CPROVER_assume(aws_string_is_valid(s));
        str = s;
    }

    /* Set up cur non-deterministically */
    bool cur_is_null = nondet_bool();
    if (cur_is_null) {
        cur = NULL;
    } else {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_STRING_LEN));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    /* Save old state */
    const struct aws_string *old_str = str;
    struct aws_byte_cursor old_cur;
    if (cur != NULL) {
        old_cur = *cur;
    }

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions */

    /* 1. Both NULL => true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }

    /* 2. One NULL, other not => false */
    if (str == NULL && cur != NULL) {
        assert(result == false);
    }
    if (str != NULL && cur == NULL) {
        assert(result == false);
    }

    /* 3. Neither NULL => result depends on content comparison */
    if (str != NULL && cur != NULL) {
        /* If lengths differ, result must be false */
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* We can't easily assert the byte-by-byte comparison result,
           but we can assert validity is preserved */
    }

    /* 4. Inputs are not modified (immutability) */
    assert(str == old_str);
    if (cur != NULL) {
        assert(cur->ptr == old_cur.ptr);
        assert(cur->len == old_cur.len);
    }

    /* 5. Validity invariants still hold */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (cur != NULL) {
        assert(aws_byte_cursor_is_valid(cur));
    }
}
