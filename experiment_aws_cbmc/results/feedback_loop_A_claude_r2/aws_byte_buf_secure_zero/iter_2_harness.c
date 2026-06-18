#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* Set up buffer with bounded size */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;
    uint8_t *old_buffer = buf.buffer;

    /* Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* Assert postconditions */
    /* len, capacity, allocator, buffer pointer must not change */
    assert(buf.len == old_len);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);
    assert(buf.buffer == old_buffer);

    /* If buffer is non-null, all bytes should be zero */
    if (buf.buffer != NULL && buf.len > 0) {
        for (size_t i = 0; i < buf.len; i++) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* Buffer is still valid */
    assert(aws_byte_buf_is_valid(&buf));
}
