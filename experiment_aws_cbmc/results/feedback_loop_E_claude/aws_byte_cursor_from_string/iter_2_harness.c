/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

void aws_byte_cursor_from_string_harness() {
    /* Allocate a valid aws_string with bounded length */
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate memory for the string header + data bytes + null terminator */
    struct aws_string *src = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(src != NULL);

    /* Use a writable copy to initialize fields */
    /* aws_string has const fields, so we write through a helper pointer */
    size_t *len_ptr = (size_t *)(&src->len);
    *len_ptr = len;

    /* Ensure the string is valid */
    __CPROVER_assume(aws_string_is_valid(src));

    /* Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_string(src);

    /* Postconditions */
    assert(result.len == src->len);
    assert(result.ptr == aws_string_bytes(src));
    assert(aws_byte_cursor_is_valid(&result));
}
