/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdlib.h>

void aws_byte_buf_eq_c_str_harness() {
    /* Set up aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state before calling */
    struct aws_byte_buf old_buf = buf;
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;
    uint8_t *old_buffer = buf.buffer;

    /* Set up c_str - must be non-NULL per precondition */
    const char *c_str = ensure_c_str_is_allocated(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);

    /* Call function under test */
    bool result = aws_byte_buf_eq_c_str(&buf, c_str);

    /* Assert the buffer was not modified */
    assert(buf.len == old_len);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);
    assert(buf.buffer == old_buffer);

    /* Assert validity invariant still holds after the call */
    assert(aws_byte_buf_is_valid(&buf));
}
