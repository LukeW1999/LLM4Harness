#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

uint8_t nondet_uint8_t(void);
size_t nondet_size_t(void);

void aws_byte_cursor_eq_c_str_harness(void) {
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    if (cursor.ptr != NULL && cursor.len > 0) {
        size_t i;
        for (i = 0; i < cursor.len; ++i) {
            cursor.ptr[i] = (char)nondet_uint8_t();
        }
    }

    struct aws_byte_cursor old = cursor;

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    size_t j;
    for (j = 0; j < c_str_len; ++j) {
        c_str[j] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    assert(cursor.ptr == old.ptr);
    assert(cursor.len == old.len);
    assert(result == aws_array_eq_c_str(old.ptr, old.len, c_str));
    assert(aws_byte_cursor_is_valid(&cursor));
}
