#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t len;

    struct aws_byte_cursor old_cursor = cursor;

    struct store_byte_from_buffer old_byte;
    if (old_cursor.len > 0) {
        save_byte_from_array(old_cursor.ptr, old_cursor.len, &old_byte);
    }

    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    if (len <= old_cursor.len) {
        assert(result.ptr == old_cursor.ptr);
        assert(result.len == len);
        assert(cursor.len == old_cursor.len - len);

        if (len == 0) {
            assert(cursor.ptr == old_cursor.ptr);
        } else {
            assert(cursor.ptr == old_cursor.ptr + len);
        }

        if (len > 0) {
            assert_bytes_match(result.ptr, old_cursor.ptr, len);
        }
    } else {
        assert(result.ptr == NULL);
        assert(result.len == 0);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    if (old_cursor.len > 0) {
        assert_byte_from_buffer_matches(old_cursor.ptr, &old_byte);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&result));
}
