/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* 1. Declare and set up the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Save old cursor state */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Set up the c_str parameter (non-deterministic null-terminated string) */
    const char *c_str;
    /* c_str can be NULL only if we want to test that path, but the function
       passes it to aws_array_eq_c_str which handles NULL c_str.
       We use a non-deterministic c_str that may or may not be NULL. */

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* The function returns a bool - no success/failure path distinction needed,
       it simply returns whether the contents are equivalent */

    /* The cursor must not be modified by this comparison function */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Validity invariant must still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
}
