/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_append_harness() {
    /* 1. Declare and set up the destination buffer */
    struct aws_byte_buf to;
    __CPROVER_assume(aws_byte_buf_is_bounded(&to, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&to);
    __CPROVER_assume(aws_byte_buf_is_valid(&to));

    /* 2. Declare and set up the source cursor */
    struct aws_byte_cursor from;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&from, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&from);
    __CPROVER_assume(aws_byte_cursor_is_valid(&from));

    /* 3. Save old state before calling */
    struct aws_byte_buf old_to = to;
    struct aws_byte_cursor old_from = from;

    /* 4. Call function under test */
    int result = aws_byte_buf_append(&to, &from);

    /* 5. Assert postconditions */
    if (result == AWS_OP_SUCCESS) {
        /* On success: to->len increases by from->len */
        assert(to.len == old_to.len + old_from.len);
        /* buffer pointer, capacity, and allocator are unchanged */
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
    } else {
        /* On failure (AWS_ERROR_DEST_COPY_TOO_SMALL): to is unchanged */
        assert(to.len == old_to.len);
        assert(to.buffer == old_to.buffer);
        assert(to.capacity == old_to.capacity);
        assert(to.allocator == old_to.allocator);
    }

    /* 6. The cursor (from) must not be modified in either case */
    assert(from.len == old_from.len);
    assert(from.ptr == old_from.ptr);

    /* 7. Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&to));
    assert(aws_byte_cursor_is_valid(&from));
}
