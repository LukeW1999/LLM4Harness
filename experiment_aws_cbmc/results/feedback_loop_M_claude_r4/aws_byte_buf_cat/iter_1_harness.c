#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* We test with a fixed small number of args to keep state space bounded */
#define NUM_ARGS 2

void aws_byte_buf_cat_harness(void) {
    /* 1. Set up destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Set up source buffers */
    struct aws_byte_buf buf0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf0);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf0));

    struct aws_byte_buf buf1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf1);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf1));

    /* 3. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_buf0 = buf0;
    struct aws_byte_buf old_buf1 = buf1;

    /* 4. Call function under test */
    int result = aws_byte_buf_cat(&dest, NUM_ARGS, &buf0, &buf1);

    /* 5. Assert postconditions */

    /* Fields that must never change regardless of result */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    /* Source buffers must not be modified */
    assert(buf0.len == old_buf0.len);
    assert(buf0.capacity == old_buf0.capacity);
    assert(buf0.buffer == old_buf0.buffer);
    assert(buf0.allocator == old_buf0.allocator);

    assert(buf1.len == old_buf1.len);
    assert(buf1.capacity == old_buf1.capacity);
    assert(buf1.buffer == old_buf1.buffer);
    assert(buf1.allocator == old_buf1.allocator);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len should have increased by the total bytes copied */
        assert(dest.len == old_dest.len + buf0.len + buf1.len);
        assert(dest.len <= dest.capacity);
    } else {
        /* On failure, dest->len contains the amount actually copied before failure.
         * The function returns error when dest is too small.
         * dest->len may have been partially updated (up to the point of failure),
         * but capacity and buffer pointer remain unchanged. */
        assert(dest.len <= dest.capacity);
    }

    /* Validity invariants must hold after the call */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&buf0));
    assert(aws_byte_buf_is_valid(&buf1));
}
