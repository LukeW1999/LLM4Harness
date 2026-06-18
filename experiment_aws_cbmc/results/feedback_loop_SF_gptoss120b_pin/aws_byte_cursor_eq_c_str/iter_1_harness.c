#include <aws/common/byte_buf.h>
#include <aws/common/byte_cursor.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void aws_byte_cursor_eq_c_str_harness(void) {
    struct aws_byte_cursor cursor;
    /* nondeterministic length for the cursor */
    cursor.len = (size_t)nondet_uint();
    __CPROVER_assume(cursor.len <= 1024U);

    /* allocate memory for cursor.ptr */
    cursor.ptr = malloc(cursor.len);
    __CPROVER_assume(cursor.ptr != NULL || cursor.len == 0U);

    /* nondeterministic length for the C string (excluding null terminator) */
    size_t c_str_len = (size_t)nondet_uint();
    __CPROVER_assume(c_str_len <= 1024U);

    /* allocate memory for the C string (including null terminator) */
    char *c_str = malloc(c_str_len + 1U);
    __CPROVER_assume(c_str != NULL);

    /* ensure null termination */
    __CPROVER_assume(c_str[c_str_len] == '\0');

    /* precondition: cursor must be valid */
    __CPROVER_assume(aws_byte_cursor_is_valid(&cursor));

    /* snapshot of input state */
    size_t old_len = cursor.len;
    uint8_t *old_ptr = cursor.ptr;
    char *old_c_str = c_str;

    /* call the function under verification */
    bool result = aws_byte_cursor_eq_c_str(&cursor, c_str);

    /* postconditions */
    /* input objects remain unchanged */
    assert(cursor.len == old_len);
    assert(cursor.ptr == old_ptr);
    assert(c_str == old_c_str);
    assert(c_str[c_str_len] == '\0');

    /* result reflects exact equality of the cursor and the C string */
    if (result) {
        assert(cursor.len == strlen(c_str));
        assert(memcmp(cursor.ptr, c_str, cursor.len) == 0);
    } else {
        assert(!(cursor.len == strlen(c_str) && memcmp(cursor.ptr, c_str, cursor.len) == 0));
    }
}
