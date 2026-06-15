#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_secure_zero_harness() {
    struct aws_byte_buf buf;

    /* Preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    size_t old_capacity = buf.capacity;
    bool buffer_was_null = (buf.buffer == NULL);
    struct aws_byte_buf old_buf = buf;

    /* Call the function */
    aws_byte_buf_secure_zero(&buf);

    /* Postconditions */
    assert(aws_byte_buf_is_valid(&buf));
    assert(buf.len == 0);
    assert(buf.capacity == old_capacity);
    assert(buf.allocator == old_buf.allocator);
    if (!buffer_was_null) {
        assert(buf.buffer != NULL);
    }
}
