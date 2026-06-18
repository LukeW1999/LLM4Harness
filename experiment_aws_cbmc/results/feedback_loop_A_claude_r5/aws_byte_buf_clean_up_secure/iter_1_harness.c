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
     * Analysis of postconditions:
     * 1. Changed fields on success:
     *    - aws_byte_buf_secure_zero zeros all bytes and sets len to 0
     *    - aws_byte_buf_clean_up frees the buffer and sets buffer to NULL,
     *      len to 0, capacity to 0, allocator to NULL
     * 2. Unchanged fields: none (all fields are reset)
     * 3. Failure: this function has no return value (void), always "succeeds"
     * 4. Validity: after clean_up, the buf should be in a zeroed/cleaned state
     *
     * After aws_byte_buf_clean_up, the buffer pointer is freed and set to NULL,
     * len is 0, capacity is 0, allocator is NULL.
     */

    /* 4. Assert postconditions */
    /* After clean_up, buffer should be NULL */
    assert(buf.buffer == NULL);
    /* After clean_up, len should be 0 */
    assert(buf.len == 0);
    /* After clean_up, capacity should be 0 */
    assert(buf.capacity == 0);
    /* After clean_up, allocator should be NULL */
    assert(buf.allocator == NULL);

    /* The resulting buf should be valid (a zeroed buf is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
