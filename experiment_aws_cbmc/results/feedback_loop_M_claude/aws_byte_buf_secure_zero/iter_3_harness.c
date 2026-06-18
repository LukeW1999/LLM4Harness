#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* 1. Set up buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save original capacity */
    size_t old_capacity = buf.capacity;

    /* 2. Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* 3. Assert postconditions */
    /* Buffer should still be valid */
    assert(aws_byte_buf_is_valid(&buf));

    /* capacity should be unchanged */
    assert(buf.capacity == old_capacity);

    /* len should be reset to 0 */
    assert(buf.len == 0);

    /* If buffer is non-null, all bytes should be zero */
    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }
}
