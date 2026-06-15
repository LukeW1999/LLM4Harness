#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <string.h>
#include <stdlib.h>

/* Helper to allocate a nondeterministic C string with a bounded length */
static char *nondet_c_string(void) {
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);
    char *s = malloc(len + 1);
    __CPROVER_assume(s != NULL);
    /* make the string readable */
    __CPROVER_assume(AWS_MEM_IS_READABLE(s, len + 1));
    /* nondeterministically fill the characters */
    for (size_t i = 0; i < len; ++i) {
        s[i] = nondet_uint8_t();
    }
    s[len] = '\0';
    return s;
}

void aws_byte_cursor_eq_c_str_harness(void) {
    /* 1. Declare and bound the cursor */
    struct aws_byte_cursor cursor;
    __CPROVER_assume(aws_byte_cursor_is_bounded(&cursor, MAX_BUFFER_SIZE));
    ensure_byte_cursor_has_allocated_buffer_member(&cursor);
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* 2. Allocate a nondeterministic C string */
    char *c_str = nondet_c_string();

    /* 3. Save old state */
    struct aws_byte_cursor old_cursor = cursor;
    char *old_c_str = c_str; /* pointer should not change */

    /* 4. Call function under test */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* 5. Unchanged fields */
    assert(cursor.ptr == old_cursor.ptr);
    assert(cursor.len == old_cursor.len);
    assert(c_str == old_c_str);

    /* 6. Validity invariant */
    assert(aws_byte_cursor_is_valid(&cursor));

    /* 7. Postcondition: result matches semantic definition */
    size_t c_len = strlen(c_str);
    bool expected = (cursor.len == c_len) && (c_len == 0 || memcmp(cursor.ptr, c_str, cursor.len) == 0);
    assert(result == expected);
}
