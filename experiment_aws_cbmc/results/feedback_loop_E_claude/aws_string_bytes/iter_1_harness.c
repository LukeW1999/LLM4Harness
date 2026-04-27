/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_bytes_harness() {
    /* Declare and set up a valid aws_string */
    struct aws_string *str = nondet_bool() ? ensure_allocated_string(MAX_STRING_LEN) : NULL;

    /* Assume str is non-NULL and valid */
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state */
    const size_t old_len = str->len;
    struct aws_allocator *const old_allocator = str->allocator;

    /* Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions:
     * 1. The function returns str->bytes (pointer equality)
     * 2. The string fields are unchanged
     * 3. The string remains valid
     */

    /* Result must equal str->bytes */
    assert(result == str->bytes);

    /* Result must not be NULL (bytes is always valid for a valid string) */
    assert(result != NULL);

    /* Unchanged fields */
    assert(str->len == old_len);
    assert(str->allocator == old_allocator);

    /* Validity invariant still holds */
    assert(aws_string_is_valid(str));
}
