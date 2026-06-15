#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_from_buf_harness() {
    /* nondeterministic inputs */
    struct aws_byte_buf buf;
    const struct aws_byte_buf *buf_ptr = &buf;

    /* assume valid preconditions */
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* save old state for later comparison */
    struct aws_byte_buf old_buf = buf;
    struct store_byte_from_buffer old_byte;
    save_byte_from_array(buf.buffer, buf.len, &old_byte);

    /* call function under verification */
    struct aws_byte_cursor cur = aws_byte_cursor_from_buf(buf_ptr);

    /* postconditions */
    assert(cur.ptr == buf.buffer);
    assert(cur.len == buf.len);
    assert(aws_byte_cursor_is_valid(&cur));

    /* buf is unchanged */
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.buffer == old_buf.buffer);
    assert(buf.allocator == old_buf.allocator);

    /* memory contents unchanged */
    if (buf.len > 0) {
        assert_byte_from_buffer_matches(buf.buffer, &old_byte);
    }

    /* validity invariant */
    assert(aws_byte_buf_is_valid(&buf));
}
