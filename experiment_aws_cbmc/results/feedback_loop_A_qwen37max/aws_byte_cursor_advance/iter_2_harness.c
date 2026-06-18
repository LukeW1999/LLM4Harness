#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>

void aws_byte_cursor_advance_harness() {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    if (cursor.len > 0) {
        cursor.ptr = malloc(cursor.len);
        __CPROVER_assume(cursor.ptr != NULL);
    } else {
        cursor.ptr = NULL;
    }
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t len;

    struct aws_byte_cursor result = aws_byte_cursor_advance(&cursor, len);

    assert(aws_byte_cursor_is_valid(&cursor));
    assert(aws_byte_cursor_is_valid(&result));
}
