#include <assert.h>
#include <stddef.h>
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

    size_t amount = (size_t)__CPROVER_nondet_unsigned_long_long();

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(amount <= old_cursor.len, "amount <= old len on success");
        assert(cursor.ptr == (uint8_t *)old_cursor.ptr + amount);
        assert(cursor.len == old_cursor.len - amount);
    } else {
        assert(result == AWS_OP_ERR);
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&old_cursor));
}
