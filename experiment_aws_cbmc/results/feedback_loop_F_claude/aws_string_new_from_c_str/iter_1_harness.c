/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* Non-deterministic parameters */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create a bounded c_str for verification */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a null-terminated C string */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure the string is null-terminated */
    c_str[len] = '\0';

    /* Preconditions */
    __CPROVER_assume(allocator != NULL);

    /* Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        /* Postconditions on success */
        /* The returned string must be valid */
        assert(aws_string_is_valid(result));

        /* The length must match the C string length */
        assert(result->len == strlen(c_str));
        assert(result->len == len);

        /* The allocator must be set correctly */
        assert(result->allocator == allocator);

        /* The bytes must match the input c_str */
        assert_bytes_match((const uint8_t *)c_str, result->bytes, len);

        /* The string must be null-terminated */
        assert(result->bytes[result->len] == '\0');
    }
    /* On failure (result == NULL), no postconditions to assert about the result,
     * but we verify the function doesn't crash and returns NULL gracefully */
}
