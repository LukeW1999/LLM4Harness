#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_secure_zero(&buf);

    /* Fields that must remain unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Length is expected to be zero after secure zero */
    assert(buf.len == 0);

    /* Buffer contents must be zeroed up to capacity */
    if (buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
}
