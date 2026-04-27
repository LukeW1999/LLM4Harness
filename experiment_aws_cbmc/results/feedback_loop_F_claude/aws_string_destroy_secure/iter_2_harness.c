/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <string.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministically decide whether to pass NULL */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path: function should be a no-op (no crash) */
        aws_string_destroy_secure(NULL);
        /* If we reach here without crashing, NULL was handled correctly */
        assert(true);
    } else {
        /* Allocate memory for aws_string with data bytes */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate the string: header + len bytes + 1 null terminator */
        uint8_t *raw = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(raw != NULL);

        struct aws_string *str = (struct aws_string *)raw;

        /* Initialize fields via mutable pointer - use NULL allocator (static string case)
         * so memory is not freed and we can check postconditions */
        *(struct aws_allocator **)&str->allocator = NULL;
        *(size_t *)&str->len = len;

        /* Initialize bytes to non-zero values so we can verify zeroing */
        uint8_t *bytes_ptr = (uint8_t *)(str + 1);
        for (size_t i = 0; i <= len; i++) {
            bytes_ptr[i] = 0xAB; /* non-zero sentinel value */
        }

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save state before call */
        size_t old_len = str->len;

        /* Call the function under test with NULL allocator (static string case) */
        /* This exercises the zeroing path without freeing */
        aws_string_destroy_secure(str);

        /* Postcondition: bytes should be zeroed after secure destroy */
        const uint8_t *result_bytes = aws_string_bytes(str);
        for (size_t i = 0; i < old_len; i++) {
            assert(result_bytes[i] == 0);
        }
        /* Null terminator should also be zeroed */
        assert(result_bytes[old_len] == 0);
    }
}
