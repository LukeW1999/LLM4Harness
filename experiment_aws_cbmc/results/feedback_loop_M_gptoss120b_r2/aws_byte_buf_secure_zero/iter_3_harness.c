#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;

    /* Allocate a buffer for the byte_buf and bound its size */
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save the original state of the byte_buf */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* The byte_buf should retain its structural fields */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* After secure zero, length must be set to zero */
    assert(buf.len == 0);

    /* All bytes up to capacity must be zeroed */
    if (buf.buffer != NULL && buf.capacity > 0) {
        for (size_t i = 0; i < buf.capacity; ++i) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* The byte_buf must still be valid */
    assert(aws_byte_buf_is_valid(&buf));
}
