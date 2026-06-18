#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Constrain the byte buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Preserve the original state */
    struct aws_byte_buf old = buf;
    size_t old_len = old.len;

    /* Call the function under verification */
    aws_byte_buf_secure_zero(&buf);

    /* Structural fields that must remain unchanged */
    assert(buf.buffer == old.buffer);
    assert(buf.capacity == old.capacity);
    assert(buf.allocator == old.allocator);

    /* Length may be reduced (e.g., set to 0) but must never increase */
    assert(buf.len <= old_len);

    /* All bytes that were part of the original logical length must be zeroed */
    if (buf.buffer != NULL) {
        for (size_t i = 0; i < old_len; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* The resulting buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
