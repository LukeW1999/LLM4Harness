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
     * 1. The function returns whether the contents of cursor and buf are equivalent.
     * 2. Neither cursor nor buf should be modified by this function.
     * 3. Both structures remain valid after the call.
     */

    /* Assert cursor fields are unchanged */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);

    /* Assert buf fields are unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Assert validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_buf_is_valid(&buf));

    /* Assert correctness of result:
     * If lengths differ, result must be false.
     * If lengths are equal and both are 0, result must be true.
     */
    if (cursor.len != buf.len) {
        assert(result == false);
    }

    if (cursor.len == 0 && buf.len == 0) {
        assert(result == true);
    }
}
