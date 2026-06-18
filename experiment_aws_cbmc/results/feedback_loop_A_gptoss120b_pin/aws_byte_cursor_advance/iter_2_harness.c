#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_byte_cursor_advance_harness() {
    /* Set up a bounded byte buffer */
    struct aws_byte_buf buf;
    __CPROVER_assume(aws_byte_buf_is_bounded(&buf, MAX_BUFFER_SIZE));
    __CPROVER_assume(buf.capacity <= MAX_BUFFER_SIZE);
    __CPROVER_assume(buf.len <= buf.capacity);
    ensure_byte_buf_has_allocated_buffer_member(&buf);
    __CPROVER_assume(aws_byte_buf_is_valid(&buf));

    /* Create a cursor from the buffer */
    struct aws_byte_cursor cursor = aws_byte_cursor_from_buf(&buf);

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;
    struct aws_byte_buf old_buf = buf;

    /* Nondeterministic advance amount */
    size_t advance;
    __CPROVER_assume(advance <= old_cursor.len);

    /* Call function under test */
    int result = aws_byte_cursor_advance(&cursor, advance);

    if (result == AWS_OP_SUCCESS) {
        /* Cursor should be advanced */
        assert(cursor.ptr == old_cursor.ptr + advance);
        assert(cursor.len == old_cursor.len - advance);
    } else {
        /* On failure cursor must be unchanged */
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    /* Buffer must remain unchanged */
    assert(buf.buffer == old_buf.buffer);
    assert(buf.len == old_buf.len);
    assert(buf.capacity == old_buf.capacity);
    assert(buf.allocator == old_buf.allocator);
    assert(aws_byte_buf_is_valid(&buf));

    /* Cursor validity invariant */
    assert(aws_byte_cursor_is_valid(&cursor));
}
