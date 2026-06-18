#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t amount;

    struct aws_byte_cursor old_cursor = cursor;

    bool result = aws_byte_cursor_advance(&cursor, amount);

    if (result) {
        assert((uintptr_t)cursor.ptr == (uintptr_t)old_cursor.ptr + (uintptr_t)amount);
        assert(cursor.len == old_cursor.len - amount);
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
