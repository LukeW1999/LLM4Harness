#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    char *c_str = malloc(MAX_BUFFER_SIZE);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(c_str[MAX_BUFFER_SIZE - 1] == '\0');

    struct aws_byte_cursor old_cursor = cursor;

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    (void)result;

    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);
    assert(aws_byte_cursor_is_valid(&cursor));

    free(c_str);
}
