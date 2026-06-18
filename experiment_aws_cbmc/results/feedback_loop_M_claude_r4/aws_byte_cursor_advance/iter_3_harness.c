#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness(void) {
    struct aws_byte_cursor cursor;
    size_t len;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    struct aws_byte_cursor old_cursor = cursor;

    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    if (result.ptr != NULL) {
        assert(aws_byte_cursor_is_valid(&result));
        assert(aws_byte_cursor_is_valid(&cursor));
        assert(result.len == len);
        assert(cursor.len == old_cursor.len - len);
        assert(cursor.ptr == old_cursor.ptr + len);
    } else {
        assert(cursor.len == old_cursor.len);
        assert(cursor.ptr == old_cursor.ptr);
    }
}
