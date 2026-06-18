#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t len;

    struct aws_byte_cursor old_cursor = cursor;
    struct store_byte_from_buffer old_cursor_byte;
    if (old_cursor.len > 0) {
        save_byte_from_array(old_cursor.ptr, old_cursor.len, &old_cursor_byte);
    }

    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    if (len > old_cursor.len) {
        assert(rv.ptr == NULL);
        assert(rv.len == 0);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    } else {
        assert(rv.ptr == old_cursor.ptr);
        assert(rv.len == len);

        if (len == 0) {
            assert(cursor.ptr == old_cursor.ptr);
        } else {
            assert(cursor.ptr == old_cursor.ptr + len);
        }
        assert(cursor.len == old_cursor.len - len);

        if (len > 0) {
            assert_bytes_match(rv.ptr, old_cursor.ptr, len);
        }
    }

    if (old_cursor.len > 0) {
        assert_byte_from_buffer_matches(old_cursor.ptr, &old_cursor_byte);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&rv));
    assert(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    assert(aws_byte_cursor_is_bounded(&rv, MAX_BUFFER_SIZE));
}
