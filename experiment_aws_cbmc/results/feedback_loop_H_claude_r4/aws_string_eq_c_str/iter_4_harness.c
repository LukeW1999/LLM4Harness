#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 10
#endif

#ifndef MAX_C_STR_LEN
#    define MAX_C_STR_LEN 10
#endif

void harness(void) {
    struct aws_string *str = NULL;
    bool str_is_null;

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
        assert(result == true);
    }

    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }

    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    assert(str == old_str);
    assert(c_str == old_c_str);

    if (str != NULL) {
        assert(aws_string_is_valid(str));
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);
    }
}
