#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    struct aws_byte_cursor old_cursor = cursor;
    
    size_t len;
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    assert(aws_byte_cursor_is_valid(&cursor));
    
    if (len <= old_cursor.len) {
        assert(result.ptr == old_cursor.ptr);
        assert(result.len == len);
        assert(cursor.ptr == old_cursor.ptr + len);
        assert(cursor.len == old_cursor.len - len);
    } else {
        assert(result.ptr == NULL);
        assert(result.len == 0);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }
}
