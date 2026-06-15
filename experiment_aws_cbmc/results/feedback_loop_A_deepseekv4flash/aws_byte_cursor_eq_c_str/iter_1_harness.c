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

    /* Bound the null-terminated string */
    __CPROVER_assume(str_len < MAX_BUFFER_SIZE);
    /* Ensure c_str is properly null-terminated and readable */
    c_str[str_len] = '\0';
    /* Assume no internal null bytes (proper C string) */
    for (size_t i = 0; i < str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, str_len + 1));

    /* Save old state */
    struct aws_byte_cursor old_cursor = cursor;

    /* Call the function */
    bool rv = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* Postcondition 1: cursor fields unchanged */
    assert(cursor.len == old_cursor.len);
    assert(cursor.ptr == old_cursor.ptr);

    /* Postcondition 2: return value matches specification */
    bool expected;
    if (cursor.len == 0) {
        /* Empty cursor can only match an empty string */
        expected = (c_str[0] == '\0');
    } else {
        /* Cursor must not contain a null byte to be eligible for true comparison */
        bool cursor_has_null = false;
        for (size_t i = 0; i < cursor.len; i++) {
            if (cursor.ptr[i] == '\0') {
                cursor_has_null = true;
                break;
            }
        }
        if (cursor_has_null) {
            expected = false;
        } else {
            /* Check exact match up to cursor length, and null terminator at cursor.len */
            bool match = true;
            if (cursor.len > str_len) {
                match = false; /* c_str too short */
            } else {
                for (size_t i = 0; i < cursor.len; i++) {
                    if (cursor.ptr[i] != c_str[i]) {
                        match = false;
                        break;
                    }
                }
                if (match && c_str[cursor.len] != '\0') {
                    match = false;
                }
            }
            expected = match;
        }
    }
    assert(rv == expected);

    /* Postcondition 3: cursor remains valid */
    assert(aws_byte_cursor_is_valid(&cursor));
}
