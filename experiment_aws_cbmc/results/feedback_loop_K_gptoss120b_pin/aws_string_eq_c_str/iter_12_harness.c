#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256
#define MAX_CSTR_LEN   256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    uint8_t str_storage[sizeof(struct aws_string) + MAX_STRING_LEN];

    if (!str_is_null) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = (struct aws_string *)str_storage;
        str->allocator = aws_default_allocator();
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        if (len < MAX_STRING_LEN) {
            str->bytes[len] = 0;
        }
    }

    char *c_str = NULL;
    bool cstr_is_null = nondet_bool();
    char c_str_buf[MAX_CSTR_LEN + 1];

    if (!cstr_is_null) {
        size_t c_len = (size_t)nondet_uint64_t();
        __CPROVER_assume(c_len <= MAX_CSTR_LEN);
        c_str = c_str_buf;
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
    }

    bool result = aws_string_eq_c_str(str, c_str);

    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        size_t i = 0;
        expected = true;
        while (i < str->len && c_str[i] != '\0') {
            if (str->bytes[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
            ++i;
        }
        if (expected) {
            if (i != str->len || c_str[i] != '\0') {
                expected = false;
            }
        }
    }

    __CPROVER_assert(result == expected, "aws_string_eq_c_str returns expected result");
}
