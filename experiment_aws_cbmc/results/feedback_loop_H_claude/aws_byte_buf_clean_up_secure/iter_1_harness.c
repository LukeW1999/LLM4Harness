#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and set up the aws_byte_buf */
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep verification tractable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before calling */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Call the function under test
     * aws_byte_buf_clean_up_secure:
     *   - Calls aws_byte_buf_secure_zero (zeroes all bytes and sets len to 0)
     *   - Calls aws_byte_buf_clean_up (frees the buffer and resets all fields)
     * After the call, the buffer should be zeroed/cleaned up.
     * The function has no return value (void).
     */
    aws_byte_buf_clean_up_secure(&buf);

    /* 3. Assert postconditions
     * After aws_byte_buf_clean_up_secure:
     * - aws_byte_buf_clean_up sets: buf->buffer = NULL, buf->len = 0, buf->capacity = 0, buf->allocator = NULL
     * - The buffer memory has been freed and zeroed
     */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 4. Assert validity invariant holds after the call
     * A zeroed/cleaned buf with all NULL/0 fields should still be valid
     * (aws_byte_buf_is_valid allows NULL buffer when len==0 and capacity==0)
     */
    assert(aws_byte_buf_is_valid(&buf));
}
