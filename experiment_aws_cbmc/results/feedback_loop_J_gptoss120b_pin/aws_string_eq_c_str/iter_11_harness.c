#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_c_str_harness(void) {
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    uint8_t str_buf[sizeof(struct aws_string) + MAX_STRING_LEN];

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        str = (struct aws_string *)str_buf;
        struct aws_allocator *allocator = aws_default_allocator();
        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
    }

    bool c_is_null = nondet_bool();
    char *c_str = NULL;
    size_t c_len = 0;
    char c_str_buf[MAX_STRING_LEN + 1];

    if (!c_is_null) {
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);

        c_str = c_str_buf;
        c_str[c_len] = '\0';

        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
    }

    bool result = aws_string_eq_c_str(str, c_str);

    bool expected = false;
    if (str && c_str) {
        size_t c_actual_len = strlen(c_str);
        if (str->len == c_actual_len) {
            expected = (memcmp(str->bytes, c_str, str->len) == 0);
        }
    } else if (!str && !c_str) {
        expected = true;
    }

    assert(result == expected);

    if (str) {
        assert(str->allocator == aws_default_allocator());
        assert(aws_string_is_valid(str));
    }

    if (c_str) {
        assert(c_str != NULL);
    }
}
