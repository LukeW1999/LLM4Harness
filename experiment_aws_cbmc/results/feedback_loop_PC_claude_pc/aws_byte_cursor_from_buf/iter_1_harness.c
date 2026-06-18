#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* 1. Declare and set up the input aws_byte_buf */
    struct aws_byte_buf buf;

    /* Bound the buffer size to keep the state space manageable */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the backing buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state BEFORE calling */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);

    /* 4. Assert postconditions */

    /* RETURN / OUTPUTS:
       The function returns a cursor where:
       - cur.ptr == buf->buffer
       - cur.len == buf->len
    */
    assert(result.ptr == buf.buffer);
    assert(result.len == buf.len);

    /* FRAME (inputs that must not change):
       The input buf must not be modified by this function.
    */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* INVARIANTS:
       The input buffer must still be valid after the call.
       The returned cursor must be valid.
    */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&result));

    /* Additional consistency checks:
       The cursor's ptr and len must be consistent with the buffer's buffer and len.
    */
    assert(result.ptr == old_buf.buffer);
    assert(result.len == old_buf.len);
}
