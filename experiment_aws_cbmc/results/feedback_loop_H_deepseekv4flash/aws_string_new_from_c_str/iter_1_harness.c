#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_new_from_c_str_harness() {
    /* Non‑deterministic allocator; assume it is valid (allocator != NULL) */
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    /* Bound the length of the C‑string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);

    /* Allocate memory for c_str (length + 1 for null terminator) */
    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    /* Fill with arbitrary non‑null bytes (except the final null) */
    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[c_str_len] = '\0';  /* guarantee null termination */

    /* Call the function under verification */
    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);

    /* Postconditions */
    if (str != NULL) {
        /* On success: returned string is valid, length matches, content matches */
        assert(aws_string_is_valid(str));
        assert(str->len == c_str_len);
        for (size_t i = 0; i < c_str_len; ++i) {
            assert(aws_string_bytes(str)[i] == (uint8_t)c_str[i]);
        }
    }
    /* On failure (str == NULL): nothing to assert about the string */

    /* All inputs are const; no mutable fields to check for unchangedness. */
}
