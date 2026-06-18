#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string str_storage = {0};
    struct aws_string *str = NULL;
    const char *c_str = NULL;

    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    char c_str_storage[MAX_STRING_LENGTH + 1] = {0};

    if (!str_is_null) {
        str = &str_storage;
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LENGTH);
    }

    if (!c_str_is_null) {
        c_str = c_str_storage;
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len <= MAX_STRING_LENGTH);
        c_str_storage[c_str_len] = '\0';
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL || c_str == NULL) {
        assert(!result);
    } else {
        if (str->len == 0) {
            bool c_str_is_empty = (c_str[0] == '\0');
            assert(result == c_str_is_empty);
        }
    }
}
