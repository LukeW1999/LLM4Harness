#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;
    uint8_t *old_content = NULL;

    if (buf.buffer != NULL && buf.len > 0) {
        old_content = malloc(buf.len);
        __CPROVER_assume(old_content != NULL);
        memcpy(old_content, buf.buffer, buf.len);
    }

    aws_byte_buf_secure_zero(&buf);

    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    if (buf.buffer != NULL && buf.len > 0) {
        for (size_t i = 0; i < buf.len; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));

    if (old_content != NULL) {
        free(old_content);
    }
}
