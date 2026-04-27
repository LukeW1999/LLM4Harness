/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_eq_byte_cursor_harness() {
    /* Both str and cur must be non-NULL for the function to work correctly */
    struct aws_string *str = ensure_allocated_string(MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    struct aws_byte_cursor *cur = malloc(sizeof(struct aws_byte_cursor));
    __CPROVER_assume(cur != NULL);
    __CPROVER_assume(aws_byte_cursor_is_bounded(cur, MAX_STRING_LEN));
    ensure_byte_cursor_has_allocated_buffer_member(cur);
    __CPROVER_assume(aws_byte_cursor_is_valid(cur));

    /* Save old state */
    struct aws_byte_cursor old_cur = *cur;

    /* Call function under test */
    bool result = aws_string_eq_byte_cursor(str, cur);

    /* Postconditions */

    /* Inputs are not modified (immutability) */
    assert(cur->ptr == old_cur.ptr);
    assert(cur->len == old_cur.len);

    /* Validity invariants still hold */
    assert(aws_string_is_valid(str));
    assert(aws_byte_cursor_is_valid(cur));
}
