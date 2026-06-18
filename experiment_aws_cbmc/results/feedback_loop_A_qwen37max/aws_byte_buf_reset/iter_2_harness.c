#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_reset_harness() {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    bool zero_contents;

    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;
    uint8_t *old_buffer = buf.buffer;

    aws_byte_buf_reset(&buf, zero_contents);

    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);
    assert(buf.buffer == old_buffer);
}
