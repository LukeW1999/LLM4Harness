/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    /* Declare and set up cursor a */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* Declare and set up cursor b */
    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save old state to check immutability */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Assert validity invariants still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));

    /* Assert cursors are unchanged (function should not modify inputs) */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    /* Assert postconditions based on result */
    if (result) {
        /* If equal, lengths must match */
        assert(a.len == b.len);
        /* If both have content, bytes must match */
        if (a.len > 0 && a.ptr != NULL && b.ptr != NULL) {
            assert_bytes_match(a.ptr, b.ptr, a.len);
        }
    } else {
        /* If not equal, either lengths differ or content differs */
        /* We can't easily assert the negative of bytes_match, but we can
           assert that if lengths differ, result must be false */
        if (a.len != b.len) {
            assert(!result);
        }
    }
}
