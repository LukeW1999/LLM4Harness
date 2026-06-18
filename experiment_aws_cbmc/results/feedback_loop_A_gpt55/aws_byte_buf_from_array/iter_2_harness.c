#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_from_array_harness() {
    struct aws_byte_buf src;
    __CPROVER_assume(aws_byte_buf_is_bounded(&src, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&src);
    __CPROVER_assume(aws_byte_buf_is_valid(&src));

    struct aws_byte_buf old_src = src;
    struct store_byte_from_buffer src_byte;
    save_byte_from_array(src.buffer, src.capacity, &src_byte);

    struct aws_byte_buf result = aws_byte_buf_from_array(src.buffer, src.len);

    assert(result.allocator == NULL);
    assert(result.buffer == old_src.buffer);
    assert(result.len == old_src.len);
    assert(result.capacity == old_src.len);

    assert(src.buffer == old_src.buffer);
    assert(src.len == old_src.len);
    assert(src.capacity == old_src.capacity);
    assert(src.allocator == old_src.allocator);
    assert_byte_from_buffer_matches(src.buffer, &src_byte);

    assert(aws_byte_buf_is_valid(&src));
    assert(aws_byte_buf_is_valid(&result));
}
