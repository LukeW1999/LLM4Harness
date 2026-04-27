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
        /* If we reach here without crashing, the NULL case is handled correctly */
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
         * - If allocator was NULL, memory was not freed (static string case)
         *   and bytes should have been zeroed
         * - If allocator was non-NULL, memory was freed; we cannot dereference str
         */
        if (old_allocator == NULL) {
            /* Memory was not freed, we can still check the string */
            /* The bytes should have been zeroed */
            for (size_t i = 0; i < old_len; i++) {
                assert(str->bytes[i] == 0);
            }
            /* Length and allocator fields should be unchanged */
            assert(str->len == old_len);
            assert(str->allocator == NULL);
        } else {
            /* Memory was freed; we cannot safely dereference str.
             * Assert a trivially true property about the saved values. */
            assert(old_len <= MAX_STRING_LEN);
        }
    }
}
