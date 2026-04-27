#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_write_u8_harness(void) {
    /* 1. Declare and set up the buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Nondet byte value to write */
    uint8_t value;

    /* 3. Save old state */
    struct aws_byte_buf old_buf = buf;
    size_t old_len = buf.len;
    size_t old_capacity = buf.capacity;

    /* 4. Call function under test */
    bool result = aws_byte_buf_write_u8(&buf, value);

    /* 5. Assert postconditions */
    if (result) {
        /* On success, len increased by 1 */
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.len == old_len + 1);
        assert(buf.capacity == old_capacity);
        assert(buf.allocator == old_buf.allocator);
        /* The written byte matches value */
        assert(buf.buffer[old_len] == value);
    } else {
        /* On failure, buffer must be unchanged */
        assert(buf.len == old_len);
        assert(buf.capacity == old_capacity);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
        /* Must have been at capacity */
        assert(old_len == old_capacity);
    }

    /* 6. Buffer validity invariant still holds */
    assert(aws_byte_buf_is_valid(&buf));
}
