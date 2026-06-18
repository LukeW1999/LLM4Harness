#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t amount;
    __CPROVER_assume(amount <= cursor.len);

    struct aws_byte_cursor old_cursor = cursor;

    bool result = aws_byte_cursor_advance(&cursor, amount);

    assert(result);
    assert(cursor.ptr == old_cursor.ptr + amount);
    assert(cursor.len == old_cursor.len - amount);
    assert(aws_byte_cursor_is_valid(&cursor));
}
