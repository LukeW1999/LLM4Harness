#include <aws/common/byte_buf.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old = buf;

    aws_byte_buf_secure_zero(&buf);

    /* buffer pointer, capacity and allocator must remain unchanged */
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* length is set to zero by the function */
    assert(buf.len == 0);

    /* the resulting byte buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* all bytes up to the original capacity must be zeroed */
    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            __CPROVER_assert(buf.buffer[i] == 0, "buffer contents must be zeroed");
        }
    }
}
