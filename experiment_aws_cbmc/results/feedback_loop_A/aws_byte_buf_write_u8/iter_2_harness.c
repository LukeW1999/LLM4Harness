#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_write_u8_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    uint8_t value;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(buf.capacity > buf.len); // Ensure there's space to write

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    size_t old_len = buf.len;

    /* 3. Call function under test */
    int result = aws_byte_buf_write_u8(&buf, value);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == old_len + 1);
        assert(buf.buffer[old_len] == value);
    } else {
        assert(buf.len == old_len); // Length should not change on failure
    }

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
