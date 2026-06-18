#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

bool nondet_bool(void);
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

#define MAX_STRING_SIZE 256

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;

    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_SIZE);

        struct {
            struct aws_string s;
            uint8_t bytes[MAX_STRING_SIZE];
        } str_wrapper;

        str = &str_wrapper.s;
        str->allocator = aws_default_allocator();
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    if (nondet_bool()) {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_SIZE);
        char *tmp = malloc(c_len + 1);
        __CPROVER_assume(tmp != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            tmp[i] = (char)nondet_uint8_t();
        }
        tmp[c_len] = '\0';
        c_str = tmp;
        __CPROVER_assume(aws_c_string_is_valid(c_str));
    } else {
        c_str = NULL;
    }

    struct aws_allocator *old_allocator = NULL;
    size_t old_len = 0;
    struct store_byte_from_buffer old_str_bytes = {0};

    if (str != NULL) {
        old_allocator = str->allocator;
        old_len = str->len;
        save_byte_from_array(str->bytes, str->len, &old_str_bytes);
    }

    struct store_byte_from_buffer old_c_str_bytes = {0};
    size_t c_str_len = 0;
    if (c_str != NULL) {
        while (c_str[c_str_len] != '\0') {
            ++c_str_len;
        }
        save_byte_from_array((const uint8_t *)c_str, c_str_len, &old_c_str_bytes);
    }

    bool result = aws_string_eq_c_str(str, c_str);

    if (str == NULL && c_str == NULL) {
        assert(result == true);
    } else if (str == NULL || c_str == NULL) {
        assert(result == false);
    } else {
        bool expected = true;
        size_t i = 0;
        while (i < str->len && c_str[i] != '\0') {
            if (str->bytes[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
            ++i;
        }
        if (i != str->len || c_str[i] != '\0') {
            expected = false;
        }
        assert(result == expected);
    }

    if (str != NULL) {
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        assert_byte_from_buffer_matches(str->bytes, &old_str_bytes);
    }

    if (c_str != NULL) {
        assert_byte_from_buffer_matches((const uint8_t *)c_str, &old_c_str_bytes);
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
