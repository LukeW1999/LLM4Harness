#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

void aws_byte_buf_cat_harness() {
    /* Non-deterministic data structures */
    struct aws_byte_buf dest;
    struct aws_byte_buf src1;
    struct aws_byte_buf src2;

    /* Bounding and allocation */
    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&dest);
    __CPROVER_assume(aws_byte_buf_is_valid(&dest));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));

    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src2);
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    /* Save old states */
    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    size_t total_src_len = old_src1.len + old_src2.len;
    size_t new_len = old_dest.len + total_src_len;

    /* Call the function */
    int result = aws_byte_buf_cat(&dest, 2, &src1, &src2);

    /* Unconditional assertions */
    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));

    /* dest fields that must not change */
    assert(dest.allocator == old_dest.allocator);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.buffer == old_dest.buffer);

    /* source buffers must be unchanged */
    assert(src1.allocator == old_src1.allocator);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.len == old_src1.len);

    assert(src2.allocator == old_src2.allocator);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.len == old_src2.len);

    /* Result must be valid */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* dest.len bounds */
    assert(dest.len >= old_dest.len);
    assert(dest.len <= old_dest.len + total_src_len);

    /* Conditional assertions for success case */
    if (result == AWS_OP_SUCCESS) {
        assert(dest.len == new_len);
        assert(new_len <= old_dest.capacity);
    }

    /* On failure, we cannot assert exact len but it is within bounds */
}
