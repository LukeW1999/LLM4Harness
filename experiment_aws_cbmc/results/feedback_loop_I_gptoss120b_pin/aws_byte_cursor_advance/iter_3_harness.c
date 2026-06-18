#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    struct aws_byte_cursor cursor;

    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    struct aws_byte_cursor old_cursor = cursor;

    size_t amount;
    __CPROVER_assume(amount <= MAX_BUFFER_SIZE);

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (amount <= old_cursor.len) {
        __CPROVER_assert(result == AWS_OP_SUCCESS, "result should be success");
        __CPROVER_assert(cursor.ptr == old_cursor.ptr + amount, "ptr should be advanced");
        __CPROVER_assert(cursor.len == old_cursor.len - amount, "len should be reduced");
    } else {
        __CPROVER_assert(result == AWS_OP_ERR, "result should be error");
        __CPROVER_assert(cursor.ptr == old_cursor.ptr, "ptr should be unchanged on error");
        __CPROVER_assert(cursor.len == old_cursor.len, "len should be unchanged on error");
    }

    __CPROVER_assert(aws_byte_cursor_is_valid(&cursor), "cursor should be valid after advance");
}
