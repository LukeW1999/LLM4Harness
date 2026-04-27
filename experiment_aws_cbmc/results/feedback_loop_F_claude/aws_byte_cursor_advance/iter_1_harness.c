/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    /* 1. Declare and set up the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Non-deterministic length to advance */
    size_t len;

    /* 3. Save old state before calling */
    struct aws_byte_cursor old_cursor = cursor;

    /* 4. Call function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    /* 5. Assert postconditions */

    /* Determine if the advance should succeed:
     * Fails if cursor->len > (SIZE_MAX >> 1) || len > (SIZE_MAX >> 1) || len > cursor->len */
    if (old_cursor.len <= (SIZE_MAX >> 1) && len <= (SIZE_MAX >> 1) && len <= old_cursor.len) {
        /* Success path */
        /* rv points to the first len bytes of the original cursor */
        assert(rv.ptr == old_cursor.ptr);
        assert(rv.len == len);

        /* cursor is advanced by len */
        if (old_cursor.ptr == NULL) {
            assert(cursor.ptr == NULL);
        } else {
            assert(cursor.ptr == old_cursor.ptr + len);
        }
        assert(cursor.len == old_cursor.len - len);
    } else {
        /* Failure path: rv is null/empty, cursor is unchanged */
        assert(rv.ptr == NULL);
        assert(rv.len == 0);

        /* cursor is unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* 6. Assert validity invariants always hold */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&rv));
}
