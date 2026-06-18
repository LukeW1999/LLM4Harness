#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdbool.h>

void aws_string_eq_c_str_harness() {
    struct aws_string *str = NULL;
    char *c_str = NULL;

    bool str_is_null = nondet_bool();
    bool c_str_is_null = nondet_bool();

    struct {
        struct aws_string header;
        uint8_t bytes[MAX_STRING_LENGTH];
    } str_storage;

    char c_str_storage[MAX_STRING_LENGTH + 1];

    if (!str_is_null) {
        str = &str_storage.header;
        str->allocator = aws_default_allocator();
        str->len = nondet_size_t();
        __CPROVER_assume(str->len <= MAX_STRING_LENGTH);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    if (!c_str_is_null) {
        c_str = c_str_storage;
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len <= MAX_STRING_LENGTH);
        c_str[c_str_len] = '\0';
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        assert(result == true || result == false);
        if (str->len == 0) {
            assert(result == (c_str[0] == '\0'));
        }
    }
}
