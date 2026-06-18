#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* 1. Declare and set up the input aws_byte_buf */
    struct aws_byte_buf buf;

    /* Bound the buffer size to prevent state space explosion */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* Allocate the buffer member non-deterministically */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* Assume the buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state before calling */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call the function under test */
    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);

    /* 4. Assert postconditions */

    /* The returned cursor's ptr should equal buf->buffer */
    assert(result.ptr == buf.buffer);

    /* The returned cursor's len should equal buf->len */
    assert(result.len == buf.len);

    /* The input buf should be unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 5. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&result));
}
