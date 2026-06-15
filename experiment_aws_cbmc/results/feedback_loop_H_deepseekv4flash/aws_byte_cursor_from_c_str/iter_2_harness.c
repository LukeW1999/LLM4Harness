#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

#define MAX_STRING_LEN 32

void aws_byte_cursor_from_c_str_harness() {
    /* Nondet string pointer: either NULL or points to a valid bounded string */
    const char *c_str;
    struct aws_byte_cursor cur;
    char str[MAX_STRING_LEN];

    /* Choose whether c_str is NULL */
    int is_null;
    __CPROVER_assume(is_null == 0 || is_null == 1);

    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);

    if (is_null) {
        c_str = NULL;
    } else {
        c_str = str;
        /* Fill first len bytes with non-null characters */
        for (size_t i = 0; i < len; i++) {
            str[i] = nondet_uint8_t();
            __CPROVER_assume(str[i] != '\0');
        }
        /* Null terminator */
        str[len] = '\0';
    }

    /* Expected length: use stored len, not strlen, to avoid symbolic strlen */
    size_t expected_len = (c_str == NULL) ? 0 : len;

    /* Call function under test */
    cur = aws_byte_cursor_from_c_str(c_str);

    /* Postconditions */
    assert(cur.ptr == (uint8_t *)c_str);
    assert(cur.len == expected_len);
    assert(aws_byte_cursor_is_valid(&cur));
}
