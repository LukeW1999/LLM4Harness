#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

/**
 * aws_byte_buf_clean_up_secure:
 *   1. Calls aws_byte_buf_secure_zero (zeroes the buffer contents and sets len=0)
 *   2. Calls aws_byte_buf_clean_up (frees the buffer and resets all fields to 0/NULL)
 *
 * After the call:
 *   - buf->buffer == NULL
 *   - buf->len == 0
 *   - buf->capacity == 0
 *   - buf->allocator == NULL
 *
 * The function has no return value (void).
 */
void aws_byte_buf_clean_up_secure_harness(void) {
    /* 1. Declare and set up the byte buffer */
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep the state space manageable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the internal buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is in a valid state before the call */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state (for reference, though after clean_up_secure everything is zeroed) */
    struct aws_byte_buf old = buf;

    /* 3. Call the function under test */
    aws_byte_buf_clean_up_secure(&buf);

    /* 4. Assert postconditions:
     *    After aws_byte_buf_clean_up_secure, all fields should be zeroed/NULL
     *    because aws_byte_buf_clean_up sets buffer=NULL, len=0, capacity=0, allocator=NULL
     */
    assert(buf.buffer == NULL);
    assert(buf.len == 0);
    assert(buf.capacity == 0);
    assert(buf.allocator == NULL);

    /* 5. Assert validity invariant — a zeroed buf is still valid */
    assert(aws_byte_buf_is_valid(&buf));
}
