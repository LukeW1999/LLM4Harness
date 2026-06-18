#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 10
#endif

#ifndef MAX_C_STR_LEN
#    define MAX_C_STR_LEN 10
#endif

void harness(void) {
    /* 1. Create a non-deterministic aws_string */
    struct aws_string *str = nondet_allocate_aws_string_nondet_body_with_max_size(MAX_STRING_LEN);

    /* 2. Create a non-deterministic C string */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_C_STR_LEN);
    char *c_str = malloc(c_str_len + 1);
    if (c_str != NULL) {
        c_str[c_str_len] = '\0';
    }

    /* 3. Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 4. If str is non-NULL, assert its validity is preserved */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
