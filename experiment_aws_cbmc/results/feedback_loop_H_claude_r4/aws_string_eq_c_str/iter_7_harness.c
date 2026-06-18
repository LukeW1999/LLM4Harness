#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 10
#endif

#ifndef MAX_C_STR_LEN
#    define MAX_C_STR_LEN 10
#endif

void harness(void) {
    struct aws_string *str = make_arbitrary_aws_string_nondet_len();

    char *c_str = NULL;
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_C_STR_LEN);

    int c_str_is_null;
    if (!c_str_is_null) {
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[c_str_len] = '\0';
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str != NULL) {
        __CPROVER_assert(aws_string_is_valid(str), "str still valid");
    }
}
