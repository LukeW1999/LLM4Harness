#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <string.h>

/* Harness for aws_byte_buf_secure_zero */
void aws_byte_buf_secure_zero_harness(void) {
    struct aws_byte_buf buf;
    struct aws_allocator *allocator = aws_default_allocator();

    /* Bound the buffer */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the internal buffer if needed */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state for immutability checks */
    struct aws_byte_buf old_buf = buf;

    /* Save a byte from the buffer for later content‑equality check */
    struct store_byte_from_buffer saved_byte;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &saved_byte);
    }

    /* Call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* Post‑condition checks */
    if (buf.buffer != NULL && buf.len > 0) {
        /* The buffer contents must be all zeroes */
        uint8_t *zero_buf = (uint8_t *)malloc(buf.len);
        __CPROVER_assume(zero_buf != NULL);
        memset(zero_buf, 0, buf.len);
        assert_bytes_match(buf.buffer, zero_buf, buf.len);
        free(zero_buf);
    }

    /* Fields that must remain unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer || (buf.buffer != NULL && old_buf.buffer != NULL));

    /* Verify the saved byte is now zero if it existed */
    if (buf.buffer != NULL && buf.len > 0) {
        assert_byte_from_buffer_is_zero(&saved_byte);
    }

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
