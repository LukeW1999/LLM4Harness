#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound the number of variadic args to keep state space manageable */
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

    /* Save dest state before call */
    struct aws_byte_buf old_dest = dest;

    /* 2. Set up source buffers (variadic args) */
    /* We use NUM_ARGS source buffers */
    struct aws_byte_buf src0, src1;

    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    /* Save source states before call */
    struct aws_byte_buf old_src0 = src0;
    struct aws_byte_buf old_src1 = src1;

    /* 3. Call function under test */
    int result = aws_byte_buf_cat(&dest, NUM_ARGS, &src0, &src1);

    /* 4. Assert postconditions */

    /* Return value must be AWS_OP_SUCCESS or AWS_OP_ERR */
    assert(result == 0 || result == -1);

    if (result == 0) {
        /* On success: dest->len increased by sum of src lens */
        assert(dest.len == old_dest.len + old_src0.len + old_src1.len);
        /* capacity and allocator and buffer pointer unchanged */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
    } else {
        /* On failure: dest may have been partially written (first append
         * succeeded but second failed), so we cannot assert dest.len == old_dest.len.
         * However, capacity, allocator, and buffer pointer must not change. */
        assert(dest.capacity == old_dest.capacity);
        assert(dest.allocator == old_dest.allocator);
        assert(dest.buffer == old_dest.buffer);
        /* len is >= old len (partial writes may have occurred) */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= dest.capacity);
    }

    /* 5. Source buffers must not be modified */
    assert(src0.len == old_src0.len);
    assert(src0.capacity == old_src0.capacity);
    assert(src0.buffer == old_src0.buffer);
    assert(src0.allocator == old_src0.allocator);

    assert(src1.len == old_src1.len);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.allocator == old_src1.allocator);

    /* 6. Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));

    /* 7. dest.len must always be within capacity */
    assert(dest.len <= dest.capacity);
}
