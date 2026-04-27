#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

/**
 * Harness for aws_byte_buf_reset
 *
 * From Doxygen:
 * "Resets the len of the buffer to 0, but does not free the memory. The buffer can then be reused.
 *  Optionally zeroes the contents, if the "zero_contents" flag is true."
 *
 * Analysis:
 * 1. Changed fields on success: buf->len is set to 0
 * 2. Unchanged fields: buf->capacity, buf->allocator, buf->buffer (pointer itself)
 * 3. Failure: function returns void, no failure path
 * 4. Validity invariants: aws_byte_buf_is_valid must hold after the call
 */
void aws_byte_buf_reset_harness() {
    /* 1. Declare and set up the buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state before calling */
    struct aws_byte_buf old = buf;

    /* 3. Non-deterministic zero_contents flag */
    bool zero_contents = nondet_bool();

    /* 4. Call function under test */
    aws_byte_buf_reset(&buf, zero_contents);

    /* 5. Assert postconditions */

    /* len must be reset to 0 */
    assert(buf.len == 0);

    /* capacity must not change */
    assert(buf.capacity == old.capacity);

    /* allocator must not change */
    assert(buf.allocator == old.allocator);

    /* buffer pointer must not change */
    assert(buf.buffer == old.buffer);

    /* 6. Assert validity invariant always holds */
    assert(aws_byte_buf_is_valid(&buf));
}
