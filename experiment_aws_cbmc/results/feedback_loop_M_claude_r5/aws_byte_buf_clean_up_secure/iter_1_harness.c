#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
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

    /*
     * aws_byte_buf_clean_up_secure:
     * - Calls aws_byte_buf_secure_zero: zeros all bytes and sets len to 0
     * - Calls aws_byte_buf_clean_up: frees the buffer and resets all fields
     *
     * After clean_up_secure, the buffer should be zeroed out:
     * - buf.len == 0
     * - buf.buffer == NULL
     * - buf.capacity == 0
     * - buf.allocator == NULL
     */

    /* 3. Assert postconditions */
    /* After clean_up, all fields should be zeroed/nulled */
    assert(buf.len == 0);
    assert(buf.buffer == NULL);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* The resulting buffer should be valid (a zeroed buffer is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
