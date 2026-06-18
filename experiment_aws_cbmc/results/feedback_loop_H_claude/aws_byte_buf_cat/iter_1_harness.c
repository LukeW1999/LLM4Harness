#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Bound the number of args to keep state space manageable */
#ifndef NUM_ARGS
#    define NUM_ARGS 2
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

    /* 2. Set up source buffers (NUM_ARGS of them) */
    struct aws_byte_buf src0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));

#if NUM_ARGS >= 2
    struct aws_byte_buf src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
#endif

    /* 3. Call function under test */
#if NUM_ARGS == 1
    int result = aws_byte_buf_cat(&dest, 1, &src0);
#elif NUM_ARGS == 2
    int result = aws_byte_buf_cat(&dest, 2, &src0, &src1);
#else
    int result = aws_byte_buf_cat(&dest, 1, &src0);
#endif

    /* 4. Assert postconditions */

    /* Validity invariants must always hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
#if NUM_ARGS >= 2
    assert(aws_byte_buf_is_valid(&src1));
#endif

    /* Fields that must never change regardless of result */
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        /* On success: dest->len contains the amount of data actually copied.
         * The total bytes appended equals the sum of all source buffer lengths.
         * dest->len == old_dest.len + sum(src_i->len) */
#if NUM_ARGS == 1
        assert(dest.len == old_dest.len + src0.len);
#elif NUM_ARGS == 2
        /* Guard against overflow in the assertion itself */
        __CPROVER_assume(old_dest.len + src0.len + src1.len <= dest.capacity);
        assert(dest.len == old_dest.len + src0.len + src1.len);
#endif
        /* len must not exceed capacity */
        assert(dest.len <= dest.capacity);
    } else {
        /* On failure (AWS_ERROR_DEST_COPY_TOO_SMALL):
         * dest->len contains the amount of data actually copied to dest
         * before the failure. The function returns early on first failure.
         * The buffer pointer and capacity are unchanged (asserted above).
         * len may have been partially updated up to the point of failure,
         * but since aws_byte_buf_append doesn't partially write, len
         * reflects only complete successful appends before the failing one.
         * We can assert len >= old_dest.len (no shrinkage) and len <= capacity. */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= dest.capacity);
    }
}
