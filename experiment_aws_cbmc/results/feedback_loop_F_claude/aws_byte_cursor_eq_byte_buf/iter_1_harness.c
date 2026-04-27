/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_byte_buf_harness() {
    /* Declare and set up aws_byte_cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Declare and set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state before calling */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    /* Call function under test */
    bool result = aws_byte_cursor_eq_byte_buf(&cursor, &buf);

    /* Postconditions:
     * 1. The function returns true iff the contents of cursor and buf are equivalent.
     * 2. Neither cursor nor buf are modified.
     * 3. Both remain valid after the call.
     */

    /* Cursor fields must not change */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* Buf fields must not change */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Validity invariants must hold */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));

    /* If lengths differ, result must be false */
    if (cursor.len != buf.len) {
        assert(!result);
    }

    /* If both have zero length, result must be true */
    if (cursor.len == 0 && buf.len == 0) {
        assert(result);
    }

    /* If result is true, lengths must be equal */
    if (result) {
        assert(cursor.len == buf.len);
    }
}
