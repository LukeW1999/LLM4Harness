#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[str_len] = '\0';

    struct aws_byte_cursor old_cursor = cursor;

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    
    if (result) {
        assert(cursor.len == strlen(c_str));
    }

    assert(aws_byte_cursor_is_valid(&cursor));
}
