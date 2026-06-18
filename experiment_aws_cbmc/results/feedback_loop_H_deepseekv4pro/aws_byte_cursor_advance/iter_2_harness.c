#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    size_t len;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(len <= cursor.len);

    struct aws_byte_cursor old = cursor;
    struct aws_byte_cursor new_cursor = aws_byte_cursor_advance(&cursor, len);

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&new_cursor));
    if (len > 0) {
        assert(new_cursor.ptr == old.ptr);
        assert(new_cursor.len == len);
        assert(cursor.ptr == old.ptr + len);
        assert(cursor.len == old.len - len);
    } else {
        assert(new_cursor.ptr == old.ptr);
        assert(new_cursor.len == 0);
        assert(cursor.ptr == old.ptr);
        assert(cursor.len == old.len);
    }
}
