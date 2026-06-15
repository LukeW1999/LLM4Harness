#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness() {
    /* Non-deterministic aws_byte_buf */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Save old state of buf to verify it is unchanged */
    struct aws_byte_buf old_buf = buf;

    /* Call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* Assert that buf is unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* Assert that the returned cursor has the correct values */
    assert(cur.len == buf.len);
    assert(cur.ptr == buf.buffer);

    /* Assert that the returned cursor is valid */
    assert(aws_byte_cursor_is_valid(&cur));

    /* Assert that the input buffer remains valid */
    assert(aws_byte_buf_is_valid(&buf));
}
