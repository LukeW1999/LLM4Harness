#include <assert.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_buf_reset_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *alloc = aws_default_allocator();

    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    struct aws_byte_buf old_buf = buf;

    __CPROVER_bool zero = nondet_bool();

    aws_byte_buf_reset(&buf, zero);

    /* Length must be zero after reset */
    assert(buf.len == 0);

    /* If zeroing was requested and a buffer existed, all bytes must be zero */
    if (zero && old_buf.buffer != NULL) {
        size_t i;
        for (i = 0; i < old_buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* Capacity, allocator, and buffer pointer must remain unchanged */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* The byte buffer must still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
