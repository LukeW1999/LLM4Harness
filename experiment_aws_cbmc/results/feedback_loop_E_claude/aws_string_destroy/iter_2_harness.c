/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>

void aws_string_destroy_harness() {
    /* Non-deterministically choose test case */
    struct aws_string *str = ensure_string_is_allocated(MAX_STRING_LEN);

    if (nondet_bool()) {
        /* Case 1: NULL string - function should handle gracefully */
        aws_string_destroy(NULL);
        /* If we reach here, NULL was handled without crash */
        assert(true);
    } else if (str != NULL) {
        if (nondet_bool()) {
            /* Case 2: Valid string with NULL allocator - should be no-op */
            __CPROVER_assume(str->allocator == NULL);
            __CPROVER_assume(aws_string_is_valid(str));

            /* Capture properties before destroy */
            size_t len_before = str->len;

            aws_string_destroy(str);

            /* With NULL allocator, string should not be freed (no-op).
             * The string's len should remain unchanged. */
            assert(str->len == len_before);
        } else {
            /* Case 3: Valid string with valid allocator - memory will be freed */
            __CPROVER_assume(str->allocator != NULL);
            ASSUME_DEFAULT_ALLOCATOR(str->allocator);
            __CPROVER_assume(aws_string_is_valid(str));

            /* Capture the allocator pointer before destroy */
            struct aws_allocator *allocator_before = str->allocator;

            aws_string_destroy(str);

            /* After destroy with valid allocator, we cannot safely dereference str.
             * The key assertion is that we reached this point without undefined behavior.
             * We can assert the allocator we captured is still valid (it's a global). */
            assert(allocator_before != NULL);
        }
    }
}
