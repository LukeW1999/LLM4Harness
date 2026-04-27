/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    /* Non-deterministic: str may be NULL or non-NULL */
    struct aws_string *str = malloc(sizeof(struct aws_string) + nondet_size_t());

    if (str != NULL) {
        /* Non-deterministically set the allocator to NULL or non-NULL */
        struct aws_allocator *allocator;
        if (nondet_bool()) {
            allocator = aws_default_allocator();
        } else {
            allocator = NULL;
        }

        /* We need to write to the const fields using a cast */
        *(struct aws_allocator **)&str->allocator = allocator;

        /* Precondition: if str is non-NULL and has an allocator, it should be valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /*
     * aws_string_destroy behavior:
     * - If str is NULL: no-op (safe to call with NULL)
     * - If str is non-NULL but str->allocator is NULL: no-op (static string)
     * - If str is non-NULL and str->allocator is non-NULL: frees the memory
     *
     * The function has no return value and no postconditions to assert
     * on the string itself (since it may be freed). We just verify it
     * doesn't crash/assert-fail under valid inputs.
     */

    /* Call function under test */
    aws_string_destroy(str);

    /*
     * Postconditions:
     * - The function must not crash for any valid input (NULL or valid aws_string)
     * - If str was NULL or had NULL allocator, nothing happened (no-op)
     * - If str had a non-NULL allocator, memory was released
     *
     * Since the memory may have been freed, we cannot dereference str after the call.
     * We assert that the function completed without error (implicit: no crash).
     * The only meaningful assertion we can make is that we reached this point.
     */
    assert(1); /* Function completed without crashing */
}
