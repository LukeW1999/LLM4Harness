#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_write_u8_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf buf;
    uint8_t value = (uint8_t)nondet_char();
    size_t initial_len;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));
    __CPROVER_assume(buf.len <= buf.capacity);
    initial_len = buf.len;

    /* Add assumption to ensure there is enough capacity */
    __CPROVER_assume(buf.capacity > initial_len);

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    int result = aws_byte_buf_write_u8(&buf, value);

    /* 4. Assert postconditions for BOTH success and failure paths */
    if (result == AWS_OP_SUCCESS) {
        assert(buf.len == initial_len + 1);
        assert(buf.capacity >= initial_len + 1);
        assert(buf.buffer[initial_len] == value);
        assert(AWS_MEM_IS_WRITABLE(buf.buffer, buf.capacity));
    } else {
        assert(buf.len == initial_len);
        assert(buf.capacity >= initial_len);
    }

    /* 5. Assert fields that must NOT change regardless of result */
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
