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

    /* Save old state of sources */
    struct aws_byte_buf old_src0 = src0;
#if NUM_ARGS >= 2
    struct aws_byte_buf old_src1 = src1;
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

    /* dest is always valid after the call */
    assert(aws_byte_buf_is_valid(&dest));

    /* dest's allocator, capacity, and buffer pointer do not change */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len contains the amount of data actually copied */
        /* dest->len >= old_dest->len (we appended data) */
        assert(dest.len >= old_dest.len);
        /* dest->len <= dest->capacity */
        assert(dest.len <= dest.capacity);
    }
    /* On failure, dest.len may be partially updated - no assertion on dest.len */

    /* Source buffers must not be modified */
    assert(src0.len == old_src0.len);
    assert(src0.capacity == old_src0.capacity);
    assert(src0.buffer == old_src0.buffer);
    assert(src0.allocator == old_src0.allocator);
    assert(aws_byte_buf_is_valid(&src0));

#if NUM_ARGS >= 2
    assert(src1.len == old_src1.len);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.allocator == old_src1.allocator);
    assert(aws_byte_buf_is_valid(&src1));
#endif
}
