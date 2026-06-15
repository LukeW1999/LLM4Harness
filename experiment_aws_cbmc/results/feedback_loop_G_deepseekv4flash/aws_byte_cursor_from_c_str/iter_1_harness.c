#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>

void aws_byte_cursor_from_c_str_harness() {
    const char *c_str;

    /* Nondeterministically choose whether c_str is NULL or a valid bounded string */
    bool is_null = nondet_bool();
    if (is_null) {
        c_str = NULL;
    } else {
        c_str = (const char *)malloc(MAX_BUFFER_SIZE);
        __CPROVER_assume(c_str != NULL);
        __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, MAX_BUFFER_SIZE));
        /* Ensure the string is null-terminated within the allocated region */
        __CPROVER_assume(c_str[MAX_BUFFER_SIZE - 1] == '\0');
    }

    struct aws_byte_cursor cur = aws_byte_cursor_from_c_str(c_str);

    /* Postcondition: returned cursor's ptr equals input pointer (cast) */
    assert(cur.ptr == (uint8_t *)c_str);

    /* Postcondition: len is 0 if input is NULL, otherwise strlen of input */
    if (c_str == NULL) {
        assert(cur.len == 0);
    } else {
        size_t expected_len = strlen(c_str);
        assert(cur.len == expected_len);
    }

    /* Validity invariant always holds */
    assert(aws_byte_cursor_is_valid(&cur));
}
