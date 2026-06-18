#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/*
 * Harness for aws_byte_buf_clean_up_secure.
 *
 * Implementation:
 *   aws_byte_buf_clean_up_secure(struct aws_byte_buf *buf) {
 *       aws_byte_buf_secure_zero(buf);
 *       aws_byte_buf_clean_up(buf);
 *   }
 *
 * aws_byte_buf_secure_zero: sets all bytes to zero and resets len to zero.
 * aws_byte_buf_clean_up: frees the buffer memory and zeros all fields.
 *
 * After aws_byte_buf_clean_up_secure:
 *   - buf->buffer is NULL (freed and zeroed)
 *   - buf->len == 0
 *   - buf->capacity == 0
 *   - buf->allocator == NULL
 *
 * Preconditions:
 *   - buf must be a valid aws_byte_buf
 */

void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and set up the buffer */
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep the state space manageable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the backing buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state before the call */
    struct aws_byte_buf old = buf;

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Assert postconditions */

    /* After clean_up_secure:
     * - aws_byte_buf_secure_zero zeros all bytes and sets len = 0
     * - aws_byte_buf_clean_up frees the buffer and zeros all fields:
     *   buffer = NULL, len = 0, capacity = 0, allocator = NULL
     */

    /* buffer pointer must be NULL after cleanup */
    assert(buf.buffer == NULL);

    /* len must be 0 after cleanup */
    assert(buf.len == 0);

    /* capacity must be 0 after cleanup */
    assert(buf.capacity == 0);

    /* allocator must be NULL after cleanup */
    assert(buf.allocator == NULL);

    /* 5. Validity invariant: the resulting buf should be valid
     * (a zeroed-out buf with NULL buffer, 0 len, 0 capacity is valid) */
    assert(aws_byte_buf_is_valid(&buf));
}
