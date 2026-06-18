#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_from_buf_harness(void) {
    /* 1. Declare and set up the input aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* 2. Save old state */
    struct aws_byte_buf old_buf = buf;

    /* 3. Call function under test - returns struct by value */
    struct aws_byte_cursor result = aws_byte_cursor_from_buf(&buf);

    /* 4. Assert postconditions:
     *    The function sets cur.ptr = buf->buffer and cur.len = buf->len
     */
    assert(result.ptr == buf.buffer);
    assert(result.len == buf.len);

    /* 5. Assert the input buf is unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* 6. Assert validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&result));
}
