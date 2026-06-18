#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and set up the byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state before calling */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /*
     * aws_byte_buf_clean_up_secure:
     * - Calls aws_byte_buf_secure_zero: sets all bytes to 0, resets len to 0
     * - Calls aws_byte_buf_clean_up: frees the buffer memory, sets buffer to NULL,
     *   resets len, capacity to 0, allocator to NULL
     *
     * After clean_up_secure:
     * - buf.len == 0
     * - buf.capacity == 0
     * - buf.buffer == NULL
     * - buf.allocator == NULL
     */

    /* 4. Assert postconditions */
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.buffer == NULL);
    assert(buf.allocator == NULL);

    /* 5. Assert validity invariant holds after the call */
    assert(aws_byte_buf_is_valid(&buf));
}
