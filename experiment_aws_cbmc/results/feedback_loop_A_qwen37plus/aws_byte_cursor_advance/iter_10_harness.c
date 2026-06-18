#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <assert.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);

    size_t n;
    __CPROVER_assume(n <= cursor.len);

    struct aws_byte_cursor old_cursor = cursor;

    if (aws_byte_cursor_advance(&cursor, n)) {
        assert(cursor.ptr == old_cursor.ptr + n);
        assert(cursor.len == old_cursor.len - n);
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }
}
