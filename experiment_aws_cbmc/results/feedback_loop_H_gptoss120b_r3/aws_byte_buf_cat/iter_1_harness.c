#include <aws/common/byte_buf.h>
#include <aws/common/array_list.h>
#include <aws/common/linked_list.h>
#include <aws/common/math.h>
#include <aws/common/string.h>
#include <aws/common/ring_buffer.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_ARGS 2

void aws_byte_buf_cat_harness() {
    /* destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* source buffers */
    struct aws_byte_buf src0;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    __CPROVER_assume(aws_byte_buf_is_valid(&src0));

    struct aws_byte_buf src1;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    /* number of arguments (0..MAX_ARGS) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args <= MAX_ARGS);

    /* Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src0 = src0;
    struct aws_byte_buf old_src1 = src1;

    /* Compute total length of all source buffers that will be considered */
    size_t total_src_len = 0;
    if (number_of_args >= 1) {
        total_src_len += src0.len;
    }
    if (number_of_args >= 2) {
        total_src_len += src1.len;
    }

    /* Call function under test */
    int result = aws_byte_buf_cat(&dest, number_of_args, &src0, &src1);

    /* Unchanged fields of dest */
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    /* Unchanged fields of source buffers */
    assert(src0.len == old_src0.len);
    assert(src0.buffer == old_src0.buffer);
    assert(src0.capacity == old_src0.capacity);
    assert(src0.allocator == old_src0.allocator);

    assert(src1.len == old_src1.len);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);

    /* Postconditions depending on result */
    if (result == AWS_OP_SUCCESS) {
        /* All source data appended */
        assert(dest.len == old_dest.len + total_src_len);
    } else {
        /* Append failed before all data could be copied */
        assert(dest.len <= old_dest.len + total_src_len);
        assert(dest.len < old_dest.len + total_src_len);
    }

    /* Validity invariants */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));
}
