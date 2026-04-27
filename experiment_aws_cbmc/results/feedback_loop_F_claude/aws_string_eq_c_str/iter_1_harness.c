/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

void aws_string_eq_c_str_harness() {
    /* Non-deterministic inputs */
    struct aws_string *str = nondet_bool() ? NULL : malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE + 1);
    const char *c_str;

    /* Set up str if non-null */
    if (str != NULL) {
        __CPROVER_assume(aws_string_is_valid(str));
        __CPROVER_assume(str->len <= MAX_BUFFER_SIZE);
    }

    /* c_str can be NULL or a valid C string */
    bool c_str_is_null = nondet_bool();
    if (c_str_is_null) {
        c_str = NULL;
    } else {
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);
        char *c_str_buf = malloc(c_str_len + 1);
        __CPROVER_assume(c_str_buf != NULL);
        c_str_buf[c_str_len] = '\0';
        c_str = c_str_buf;
    }

    /* Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postconditions */

    /* Case 1: both NULL => true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Case 2: exactly one is NULL => false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* Case 3: neither is NULL => result depends on content comparison */
    /* We can't easily assert the exact value here without reimplementing,
     * but we can assert that str remains valid and unchanged */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    /* The function returns a bool, so result must be true or false */
    assert(result == true || result == false);
}
