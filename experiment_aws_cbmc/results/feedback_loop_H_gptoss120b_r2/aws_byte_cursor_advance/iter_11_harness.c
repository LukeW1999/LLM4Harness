#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

#define MAX_BUFFER_SIZE 256

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;

    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));
    __CPROVER_assume(cursor.len <= MAX_BUFFER_SIZE);

    size_t amount;
    __CPROVER_assume(amount <= cursor.len);

    struct aws_byte_cursor old = cursor;

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.ptr == old.ptr + amount);
        assert(cursor.len == old.len - amount);
    } else {
        assert(cursor.ptr == old.ptr);
        assert(cursor.len == old.len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
