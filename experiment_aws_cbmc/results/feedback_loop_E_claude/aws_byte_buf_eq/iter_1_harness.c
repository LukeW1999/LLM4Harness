/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_eq_harness() {
    /* Declare and set up first buffer */
    struct aws_byte_buf a;
    __CPROVER_assume(aws_byte_buf_is_bounded(&a, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&a);
    __CPROVER_assume(aws_byte_buf_is_valid(&a));

    /* Declare and set up second buffer */
    struct aws_byte_buf b;
    __CPROVER_assume(aws_byte_buf_is_bounded(&b, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&b);
    __CPROVER_assume(aws_byte_buf_is_valid(&b));

    /* Save old state before calling */
    struct aws_byte_buf old_a = a;
    struct aws_byte_buf old_b = b;

    /* Call function under test */
    bool result = aws_byte_buf_eq(&a, &b);

    /* Assert validity invariants always hold after the call */
    assert(aws_byte_buf_is_valid(&a));
    assert(aws_byte_buf_is_valid(&b));

    /* Assert that neither buffer is modified by the comparison */
    assert(a.len == old_a.len);
    assert(a.capacity == old_a.capacity);
    assert(a.allocator == old_a.allocator);
    assert(a.buffer == old_a.buffer);

    assert(b.len == old_b.len);
    assert(b.capacity == old_b.capacity);
    assert(b.allocator == old_b.allocator);
    assert(b.buffer == old_b.buffer);

    /* Assert correctness of the result:
     * If lengths differ, result must be false.
     * If lengths are equal and result is true, the contents must match. */
    if (a.len != b.len) {
        assert(result == false);
    }

    if (result == true) {
        /* Lengths must be equal when result is true */
        assert(a.len == b.len);
        /* If both are non-empty, bytes must match */
        if (a.len > 0) {
            assert_bytes_match(a.buffer, b.buffer, a.len);
        }
    }
}
