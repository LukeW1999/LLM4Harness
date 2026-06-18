#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    struct aws_byte_cursor old_cursor = cursor;
    struct store_byte_from_buffer old_cursor_data;
    if (cursor.ptr != NULL && cursor.len > 0) {
        save_byte_from_array(cursor.ptr, cursor.len, &old_cursor_data);
    }

    size_t advance_len;
    __CPROVER_assume(advance_len <= cursor.len);

    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, advance_len);

    /* 1. original cursor must be advanced */
    assert(cursor.len == old_cursor.len - advance_len);
    assert(cursor.ptr == old_cursor.ptr + advance_len);
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 2. underlying data unchanged */
    if (old_cursor.ptr != NULL && old_cursor.len > 0) {
        assert_byte_from_buffer_matches(old_cursor.ptr, &old_cursor_data);
    }

    /* 3. result cursor points to advanced bytes */
    assert(result.ptr == old_cursor.ptr);
    assert(result.len == advance_len);
    assert(aws_byte_cursor_is_valid(&result));
}
