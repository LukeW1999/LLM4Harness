/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>

void aws_string_bytes_harness() {
    /* Allocate a non-deterministic aws_string */
    /* We need to allocate enough memory for the struct plus at least 1 byte for bytes[1] */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate memory for the string header + data bytes + null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len);
    __CPROVER_assume(str != NULL);

    /* Set up the string fields - we need to write through const fields */
    /* Use a writable view to initialize */
    size_t *len_ptr = (size_t *)&str->len;
    *len_ptr = len;

    /* Precondition: str must be a valid aws_string */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old state */
    const struct aws_string *old_str = str;
    const uint8_t *old_bytes = str->bytes;
    size_t old_len = str->len;
    struct aws_allocator *old_allocator = str->allocator;

    /* Call function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Postconditions:
     * 1. The function returns str->bytes (the pointer to the data bytes)
     * 2. The string struct is unchanged
     * 3. The string remains valid
     */

    /* Changed fields: result is the return value */
    assert(result == str->bytes);
    assert(result == old_bytes);

    /* Unchanged fields: nothing in the struct changes */
    assert(str->len == old_len);
    assert(str->allocator == old_allocator);
    assert(str->bytes == old_bytes);

    /* Validity invariant */
    assert(aws_string_is_valid(str));

    /* The result pointer must be readable (at least len bytes + null terminator) */
    /* The bytes field is always accessible since str is valid */
    assert(result != NULL);
}
