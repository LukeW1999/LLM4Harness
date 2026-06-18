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

    /* 2. Call function under test
     * aws_byte_buf_clean_up_secure:
     *   - calls aws_byte_buf_secure_zero: sets all bytes to 0, resets len to 0
     *   - calls aws_byte_buf_clean_up: frees the buffer memory, zeros out the struct fields
     * After the call:
     *   - buf.buffer is NULL (freed and zeroed)
     *   - buf.len == 0
     *   - buf.capacity == 0
     *   - buf.allocator == NULL (zeroed out by clean_up)
     */
    aws_byte_buf_clean_up_secure(&buf);

    /* 3. Assert postconditions
     * After aws_byte_buf_clean_up, the buffer fields are zeroed out:
     * - buffer pointer is NULL
     * - len is 0
     * - capacity is 0
     * - allocator is NULL
     */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 4. Assert validity invariant - after clean_up, the buf should still be valid
     * (a zeroed-out buf with NULL buffer, 0 len, 0 capacity is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
