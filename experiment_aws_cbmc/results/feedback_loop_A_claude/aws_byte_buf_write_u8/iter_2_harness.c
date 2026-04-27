#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/common.h>
#include <string.h>

void aws_byte_buf_write_u8_harness(void) {
    /* 1. Set up the byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &old_byte);
    }

    /* 3. Nondet value to write */
    uint8_t value;

    /* 4. Call function under test */
    bool result = aws_byte_buf_write_u8(&buf, value);

    /* 5. Assert postconditions */
    if (result) {
        /* Success: one byte was written, len increased by 1 */
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.len == old_buf.len + 1);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
        /* The written byte should be value */
        assert(buf.buffer[old_buf.len] == value);
    } else {
        /* Failure: buf is unchanged */
        assert(aws_byte_buf_is_valid(&buf));
        assert(buf.len == old_buf.len);
        assert(buf.capacity == old_buf.capacity);
        assert(buf.allocator == old_buf.allocator);
        assert(buf.buffer == old_buf.buffer);
        /* Verify existing bytes are unchanged */
        if (buf.buffer != NULL && buf.len > 0) {
            assert_byte_from_buffer_matches(buf.buffer, &old_byte);
        }
    }

    /* 6. buf validity invariant holds */
    assert(aws_byte_buf_is_valid(&buf));
}
