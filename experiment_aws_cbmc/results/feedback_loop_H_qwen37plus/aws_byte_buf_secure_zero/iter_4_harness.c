#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    buf.allocator = aws_default_allocator();
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    aws_byte_buf_secure_zero(&buf);

    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);
    if (old_len > 0) {
        assert_bytes_are_zero(buf.buffer, old_len);
    }
}
