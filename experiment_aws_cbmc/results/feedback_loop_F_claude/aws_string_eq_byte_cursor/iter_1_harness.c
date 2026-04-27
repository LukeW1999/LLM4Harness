/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

void aws_string_eq_byte_cursor_harness() {
    /* Non-deterministic inputs */
    struct aws_string *str = NULL;
    struct aws_byte_cursor *cur = NULL;

    /* Non-deterministically decide whether str and cur are NULL */
    bool str_is_null = nondet_bool();
    bool cur_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate and set up a valid aws_string */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string header + len + 1 bytes (null terminator) */
        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields - cast away const for initialization */
        *(size_t *)&str->len = len;
        /* allocator can be anything */

        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!cur_is_null) {
        cur = malloc(sizeof(struct aws_byte_cursor));
        __CPROVER_assume(cur != NULL);
        __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_BUFFER_SIZE));
        ensure_byte_cursor_has_allocated_buffer_member(cur);
        __CPROVER_assume(aws_byte_cursor_is_valid(cur));
    }

    /* Save old state */
    struct aws_string *old_str = str;
    struct aws_byte_cursor *old_cur = cur;

    /* Save string fields if non-null */
    size_t old_str_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;

    /* Save cursor fields if non-null */
    size_t old_cur_len = (cur != NULL) ? cur->len : 0;
    uint8_t *old_cur_ptr = (cur != NULL) ? cur->ptr : NULL;

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions based on Doxygen and implementation */

    /* Case 1: both NULL -> returns true */
    if (str == NULL && cur == NULL) {
        assert(result == true);
    }
    /* Case 2: one is NULL, other is not -> returns false */
    else if (str == NULL || cur == NULL) {
        assert(result == false);
    }
    /* Case 3: both non-NULL -> result depends on content comparison */
    else {
        /* The result is a boolean - we can't assert the exact value without
         * knowing the content, but we can assert structural invariants */
        /* If lengths differ, must be false */
        if (str->len != cur->len) {
            assert(result == false);
        }
        /* If lengths are both 0, must be true */
        if (str->len == 0 && cur->len == 0) {
            assert(result == true);
        }
    }

    /* Immutability: str fields must not change */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(str->allocator == old_str_allocator);
        assert(aws_string_is_valid(str));
    }

    /* Immutability: cur fields must not change */
    if (cur != NULL) {
        assert(cur->len == old_cur_len);
        assert(cur->ptr == old_cur_ptr);
        assert(aws_byte_cursor_is_valid(cur));
    }

    /* Pointers themselves must not change */
    assert(str == old_str);
    assert(cur == old_cur);
}
