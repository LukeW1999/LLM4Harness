#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_buf_secure_zero_harness() {
    /* 1. Declare and bound data structures */
    struct aws_byte_buf byte_buf;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    uint8_t *buffer = (uint8_t *)malloc(len);
    __CPROVER_assume(buffer != NULL);
    byte_buf.buffer = buffer;
    byte_buf.len = len;
    byte_buf.capacity = len;
    byte_buf.allocator = aws_default_allocator();

    /* 2. Save old state BEFORE calling — needed for immutability checks */
    struct aws_byte_buf old_byte_buf = byte_buf;

    /* 3. Add precondition check */
    __CPROVER_assume(aws_byte_buf_is_valid(&byte_buf));

    /* 4. Call function under test */
    aws_byte_buf_secure_zero(&byte_buf);

    /* 5. Assert postconditions for BOTH success and failure paths */
    assert(byte_buf.len == old_byte_buf.len);
    assert(byte_buf.capacity == old_byte_buf.capacity);
    assert(byte_buf.allocator == old_byte_buf.allocator);
    for (size_t i = 0; i < byte_buf.len; i++) {
        assert(byte_buf.buffer[i] == 0);
    }

    /* 6. Assert fields that must NOT change regardless of result */
    assert(byte_buf.buffer == old_byte_buf.buffer);

    /* 7. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&byte_buf));

    free(buffer);
}
