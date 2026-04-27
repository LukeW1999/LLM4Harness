#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_byte_buf_clean_up_secure
 *
 * aws_byte_buf_clean_up_secure:
 * 1. Calls aws_byte_buf_secure_zero: sets all bytes to zero and resets len to 0
 * 2. Calls aws_byte_buf_clean_up: frees the buffer memory and zeros all fields
 *
 * After the call:
 * - buf->buffer is NULL (freed and zeroed)
 * - buf->len is 0
 * - buf->capacity is 0
 * - buf->allocator is NULL
 */
void aws_byte_buf_clean_up_secure_harness() {
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* After clean_up_secure:
     * - aws_byte_buf_secure_zero zeros all bytes and sets len = 0
     * - aws_byte_buf_clean_up frees buffer and zeros all fields
     * So all fields should be zeroed/NULL after the call
     */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The resulting buf should be valid (a zeroed buf is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
