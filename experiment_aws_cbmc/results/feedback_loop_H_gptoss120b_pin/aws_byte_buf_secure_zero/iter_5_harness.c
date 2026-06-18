#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_byte_buf old_buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    old_buf = buf;

    aws_byte_buf_secure_zero(&buf);

    /* Verify that structural fields are unchanged */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.len == old_buf.len);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* Verify that the buffer contents are zeroed */
    if (buf.buffer != NULL && buf.len > 0) {
        for (size_t i = 0; i < buf.len; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
    assert(alloc == aws_default_allocator());
}
