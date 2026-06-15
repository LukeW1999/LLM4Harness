#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_byte_cursor_eq_c_str_harness() {
    /* Non-deterministic inputs */
    struct aws_byte_cursor cursor;
    size_t str_len;
    char c_str[MAX_BUFFER_SIZE + 1];

    /* Bound cursor and ensure it is valid */
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* Bound the null-terminated string length */
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    /* Initialize c_str as a valid null-terminated string */
    for (size_t i = 0; i < str_len; i++) {
        c_str[i] = nondet_char();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[str_len] = '\0';

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call the function */
    bool rv = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Postcondition 1: cursor fields unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Postcondition 2: cursor remains valid */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* Postcondition 3: basic consistency for empty cursor */
    if (cursor.len == 0) {
        /* Only possible match is if c_str is also empty */
        assert(rv == (c_str[0] == '\0'));
    }
}
