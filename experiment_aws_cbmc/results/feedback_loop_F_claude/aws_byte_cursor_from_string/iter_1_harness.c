/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_byte_cursor_from_string_harness() {
    /* Non-deterministically choose whether src is NULL or a valid string */
    bool src_is_null = nondet_bool();

    struct aws_string *src = NULL;

    if (!src_is_null) {
        /* Allocate a valid aws_string */
        src = ensure_string_is_allocated_bounded_length(MAX_STRING_LEN);
        __CPROVER_assume(aws_string_is_valid(src));
    }

    /* Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    if (src == NULL) {
        /* If src is NULL, returns an empty cursor */
        assert(result.ptr == NULL);
        assert(result.len == 0);
    } else {
        /* If src is valid, returns cursor pointing to string bytes with matching length */
        assert(result.len == src->len);
        assert(result.ptr == aws_string_bytes(src));
        assert(aws_string_is_valid(src));
        assert(aws_byte_cursor_is_valid(&result));
    }
}
