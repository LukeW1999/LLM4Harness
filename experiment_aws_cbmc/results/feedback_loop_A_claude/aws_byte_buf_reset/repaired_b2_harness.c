#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_buf_reset_harness(void) {
    /* Set up a byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state to verify what changes */
    struct aws_byte_buf buf_old = buf;

    /* Save a copy of the buffer contents before reset */
    uint8_t old_contents[MAX_BUFFER_SIZE];
    if (buf.buffer != NULL && buf.capacity > 0) {
        memcpy(old_contents, buf.buffer, buf.capacity);
    }

    /* Non-deterministic choice for zero_contents parameter */
    bool zero_contents;

    /* Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* After reset, len should be 0 */
    assert(buf.len == 0);

    /* capacity and buffer pointer should remain unchanged */
    assert(buf.capacity == buf_old.capacity);
    assert(buf.buffer == buf_old.buffer);
    assert(buf.allocator == buf_old.allocator);

    /* buf should still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* If zero_contents was true, the buffer contents should be zeroed */
    if (zero_contents && buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}
