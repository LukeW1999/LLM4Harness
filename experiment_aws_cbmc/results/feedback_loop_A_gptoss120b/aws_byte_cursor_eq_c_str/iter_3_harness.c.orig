#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

uint8_t nondet_uint8_t(void);
size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_byte_cursor_eq_c_str_harness(void) {
    struct aws_byte_cursor cursor;

    cursor.len = nondet_size_t();
    __CPROVER_assume(cursor.len <= MAX_BUFFER_SIZE);

    if (nondet_bool()) {
        cursor.ptr = malloc(cursor.len);
        __CPROVER_assume(cursor.ptr != NULL);
        for (size_t i = 0; i < cursor.len; ++i) {
            cursor.ptr[i] = (char)nondet_uint8_t();
        }
    } else {
        cursor.ptr = NULL;
        __CPROVER_assume(cursor.len == 0);
    }

    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    struct aws_byte_cursor old = cursor;

    size_t c_str_len = nondet_size_t();
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t j = 0; j < c_str_len; ++j) {
        c_str[j] = (char)nondet_uint8_t();
    }
    c_str[c_str_len] = '\0';

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    assert(cursor.ptr == old.ptr);
    assert(cursor.len == old.len);
    assert(result == aws_array_eq_c_str(old.ptr, old.len, c_str));
    assert(aws_byte_cursor_is_valid(&cursor));
}
