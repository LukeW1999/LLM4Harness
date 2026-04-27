/**
 * Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0.
 */

#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef NUMBER_OF_ARGS
#    define NUMBER_OF_ARGS 2
#endif

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_cat_harness() {
    /* Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save old state of dest */
    struct aws_byte_buf old_dest = dest;

    /* Set up source buffers */
    struct aws_byte_buf buf0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf0);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf0));

#if NUMBER_OF_ARGS >= 2
    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));
#endif

    /* Call function under test */
#if NUMBER_OF_ARGS == 1
    int result = aws_byte_buf_cat(&dest, 1, &buf0);
#elif NUMBER_OF_ARGS == 2
    int result = aws_byte_buf_cat(&dest, 2, &buf0, &buf1);
#else
    int result = aws_byte_buf_cat(&dest, 1, &buf0);
#endif

    /* Assert validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));

    /* Assert unchanged fields regardless of result */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest->len contains the amount of data actually copied */
        /* dest->len >= old_dest->len (we appended data) */
        assert(dest.len >= old_dest.len);

#if NUMBER_OF_ARGS == 1
        /* All bytes from buf0 were appended */
        assert(dest.len == old_dest.len + buf0.len);
#elif NUMBER_OF_ARGS == 2
        /* All bytes from buf0 and buf1 were appended */
        assert(dest.len == old_dest.len + buf0.len + buf1.len);
#endif

        /* dest->len must not exceed capacity */
        assert(dest.len <= dest.capacity);
    } else {
        /* On failure: dest->len contains the amount of data actually copied before failure */
        /* dest->len may have been partially updated, but capacity and buffer pointer unchanged */
        assert(dest.len <= dest.capacity);
        assert(dest.len >= old_dest.len);
    }

    /* Source buffers should remain valid and unchanged */
    assert(aws_byte_buf_is_valid(&buf0));
#if NUMBER_OF_ARGS >= 2
    assert(aws_byte_buf_is_valid(&buf1));
#endif
}
