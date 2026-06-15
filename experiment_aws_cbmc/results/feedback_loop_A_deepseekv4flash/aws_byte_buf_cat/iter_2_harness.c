#include <aws/common/byte_buf.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/proof_allocators.h"

void aws_byte_buf_cat_harness() {
    /* nondet parameters */
    struct aws_byte_buf dest;
    struct aws_byte_buf src1;
    struct aws_byte_buf src2;

    /* bound buffers to limit state space */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));

    /* ensure buffers are allocated */
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    ensure_byte_buf_has_allocated_buffer_member(&src2);

    /* assume validity */
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* save old state */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    /* call the function with exactly 2 additional buffers */
    int result = aws_byte_buf_cat(&dest, 2, &src1, &src2);

    /* postconditions */

    /* dest remains valid */
    assert(aws_byte_buf_is_valid(&dest));

    /* source buffers unchanged (they are not modified) */
    assert(aws_byte_buf_is_valid(&src1));
    assert(src1.len == old_src1.len);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);

    assert(aws_byte_buf_is_valid(&src2));
    assert(src2.len == old_src2.len);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);

    /* structural invariants for dest */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer); /* no reallocation */

    /* both success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        /* dest->len increased by sum of source lengths */
        assert(dest.len == old_dest.len + old_src1.len + old_src2.len);
    } else {
        /* on failure, dest->len is increased by at most the total source length */
        assert(dest.len >= old_dest.len);
        assert(dest.len <= old_dest.len + old_src1.len + old_src2.len);
    }
}
