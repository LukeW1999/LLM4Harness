#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 256

void aws_string_eq_c_str_harness(void) {
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;
    struct aws_allocator *allocator = aws_default_allocator();

    uint8_t str_buf[sizeof(struct aws_string) + MAX_STRING_LEN];

    if (!str_is_null) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = (struct aws_string *)str_buf;
        str->allocator = allocator;
        str->len = len;
        __CPROVER_assume(aws_string_is_valid(str));
    }

    bool c_str_is_null = nondet_bool();
    char c_str_buf[MAX_STRING_LEN + 1];
    char *c_str = NULL;
    size_t c_len = 0;

    if (!c_str_is_null) {
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);
        c_str = c_str_buf;
        c_str[c_len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    }

    struct aws_allocator *old_str_allocator = NULL;
    size_t old_str_len = 0;
    struct store_byte_from_buffer old_str_bytes;
    if (str != NULL) {
        old_str_allocator = str->allocator;
        old_str_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_c_str_bytes;
    if (c_str != NULL) {
        save_byte_from_array((uint8_t *)c_str, c_len + 1, &old_c_str_bytes);
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = false;
        if (str->len == c_len) {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != (uint8_t)c_str[i]) {
                    expected = false;
                    break;
                }
            }
        }
        assert(result == expected);
    }

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }
    if (c_str != NULL) {
        assert_byte_from_buffer_matches((uint8_t *)c_str, &old_c_str_bytes);
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
