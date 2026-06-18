#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Bound the number of buffers to keep the proof tractable */
#ifndef NUM_BUFFERS
#    define NUM_BUFFERS 2
#endif

#ifndef MAX_BUFFER_SIZE
#    define MAX_BUFFER_SIZE 8
#endif

void aws_byte_buf_cat_harness(void) {
    /* 1. Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* Save old state of dest */
    struct aws_byte_buf old_dest = dest;

    /* 2. Set up source buffers (fixed number for bounded verification) */
    struct aws_byte_buf buf0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf0);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf0));

    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));

    /* Save old state of source buffers */
    struct aws_byte_buf old_buf0 = buf0;
    struct aws_byte_buf old_buf1 = buf1;

    /* 3. Call function under test */
    int result = aws_byte_buf_cat(&dest, NUM_BUFFERS, &buf0, &buf1);

    /* 4. Assert postconditions */

    /* Validity invariants always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&buf0));
    assert(aws_byte_buf_is_valid(&buf1));

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest->len contains the amount of data actually copied */
        /* dest->len >= old_dest.len (we appended data) */
        assert(dest.len >= old_dest.len);
        /* dest->len == old_dest.len + buf0.len + buf1.len */
        assert(dest.len == old_dest.len + buf0.len + buf1.len);
        /* capacity and allocator and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
    } else {
        /* On failure: dest->len contains the amount actually copied before failure */
        /* dest->len may have been partially updated, but capacity/allocator/buffer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        /* len is between old len and capacity */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= dest.capacity);
    }

    /* Source buffers are not modified by cat */
    assert(buf0.len == old_buf0.len);
    assert(buf0.capacity == old_buf0.capacity);
    assert(buf0.allocator == old_buf0.allocator);
    assert(buf0.buffer == old_buf0.buffer);

    assert(buf1.len == old_buf1.len);
    assert(buf1.capacity == old_buf1.capacity);
    assert(buf1.allocator == old_buf1.allocator);
    assert(buf1.buffer == old_buf1.buffer);
}
