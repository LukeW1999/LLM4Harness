#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_BUFFER_SIZE 256

extern bool nondet_bool(void);
extern size_t nondet_size_t(void);
extern uint8_t nondet_uint8_t(void);

void aws_string_eq_c_str_harness(void) {
    bool str_is_null = nondet_bool();
    struct aws_string *str = NULL;

    bool c_is_null = nondet_bool();
    char *c_str = NULL;

    size_t str_len = 0;
    uint8_t old_str_bytes[MAX_BUFFER_SIZE];
    struct aws_allocator *old_str_allocator = NULL;
    uint8_t str_mem[sizeof(struct aws_string) + MAX_BUFFER_SIZE];
    if (!str_is_null) {
        str_len = nondet_size_t();
        __CPROVER_assume(str_len <= MAX_BUFFER_SIZE);

        str = (struct aws_string *)str_mem;
        str->allocator = aws_default_allocator();
        str->len = str_len;

        for (size_t i = 0; i < str_len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        str->bytes[str_len] = 0;

        old_str_allocator = str->allocator;
        for (size_t i = 0; i < str_len; ++i) {
            old_str_bytes[i] = str->bytes[i];
        }
    }

    size_t c_len = 0;
    char old_c_str[MAX_BUFFER_SIZE + 1];
    char c_mem[MAX_BUFFER_SIZE + 1];
    if (!c_is_null) {
        c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_BUFFER_SIZE);

        c_str = c_mem;
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';

        for (size_t i = 0; i <= c_len; ++i) {
            old_c_str[i] = c_str[i];
        }
    }

    bool result = aws_string_eq_c_str(str, c_str);

    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = true;
        for (size_t i = 0; i < str->len; ++i) {
            if ((uint8_t)str->bytes[i] != (uint8_t)c_str[i]) {
                expected = false;
                break;
            }
        }
        if (expected) {
            if (c_str[str->len] != '\0') {
                expected = false;
            }
        }
    }
    assert(result == expected);

    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == str_len);
        for (size_t i = 0; i < str_len; ++i) {
            assert(str->bytes[i] == old_str_bytes[i]);
        }
        assert(aws_string_is_valid(str));
    }

    if (c_str != NULL) {
        for (size_t i = 0; i <= c_len; ++i) {
            assert(c_str[i] == old_c_str[i]);
        }
    }
}
