#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_buf_secure_zero_harness(void) {
    /* Set up buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save state before call */
    size_t old_capacity = buf.capacity;
    struct aws_allocator *old_allocator = buf.allocator;

    /* Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* After secure zero:
     * - len should be reset to 0
     * - capacity, allocator should be unchanged
     * - buffer contents should be zeroed (if buffer is non-null)
     */
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);

    /* If buffer is non-null, all bytes should be zero */
    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* Buffer should still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
