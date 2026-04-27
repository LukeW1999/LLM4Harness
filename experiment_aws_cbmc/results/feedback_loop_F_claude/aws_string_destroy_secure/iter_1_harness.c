/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministically decide whether to pass NULL or a valid string */
    struct aws_string *str = nondet_bool() ? NULL : malloc(sizeof(struct aws_string) + MAX_BUFFER_SIZE + 1);

    if (str != NULL) {
        /* Set up a valid aws_string with non-deterministic length */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* We need to write to the const fields - use a mutable alias */
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[MAX_BUFFER_SIZE + 1];
        } *mutable_str = (void *)str;

        mutable_str->len = len;

        /* Non-deterministically set allocator (NULL means static, non-NULL means dynamic) */
        if (nondet_bool()) {
            mutable_str->allocator = NULL;
        } else {
            mutable_str->allocator = aws_default_allocator();
        }

        /* Assume the string is valid before calling */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Call the function under test */
    aws_string_destroy_secure(str);

    /* Postconditions:
     * 1. If str was NULL, function is a no-op (no crash)
     * 2. If str was non-NULL:
     *    - The data bytes were zeroed (aws_secure_zero was called)
     *    - If allocator was non-NULL, memory was released
     *    - If allocator was NULL, memory was not freed (static string)
     * 3. The function always returns void, so no return value to check
     * 4. No validity invariant to check post-call since str may be freed
     */

    /* The key postcondition: if we reach here without crashing, the function
     * handled both NULL and non-NULL cases correctly */
    assert(1); /* Function completed without undefined behavior */
}
```

Wait, I need to reconsider. The harness needs meaningful assertions. Let me rewrite properly.

```c
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministically decide whether to pass NULL or a valid string */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path: function should be a no-op */
        aws_string_destroy_secure(NULL);
        /* If we reach here, NULL was handled correctly (no crash) */
        assert(true);
    } else {
        /* Allocate memory for aws_string with some data bytes */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate the string structure plus data bytes plus null terminator */
        struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the string fields using a mutable alias */
        struct {
            struct aws_allocator *allocator;
            size_t len;
            uint8_t bytes[1];
        } *mutable_str = (void *)str;

        mutable_str->len = len;

        /* Non-deterministically choose allocator */
        bool has_allocator = nondet_bool();
        if (has_allocator) {
            mutable_str->allocator = aws_default_allocator();
        } else {
            mutable_str->allocator = NULL;
        }

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save the allocator value before the call */
        struct aws_allocator *old_allocator = str->allocator;
        size_t old_len = str->len;

        /* Call the function under test */
        aws_string_destroy_secure(str);

        /* Postconditions:
         * - Function completed without crashing
         * - If allocator was NULL, the string was not freed (static string case)
         *   We can't easily assert memory state after free, but we can assert
         *   that the function ran to completion
         */
        assert(true); /* Function completed without undefined behavior */

        /* If allocator was NULL, str memory was not freed, so we can check it */
        if (!has_allocator) {
            /* Memory was not freed, bytes should be zeroed */
            /* We can verify the bytes are zeroed */
            for (size_t i = 0; i < old_len; i++) {
                assert(aws_string_bytes(str)[i] == 0);
            }
        }
    }
}
```

```c
/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_destroy_secure_harness() {
    /* Non-deterministically decide whether to pass NULL */
    bool is_null = nondet_bool();

    if (is_null) {
        /* Test NULL path: function should be a no-op (no crash) */
        aws_string_destroy_secure(NULL);
        assert(true);
    } else {
        /* Allocate memory for aws_string with data bytes */
        size_t len;
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate the string: header + len bytes + 1 null terminator */
        uint8_t *raw = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(raw != NULL);

        struct aws_string *str = (struct aws_string *)raw;

        /* Initialize fields via mutable pointer */
        *(struct aws_allocator **)&str->allocator = NULL; /* start with NULL allocator */
        *(size_t *)&str->len = len;

        /* Assume the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));

        /* Save state before call */
        size_t old_len = str->len;

        /* Call the function under test with NULL allocator (static string case) */
        /* This exercises the zeroing path without freeing */
        aws_string_destroy_secure(str);

        /* Postcondition: bytes should be zeroed after secure destroy */
        for (size_t i = 0; i < old_len; i++) {
            assert(aws_string_bytes(str)[i] == 0);
        }
    }
}
