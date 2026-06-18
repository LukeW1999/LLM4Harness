#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

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
    size_t old_len = old_buf.len;

    /* Save a byte from the buffer for later content‑equality check */
    struct store_byte_from_buffer saved_byte;
    if (buf.buffer != NULL && buf.len > 0) {
        save_byte_from_array(buf.buffer, buf.len, &saved_byte);
    }

    /* Call the function under test */
    aws_byte_buf_secure_zero(&buf);

    /* Post‑condition checks */
    if (old_buf.buffer != NULL && old_len > 0) {
        /* The original buffer contents must be all zeroes */
        uint8_t *zero_buf = (uint8_t *)malloc(old_len);
        __CPROVER_assume(zero_buf != NULL);
        memset(zero_buf, 0, old_len);
        assert_bytes_match(old_buf.buffer, zero_buf, old_len);
        free(zero_buf);
    }

    /* Fields that must remain unchanged (except len) */
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(buf.buffer == old_buf.buffer);

    /* Length should be zero after secure zero */
    assert(buf.len == 0);

    /* Verify the saved byte is now zero if it existed */
    if (old_buf.buffer != NULL && old_len > 0) {
        assert_byte_from_buffer_is_zero(&saved_byte);
    }

    /* Validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
