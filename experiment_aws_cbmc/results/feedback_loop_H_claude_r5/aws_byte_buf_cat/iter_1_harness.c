#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

/* Bound the number of args to keep state space manageable */
#ifndef MAX_NUMBER_OF_ARGS
#    define MAX_NUMBER_OF_ARGS 2
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

    /* 2. Choose number of args (bounded) */
    size_t number_of_args;
    __CPROVER_assume(number_of_args <= MAX_NUMBER_OF_ARGS);

    /* 3. Set up source buffers */
    struct aws_byte_buf src0;
    struct aws_byte_buf src1;

    if (number_of_args >= 1) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src0);
        __CPROVER_assume(aws_byte_buf_is_valid(&src0));
    }

    if (number_of_args >= 2) {
        __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
        ensure_byte_buf_has_allocated_buffer_member(&src1);
        __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    }

    /* 4. Call function under test */
    int result;
    if (number_of_args == 0) {
        result = aws_byte_buf_cat(&dest, 0);
    } else if (number_of_args == 1) {
        result = aws_byte_buf_cat(&dest, 1, &src0);
    } else {
        result = aws_byte_buf_cat(&dest, 2, &src0, &src1);
    }

    /* 5. Assert postconditions */

    /* Validity invariant must always hold */
    assert(aws_byte_buf_is_valid(&dest));

    /* Fields that never change */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);

    if (result == AWS_OP_SUCCESS) {
        /* On success, dest->len contains the amount of data actually copied */
        /* dest->len >= old_dest.len (we only append) */
        assert(dest.len >= old_dest.len);
        /* dest->len <= dest->capacity */
        assert(dest.len <= dest.capacity);

        /* For 0 args, nothing changes */
        if (number_of_args == 0) {
            assert(dest.len == old_dest.len);
        }

        /* For 1 arg, dest->len == old_dest.len + src0.len */
        if (number_of_args == 1) {
            assert(dest.len == old_dest.len + src0.len);
        }

        /* For 2 args, dest->len == old_dest.len + src0.len + src1.len */
        if (number_of_args == 2) {
            assert(dest.len == old_dest.len + src0.len + src1.len);
        }
    } else {
        /* On failure (AWS_OP_ERR), dest->len contains the amount actually copied
         * before the failure. The function returns early on first failure.
         * dest->len may have been partially updated. */
        /* At minimum, dest->len >= old_dest.len (we only ever append) */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= dest.capacity);
    }

    /* Source buffers should not be modified */
    if (number_of_args >= 1) {
        assert(aws_byte_buf_is_valid(&src0));
    }
    if (number_of_args >= 2) {
        assert(aws_byte_buf_is_valid(&src1));
    }
}
