#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness() {
    /* nondeterministic buffer */
    struct aws_byte_buf buf;

    /* bound the buffer's capacity and length */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));

    /* ensure the buffer's inner pointer is allocated (non-deterministically) */
    ensure_byte_buf_has_allocated_buffer_member(&buf);

    /* assume the buffer is valid */
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save the old buffer state to verify no modifications */
    struct aws_byte_buf old_buf = buf;

    /* call the function under test */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(&buf);

    /* 1. assert the returned cursor correctly mirrors the buffer */
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);

    /* 2. assert the buffer remains unchanged (const input) */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* 3. assert cursor validity */
    assert(aws_byte_cursor_is_valid(&cur));

    /* 4. assert buffer validity is preserved */
    assert(aws_byte_buf_is_valid(&buf));
}
