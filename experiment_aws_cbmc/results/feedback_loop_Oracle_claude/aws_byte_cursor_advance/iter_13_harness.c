#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_advance_harness(void) {
    struct aws_byte_cursor cursor;
    __CPROVER_havoc_object(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t original_len = cursor.len;
    uint8_t *original_ptr = cursor.ptr;

    size_t len;

    struct aws_byte_cursor rv = aws_byte_cursor_advance(&cursor, len);

    __CPROVER_assert(aws_byte_cursor_is_valid(&rv), "rv is valid");
    __CPROVER_assert(aws_byte_cursor_is_valid(&cursor), "cursor is valid after call");

    if (len > original_len) {
        __CPROVER_assert(rv.ptr == NULL, "rv.ptr is NULL on failure");
        __CPROVER_assert(rv.len == 0, "rv.len is 0 on failure");
        __CPROVER_assert(cursor.ptr == original_ptr, "cursor.ptr unchanged on failure");
        __CPROVER_assert(cursor.len == original_len, "cursor.len unchanged on failure");
    } else {
        __CPROVER_assert(rv.ptr == original_ptr, "rv.ptr is original_ptr on success");
        __CPROVER_assert(rv.len == len, "rv.len equals len on success");
        if (original_ptr != NULL) {
            __CPROVER_assert(cursor.ptr == original_ptr + len, "cursor.ptr advanced by len");
        }
        __CPROVER_assert(cursor.len == original_len - len, "cursor.len decreased by len");
    }
}
