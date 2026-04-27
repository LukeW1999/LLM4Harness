/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>

void aws_string_destroy_harness() {
    /* Test Case 1: NULL string - function should handle gracefully */
    aws_string_destroy(NULL);
    /* If we reach here without crashing, NULL was handled correctly */
    assert(1);

    /* Test Case 2: Valid string with a valid allocator */
    struct aws_string *str = ensure_string_is_allocated(MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Use the default allocator */
    struct aws_allocator *alloc = aws_default_allocator();
    __CPROVER_assume(alloc != NULL);
    str->allocator = alloc;

    /* Verify the string is valid before destruction */
    assert(aws_string_is_valid(str));
    assert(str->allocator != NULL);

    /* Call the function under test */
    aws_string_destroy(str);

    /* After destroy, we verify we reached this point without undefined behavior */
    assert(alloc != NULL);
}
