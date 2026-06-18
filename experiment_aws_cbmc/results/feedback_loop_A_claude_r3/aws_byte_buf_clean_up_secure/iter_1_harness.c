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

    /* 2. Save old state */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /*
     * 4. Assert postconditions:
     *
     * aws_byte_buf_clean_up_secure calls:
     *   - aws_byte_buf_secure_zero: sets all bytes to 0 and resets len to 0
     *   - aws_byte_buf_clean_up: frees the buffer memory and zeros out the struct
     *
     * After aws_byte_buf_clean_up, the buffer pointer is freed and set to NULL,
     * len is 0, capacity is 0, and allocator is set to NULL.
     */

    /* After clean_up_secure, the struct should be zeroed out */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(buf.allocator == NULL);
}
