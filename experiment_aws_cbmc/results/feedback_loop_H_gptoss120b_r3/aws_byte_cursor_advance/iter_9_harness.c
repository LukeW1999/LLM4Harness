#define MAX_BUFFER_SIZE 1024

#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    struct aws_byte_cursor old_cursor = cursor;

    struct store_byte_from_buffer cursor_byte;
    if (cursor.ptr != NULL && cursor.len > 0) {
        save_byte_from_array((const uint8_t *)cursor.ptr, cursor.len, &cursor_byte);
    }

    size_t amount;
    __CPROVER_assume(amount <= SIZE_MAX);

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        if (old_cursor.ptr != NULL) {
            assert(cursor.ptr == old_cursor.ptr + amount);
        } else {
            assert(cursor.ptr == NULL);
        }
        assert(cursor.len == old_cursor.len - amount);

        if (cursor.ptr != NULL && cursor.len > 0 && cursor_byte.index >= amount) {
            assert_byte_from_buffer_matches(
                cursor.ptr + (cursor_byte.index - amount),
                &cursor_byte);
        }
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
