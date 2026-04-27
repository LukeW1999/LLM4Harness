/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 32
#endif

void aws_string_eq_harness() {
    /* Allocate two aws_string objects with bounded length */
    struct aws_string *a = ensure_arbitrary_aws_string_is_allocated_bounded_length(MAX_STRING_LEN);
    struct aws_string *b = ensure_arbitrary_aws_string_is_allocated_bounded_length(MAX_STRING_LEN);

    /* Assume both are valid (non-NULL and well-formed) */
    __CPROVER_assume(aws_string_is_valid(a));
    __CPROVER_assume(aws_string_is_valid(b));

    /* Save lengths before call */
    size_t old_a_len = a->len;
    size_t old_b_len = b->len;

    /* Call function under test */
    bool result = aws_string_eq(a, b);

    /* Postcondition: if same pointer, must return true */
    if (a == b) {
        assert(result == true);
    }

    /* Postcondition: if lengths differ, must return false */
    if (a->len != b->len) {
        assert(result == false);
    }

    /* Postcondition: strings remain valid after the call */
    assert(aws_string_is_valid(a));
    assert(aws_string_is_valid(b));

    /* Postcondition: lengths unchanged */
    assert(a->len == old_a_len);
    assert(b->len == old_b_len);

    /* Result is a valid bool */
    assert(result == true || result == false);
}
