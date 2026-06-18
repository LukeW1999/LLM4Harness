#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_cat_harness() {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_buf dest;
    struct aws_byte_buf src1;
    struct aws_byte_buf src2;

    __CPROVER_assume(aws_byte_buf_is_bounded(&dest, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src1, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_bounded(&src2, MAX_BUFFER_SIZE));

    ensure_byte_buf_has_allocated_buffer_member(&dest);
    ensure_byte_buf_has_allocated_buffer_member(&src1);
    ensure_byte_buf_has_allocated_buffer_member(&src2);

    dest.allocator = allocator;
    src1.allocator = allocator;
    src2.allocator = allocator;

    __CPROVER_assume(aws_byte_buf_is_valid(&dest));
    __CPROVER_assume(aws_byte_buf_is_valid(&src1));
    __CPROVER_assume(aws_byte_buf_is_valid(&src2));

    __CPROVER_assume(src1.len <= SIZE_MAX - src2.len);
    size_t total_to_copy = src1.len + src2.len;

    struct aws_byte_buf old_dest = dest;
    struct aws_byte_buf old_src1 = src1;
    struct aws_byte_buf old_src2 = src2;

    size_t available = old_dest.capacity - old_dest.len;

    int result = aws_byte_buf_cat(&dest, 2, &src1, &src2);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(total_to_copy <= available);
        assert(dest.len == old_dest.len + total_to_copy);
    } else {
        assert(total_to_copy > available);

        if (old_src1.len > available) {
            assert(dest.len == old_dest.len);
        } else {
            assert(dest.len == old_dest.len + old_src1.len);
        }
    }

    assert(dest.buffer == old_dest.buffer);
    assert(dest.capacity == old_dest.capacity);
    assert(dest.allocator == old_dest.allocator);

    assert(src1.len == old_src1.len);
    assert(src1.buffer == old_src1.buffer);
    assert(src1.capacity == old_src1.capacity);
    assert(src1.allocator == old_src1.allocator);

    assert(src2.len == old_src2.len);
    assert(src2.buffer == old_src2.buffer);
    assert(src2.capacity == old_src2.capacity);
    assert(src2.allocator == old_src2.allocator);

    assert(aws_byte_buf_is_valid(&dest));
    assert(aws_byte_buf_is_valid(&src1));
    assert(aws_byte_buf_is_valid(&src2));
}
