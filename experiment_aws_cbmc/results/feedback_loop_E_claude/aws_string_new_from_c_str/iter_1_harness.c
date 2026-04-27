/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>

void aws_string_new_from_c_str_harness() {
    /* parameters */
    const char *c_str = ensure_c_str_is_allocated(MAX_STRING_LEN);
    struct aws_allocator *allocator;

    /* assumptions */
    __CPROVER_assume(c_str != NULL);
    ASSUME_DEFAULT_ALLOCATOR(allocator);

    /* operation under verification */
    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);

    /* assertions */
    if (str) {
        /* The resulting string length must match strlen(c_str) */
        assert(str->len <= MAX_STRING_LEN);
        /* Null terminator must be present after the data */
        assert(str->bytes[str->len] == 0);
        /* Bytes must match the input c_str */
        assert_bytes_match(str->bytes, (const uint8_t *)c_str, str->len);
        /* The allocator field must be set to the provided allocator */
        assert(str->allocator == allocator);
        /* The resulting string must be valid */
        assert(aws_string_is_valid(str));
    }
    /* The input c_str must remain valid after the call */
    assert(aws_c_string_is_valid(c_str));
}
