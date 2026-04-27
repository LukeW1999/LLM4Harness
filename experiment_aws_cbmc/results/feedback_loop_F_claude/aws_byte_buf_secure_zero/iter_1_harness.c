#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_byte_buf_secure_zero
 *
 * From the Doxygen:
 *   "Sets all bytes of buffer to zero and resets len to zero."
 *
 * Analysis:
 * 1. Changed fields on success:
 *    - buf->len is set to 0
 *    - buf->buffer contents are zeroed (if buffer != NULL)
 * 2. Unchanged fields:
 *    - buf->capacity remains the same
 *    - buf->allocator remains the same
 *    - buf->buffer pointer remains the same (only contents change)
 * 3. Failure: function is void, no failure path
 * 4. Validity invariants: aws_byte_buf_is_valid must hold after the call
 */
void aws_byte_buf_secure_zero_harness() {
    /* 1. Declare and bound the buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state before calling */
    struct aws_byte_buf old = buf;

    /* 3. Call function under test */
    aws_byte_buf_secure_zero(&buf);

    /* 4. Assert postconditions */

    /* len must be reset to 0 */
    assert(buf.len == 0);

    /* capacity must not change */
    assert(buf.capacity == old.capacity);

    /* allocator must not change */
    assert(buf.allocator == old.allocator);

    /* buffer pointer must not change (only contents are zeroed) */
    assert(buf.buffer == old.buffer);

    /* If buffer is non-NULL, all bytes up to capacity should be zero */
    if (buf.buffer != NULL && buf.capacity > 0) {
        /* Check that the buffer contents are zeroed */
        /* We check a representative byte to keep verification tractable */
        /* CBMC will verify this for all reachable paths */
        for (size_t i = 0; i < buf.capacity; i++) {
            assert(buf.buffer[i] == 0);
        }
    }

    /* 5. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
