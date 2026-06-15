#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>

#define MAX_STRING_LEN 32

void aws_byte_cursor_from_c_str_harness() {
    /* nondet string pointer */
    const char *c_str;
    struct aws_byte_cursor cur;
    char str[MAX_STRING_LEN];

    /* either null or a valid null-terminated string within bounded buffer */
    __CPROVER_assume(c_str == NULL || c_str == str);

    if (c_str != NULL) {
        size_t len;
        __CPROVER_assume(len < MAX_STRING_LEN);
        /* non-null characters before null */
        for (size_t i = 0; i < len; i++) {
            str[i] = nondet_uint8_t();
            __CPROVER_assume(str[i] != '\0');
        }
        /* null terminator */
        str[len] = '\0';
        /* remaining bytes are arbitrary, but must be readable */
    }

    /* record the expected length */
    size_t expected_len;
    if (c_str == NULL) {
        expected_len = 0;
    } else {
        expected_len = strlen(c_str);
    }

    /* call function under test */
    cur = aws_byte_cursor_from_c_str(c_str);

    /* postconditions */
    assert(cur.ptr == (uint8_t *)c_str);
    assert(cur.len == expected_len);
    
    /* validity of returned cursor */
    assert(aws_byte_cursor_is_valid(&cur));
}
