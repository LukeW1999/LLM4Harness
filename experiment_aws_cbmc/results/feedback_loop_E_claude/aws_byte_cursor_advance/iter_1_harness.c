/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    /* Declare and set up the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Non-deterministic length to advance */
    size_t len;

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call function under test */
    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    /* Assert validity invariants */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&rv));

    /* Check postconditions based on whether advance succeeded or failed */
    if (rv.ptr != NULL) {
        /* Success path: len <= cursor->len and neither len nor cursor->len > SIZE_MAX/2 */
        assert(rv.ptr == old_cursor.ptr);
        assert(rv.len == len);
        assert(cursor.len == old_cursor.len - len);
        if (old_cursor.ptr != NULL) {
            assert(cursor.ptr == old_cursor.ptr + len);
        } else {
            assert(cursor.ptr == NULL);
        }
    } else {
        /* Failure path: cursor is unchanged */
        assert(rv.len == 0);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }
}
