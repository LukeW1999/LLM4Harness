#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* 1. Set up buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* 4. Assert postconditions */
    /* The buffer pointer, len, capacity, and allocator should remain the same */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* The buffer contents should be zeroed */
    if (buf.buffer != NULL && buf.len > 0) {
        for (size_t i = 0; i < buf.len; i++) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));
}
