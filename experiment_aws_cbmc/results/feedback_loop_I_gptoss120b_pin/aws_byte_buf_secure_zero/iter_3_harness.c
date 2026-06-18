#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    aws_byte_buf_secure_zero(&buf);

    /* Length is expected to be zero after secure zero */
    assert(buf.len == 0);

    /* Other fields must remain unchanged */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* Buffer contents must be zeroed for the original length */
    if (old_buf.buffer != NULL && old_buf.len > 0) {
        for (size_t i = 0; i < old_buf.len; ++i) {
            assert(old_buf.buffer[i] == 0);
        }
    }

    /* The buffer must still satisfy its validity predicate */
    assert(aws_byte_buf_is_valid(&buf));
}
