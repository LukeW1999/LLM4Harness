#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 10
#endif

#ifndef MAX_C_STR_LEN
#    define MAX_C_STR_LEN 10
#endif

void harness(void) {
    struct aws_string *str = NULL;
    bool str_is_null;
    __CPROVER_assume(str_is_null == 0 || str_is_null == 1);

    if (!str_is_null) {
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);

        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);

        *(size_t *)(&str->len) = len;
        struct aws_allocator *allocator = aws_default_allocator();
        *(struct aws_allocator **)(&str->allocator) = allocator;

        ((uint8_t *)str->bytes)[len] = '\0';

        __CPROVER_assume(aws_string_is_valid(str));
    }

    char *c_str = NULL;
    bool c_str_is_null;
    __CPROVER_assume(c_str_is_null == 0 || c_str_is_null == 1);

    if (!c_str_is_null) {
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_C_STR_LEN);

        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);

        c_str[c_str_len] = '\0';
    }

    const struct aws_string *old_str = str;
    const char *old_c_str = c_str;

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        __CPROVER_assert(result == true, "null str and null c_str should be equal");
    }

    if (str == NULL && c_str != NULL) {
        __CPROVER_assert(result == false, "null str and non-null c_str should not be equal");
    }

    if (str != NULL && c_str == NULL) {
        __CPROVER_assert(result == false, "non-null str and null c_str should not be equal");
    }

    __CPROVER_assert(str == old_str, "str pointer unchanged");
    __CPROVER_assert(c_str == old_c_str, "c_str pointer unchanged");

    if (str != NULL) {
        __CPROVER_assert(aws_string_is_valid(str), "str still valid");
        __CPROVER_assert(str->len == old_str->len, "str len unchanged");
        __CPROVER_assert(str->allocator == old_str->allocator, "str allocator unchanged");
    }
}
