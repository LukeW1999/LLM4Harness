#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_CURSOR_LEN 256

void aws_byte_cursor_advance_harness(void) {
    struct aws_byte_cursor cursor;
    size_t amount = nondet_size_t();

    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_CURSOR_LEN));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    __CPROVER_assume(amount <= MAX_CURSOR_LEN);

    bool succeed = nondet_bool();
    if (succeed) {
        __CPROVER_assume(amount <= cursor.len);
    } else {
        __CPROVER_assume(amount > cursor.len);
    }

    struct aws_byte_cursor old_cursor = cursor;

    int result = aws_byte_cursor_advance(&cursor, amount);

    if (result == AWS_OP_SUCCESS) {
        assert(cursor.len == old_cursor.len - amount);
        assert(cursor.ptr == (const uint8_t *)old_cursor.ptr + amount);
    } else {
        assert(cursor.ptr == old_cursor.ptr);
        assert(cursor.len == old_cursor.len);
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
