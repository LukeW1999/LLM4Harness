#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness() {
    /* parameters */
    struct aws_byte_buf buf;

    /* assumptions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state */
    struct aws_byte_buf old_buf = buf;

    /* operation under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* assertions */
    assert(aws_byte_cursor_is_valid(&cur));
    assert(cur.ptr == old_buf.buffer);
    assert(cur.len == old_buf.len);
    assert(cur.ptr == buf.buffer);  // Ensure the pointer is directly taken from buf
    assert(cur.len == buf.len);     // Ensure the length is directly taken from buf

    /* unchanged fields */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);

    /* validity invariants */
    assert(aws_byte_buf_is_valid(&buf));
    assert(aws_byte_cursor_is_valid(&cur));
}
