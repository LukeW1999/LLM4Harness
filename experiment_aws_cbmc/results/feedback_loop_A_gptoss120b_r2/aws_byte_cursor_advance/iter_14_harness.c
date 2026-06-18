#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 1024

void aws_byte_cursor_advance_harness(void) {
    struct aws_allocator *allocator = aws_default_allocator();

    struct aws_byte_cursor cursor;
    ensure_byte_cursor_has_allocated_buffer_member(&cursor, allocator);
    __CPROVER_assume(cursor.len <= MAX_BUFFER_SIZE);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    struct aws_byte_cursor old_cursor = cursor;

    size_t amount = __CPROVER_nondet_size_t();

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(amount <= old_cursor.len, "amount <= old len on success");
        __CPROVER_assert(cursor.ptr == (uint8_t *)old_cursor.ptr + amount,
                         "cursor.ptr advanced correctly");
        __CPROVER_assert(cursor.len == old_cursor.len - amount,
                         "cursor.len reduced correctly");
    } else {
        __CPROVER_assert(result == AWS_OP_ERR, "result is error");
        __CPROVER_assert(cursor.ptr == old_cursor.ptr, "cursor.ptr unchanged on error");
        __CPROVER_assert(cursor.len == old_cursor.len, "cursor.len unchanged on error");
    }

    __CPROVER_assert(aws_byte_cursor_is_valid(&cursor), "cursor remains valid");
    __CPROVER_assert(aws_byte_cursor_is_valid(&old_cursor), "old_cursor remains valid");
}
