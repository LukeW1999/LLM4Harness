#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 1024
#endif

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    aws_byte_buf_secure_zero(&buf);

    /* Verify structural fields that must remain unchanged */
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* The function zeroes the buffer and resets length */
    assert(buf.len == 0);

    /* Verify the buffer contents are zeroed */
    if (buf.buffer != NULL) {
        size_t i;
        for (i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    assert(aws_byte_buf_is_valid(&buf));
}
