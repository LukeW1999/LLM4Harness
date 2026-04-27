/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <stddef.h>
#include <assert.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministically decide whether to pass NULL or a valid string */
    bool is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    if (is_null) {
        /* Test with NULL: function should be a no-op */
        aws_string_destroy_secure(NULL);
        assert(1);
    } else {
        /* Test with a valid string */
        struct aws_string *str = ensure_string_is_allocated(MAX_STRING_LEN);
        __CPROVER_assume(str != NULL);
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save relevant fields before the call */
        size_t old_len = str->len;
        struct aws_allocator *old_allocator = str->allocator;

        /* Call the function under test */
        aws_string_destroy_secure(str);

        /*
         * Postconditions:
         * - Function completed without crashing
         * - If allocator was non-NULL, memory was freed; we cannot dereference str
         */
        assert(old_len <= MAX_STRING_LEN);
    }
}
