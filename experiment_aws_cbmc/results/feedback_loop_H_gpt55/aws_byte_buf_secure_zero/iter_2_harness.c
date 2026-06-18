#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_secure_zero(&buf);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.len == 0);

    if (old_buf.buffer != NULL && old_buf.capacity > 0) {
        size_t index;
        __CPROVER_assume(index < old_buf.capacity);
        assert(buf.buffer[index] == 0);
    }

    assert(aws_byte_buf_is_valid(&buf));
}
