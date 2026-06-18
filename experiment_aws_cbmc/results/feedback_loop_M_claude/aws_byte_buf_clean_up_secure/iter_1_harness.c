#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and set up the byte buffer */
    struct aws_byte_buf buf;

    /* Bound the buffer size to prevent state space explosion */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before calling */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 3. Assert postconditions:
     * aws_byte_buf_clean_up_secure calls aws_byte_buf_secure_zero (zeros memory and resets len to 0)
     * then aws_byte_buf_clean_up (frees buffer and resets all fields to 0/NULL).
     * After clean_up, the buffer should be zeroed out:
     * - buf.len == 0
     * - buf.buffer == NULL
     * - buf.capacity == 0
     * - buf.allocator == NULL
     */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);
}
