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
    uint8_t *old_buffer = buf.buffer;

    /* Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* Postconditions */
    /* Buffer pointer, capacity, allocator should be unchanged */
    assert(buf.buffer == old_buffer);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_allocator);

    /* len should be set to 0 */
    assert(buf.len == 0);

    /* All bytes in the buffer should be zero */
    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* Buffer should still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
