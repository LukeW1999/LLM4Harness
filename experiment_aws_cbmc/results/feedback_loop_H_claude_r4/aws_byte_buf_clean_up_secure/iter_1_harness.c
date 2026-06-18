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
     * Analysis of aws_byte_buf_clean_up_secure:
     * - Calls aws_byte_buf_secure_zero: sets all bytes to 0, resets len to 0
     * - Calls aws_byte_buf_clean_up: frees the buffer memory, sets buffer to NULL,
     *   sets len to 0, sets capacity to 0, allocator remains
     *
     * After clean_up_secure:
     * - buf.len == 0 (zeroed then cleaned up)
     * - buf.capacity == 0 (cleaned up)
     * - buf.buffer == NULL (cleaned up)
     * - buf.allocator == old.allocator (allocator not freed)
     */

    /* 4. Assert postconditions */
    /* After clean_up, buffer is NULL */
    assert(buf.buffer == NULL);
    /* After clean_up, len is 0 */
    assert(buf.len == 0);
    /* After clean_up, capacity is 0 */
    assert(buf.capacity == 0);
    /* Allocator is preserved */
    assert(buf.allocator == old.allocator);

    /* 5. Assert validity invariant holds after the call */
    assert(aws_byte_buf_is_valid(&buf));
}
