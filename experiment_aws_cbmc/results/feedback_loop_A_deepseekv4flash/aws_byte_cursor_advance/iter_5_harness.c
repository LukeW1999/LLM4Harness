#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    size_t len;

    cursor.len = nondet_size_t();
    __CPROVER_assume(cursor.len <= 100);
    cursor.ptr = malloc(cursor.len);
    __CPROVER_assume(cursor.ptr != NULL);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    // Precondition: len must not exceed cursor.len
    __CPROVER_assume(len <= cursor.len);

    struct aws_byte_cursor old_cursor = cursor;

    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    size_t advance = len; // since len <= old_cursor.len, advance = len
    __CPROVER_assert(aws_byte_cursor_is_valid(&cursor), "cursor remains valid");
    __CPROVER_assert(cursor.ptr == old_cursor.ptr + advance, "cursor ptr advanced by advance");
    __CPROVER_assert(cursor.len == old_cursor.len - advance, "cursor len decreased by advance");
    __CPROVER_assert(aws_byte_cursor_is_valid(&result), "result is valid");
    __CPROVER_assert(result.ptr == old_cursor.ptr, "result ptr points to original position");
    __CPROVER_assert(result.len == advance, "result len is advance");
}
