#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_ARGS 4

void aws_byte_buf_cat_harness(void) {
    /* 1. Destination buffer */
    struct aws_byte_buf dest;
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    /* 2. Source buffers (up to MAX_ARGS) */
    struct aws_byte_buf src0, src1, src2, src3;
    ensure_byte_buf_has_allocated_buffer_member(&src0);
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    ensure_byte_buf_has_allocated_buffer_member(&src3);

    __CPROVER_assume(aws_byte_buf_is_bounded(&src0, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src3, MAX_BUFFER_SIZE));

    __CPROVER_assume(aws_byte_buf_is_valid(&src0));
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));
    __CPROVER_assume(aws_byte_buf_is_valid(&src3));

    /* 3. Number of arguments (must be >1) */
    size_t number_of_args = nondet_size_t();
    __CPROVER_assume(number_of_args >= 2 && number_of_args <= MAX_ARGS);

    /* 4. Save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src0 = src0;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;
    struct aws_byte_buf old_src3 = src3;

    /* 5. Call function under test */
    int result;
    if (number_of_args == 2) {
        result = aws_byte_buf_cat(&dest, 2, &src0, &src1);
    } else if (number_of_args == 3) {
        result = aws_byte_buf_cat(&dest, 3, &src0, &src1, &src2);
    } else { /* number_of_args == 4 */
        result = aws_byte_buf_cat(&dest, 4, &src0, &src1, &src2, &src3);
    }

    /* 6. Compute total length of all source buffers */
    size_t total_len = 0;
    if (number_of_args >= 1) total_len += src0.len;
    if (number_of_args >= 2) total_len += src1.len;
    if (number_of_args >= 3) total_len += src2.len;
    if (number_of_args >= 4) total_len += src3.len;

    /* 7. Post‑condition checks */
    if (result == AWS_OP_SUCCESS) {
        /* dest length increased by the sum of source lengths */
        assert(dest.len == old_dest.len + total_len);
    } else {
        /* on failure dest length is non‑decreasing and cannot exceed the total possible */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + total_len);
    }

    /* Unchanged fields of dest */
    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    /* Source buffers must remain unchanged */
    assert(src0.len == old_src0.len);
    assert(src0.buffer == old_src0.buffer);
    assert(src0.capacity == old_src0.capacity);
    assert(src0.allocator == old_src0.allocator);

    assert(src1.len == old_src1.len);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);

    assert(src2.len == old_src2.len);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);

    assert(src3.len == old_src3.len);
    assert(src3.buffer == old_src3.buffer);
    assert(src3.capacity == old_src3.capacity);
    assert(src3.allocator == old_src3.allocator);

    /* 8. Validity invariants must hold */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src0));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
    assert(aws_byte_buf_is_valid(&src3));
}
