#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdlib.h>

#ifndef MAX_BUFFER_SIZE
#define MAX_BUFFER_SIZE 100
#endif

void aws_byte_cursor_eq_c_str_harness() {
    struct aws_byte_cursor cursor;
    size_t cursor_len;
    __CPROVER_assume(cursor_len <= MAX_BUFFER_SIZE);
    uint8_t *buf = (uint8_t *)malloc(cursor_len);
    __CPROVER_assume(buf != NULL);
    cursor.ptr = buf;
    cursor.len = cursor_len;

    size_t c_str_len;
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    c_str[c_str_len] = '\0'; /* ensure null terminator */

    struct aws_byte_cursor old_cursor = cursor;

    /* Branch on equality of lengths to exercise different code paths */
    if (nondet_bool()) {
        __CPROVER_assume(c_str_len == cursor.len);
    } else {
        __CPROVER_assume(c_str_len != cursor.len);
    }

    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Postconditions: cursor is unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);
    assert(aws_byte_cursor_is_valid(&cursor));

    /* If lengths differ, result must be false */
    if (c_str_len != cursor.len) {
        assert(!result);
    }

    free(buf);
    free(c_str);
}
