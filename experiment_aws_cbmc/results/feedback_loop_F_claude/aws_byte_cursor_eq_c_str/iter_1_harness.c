/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* 1. Declare and set up the aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Save old cursor state for immutability checks */
    struct aws_byte_cursor old_cursor = cursor;

    /* 3. Set up a non-deterministic null-terminated c_str */
    const char *c_str = ensure_c_str_is_allocated(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);

    /* 4. Call the function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Assert postconditions */

    /* The function returns a bool - no success/failure path distinction needed,
       but we can assert the result is a valid bool */
    assert(result == true || result == false);

    /* The cursor must not be modified by this comparison function */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* The cursor must remain valid after the call */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* The c_str must remain a valid C string after the call */
    assert(aws_c_string_is_valid(c_str));

    /* Verify the result is consistent with aws_array_eq_c_str semantics:
       If result is true, the cursor contents match the c_str.
       We verify this by checking the underlying array equality function. */
    bool expected = aws_array_eq_c_str(cursor.ptr, cursor.len, c_str);
    assert(result == expected);
}
