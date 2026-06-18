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

    /* Preserve original contents for later zero‑check */
    uint8_t *old_content = NULL;
    if (buf.buffer != NULL && buf.capacity > 0) {
        old_content = malloc(buf.capacity);
        __CPROVER_assume(old_content != NULL);
        memcpy(old_content, buf.buffer, buf.capacity);
    }

    aws_byte_buf_secure_zero(&buf);

    /* Invariants that must be preserved */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    /* The function zeroes the buffer and resets length */
    assert(buf.len == 0);

    /* Verify that the entire buffer has been zeroed */
    if (buf.buffer != NULL && old_buf.capacity > 0) {
        for (size_t i = 0; i < old_buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));

    if (old_content != NULL) {
        free(old_content);
    }
}
