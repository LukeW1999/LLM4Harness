#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    size_t len;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    __CPROVER_assume(len <= cursor.len);

    struct aws_byte_cursor old_cursor = cursor;

    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    /* Postconditions on the returned cursor */
    assert(aws_byte_cursor_is_valid(&result));
    assert(result.ptr == old_cursor.ptr);
    assert(result.len == len);

    /* Cursor has been advanced */
    assert(cursor.ptr == old_cursor.ptr + len);
    assert(cursor.len == old_cursor.len - len);
    assert(aws_byte_cursor_is_valid(&cursor));
}
