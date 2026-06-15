#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_string_new_from_c_str_harness() {
    struct aws_allocator *allocator;
    __CPROVER_assume(allocator != NULL);

    size_t c_str_len;
    __CPROVER_assume(c_str_len < MAX_BUFFER_SIZE);

    char *c_str = (char *)malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    __CPROVER_assume(AWS_MEM_IS_READABLE(c_str, c_str_len + 1));

    for (size_t i = 0; i < c_str_len; ++i) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[c_str_len] = '\0';

    struct aws_string *str = aws_string_new_from_c_str(allocator, c_str);

    assert(str != NULL);
    assert(aws_string_is_valid(str));
    assert(str->len == c_str_len);
    for (size_t i = 0; i < c_str_len; ++i) {
        assert(aws_string_bytes(str)[i] == (uint8_t)c_str[i]);
    }

    free(c_str);
    aws_string_destroy(str);
}
