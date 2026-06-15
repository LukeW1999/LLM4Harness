#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_buf_clean_up_secure_harness() {
    /* Non-deterministic byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state */
    struct aws_byte_buf old = buf;

    /* Call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* Postconditions:
     * - aws_byte_buf_secure_zero: sets all bytes to zero and resets len to zero.
     * - aws_byte_buf_clean_up: frees buffer and sets buffer to NULL, len and capacity to 0.
     * Combined: buffer is freed, len=0, capacity=0, buffer=NULL.
     */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    /* allocator is unchanged (clean_up does not modify it) */
    assert(buf.allocator == old.allocator);
    /* Validity: after clean_up, buffer is still valid (len=0, capacity=0, buffer=NULL is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
