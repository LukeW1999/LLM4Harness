/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_harness() {
    /* Declare and bound cursor a */
    struct aws_byte_cursor a;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_cursor_is_valid(&a));

    /* Declare and bound cursor b */
    struct aws_byte_cursor b;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_cursor_is_valid(&b));

    /* Save old state before calling */
    struct aws_byte_cursor old_a = a;
    struct aws_byte_cursor old_b = b;

    /* Call function under test */
    bool result = aws_byte_cursor_eq(&a, &b);

    /* Assert postconditions */

    /* The function compares contents; if true, lengths must be equal */
    if (result) {
        assert(a.len == b.len);
        /* If both have non-zero length, bytes must match */
        if (a.len > 0 && a.ptr != NULL && b.ptr != NULL) {
            assert_bytes_match(a.ptr, b.ptr, a.len);
        }
    }

    /* Neither cursor should be modified by the comparison */
    assert(a.ptr == old_a.ptr);
    assert(a.len == old_a.len);
    assert(b.ptr == old_b.ptr);
    assert(b.len == old_b.len);

    /* Validity invariants must still hold */
    assert(aws_byte_cursor_is_valid(&a));
    assert(aws_byte_cursor_is_valid(&b));
}
