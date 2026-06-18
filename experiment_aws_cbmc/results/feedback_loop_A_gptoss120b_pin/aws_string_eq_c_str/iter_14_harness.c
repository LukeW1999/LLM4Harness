#include <stdbool.h>
#include <stddef.h>
#include <proof_helpers/make_common_data_structures.h>
#include <aws/common/string.h>

#define MAX_STRING_LEN 64
#define MAX_CSTR_LEN   64

void aws_string_eq_c_str_harness(void) {
    bool str_is_null = nondet_bool();
    bool cstr_is_null = nondet_bool();

    struct aws_string *str = NULL;
    char *c_str = NULL;

    struct aws_allocator *allocator = aws_default_allocator();

    /* ----- set up str ----- */
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } str_buf;
    if (!str_is_null) {
        size_t len = (size_t)nondet_uint64_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        str = &str_buf.s;
        str->allocator = allocator;
        str->len = len;

        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
    }

    /* ----- set up c_str ----- */
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

    /* ----- save old state for immutability checks ----- */
    struct aws_string *old_str = NULL;
    struct {
        struct aws_string s;
        uint8_t bytes[MAX_STRING_LEN];
    } old_str_buf;
    if (str != NULL) {
        old_str = &old_str_buf.s;
        old_str->allocator = str->allocator;
        old_str->len = str->len;
        for (size_t i = 0; i < str->len; ++i) {
            old_str->bytes[i] = str->bytes[i];
        }
    }

    char old_c_str_buf[MAX_CSTR_LEN + 1];
    char *old_c_str = NULL;
    size_t old_c_len = 0;
    if (c_str != NULL) {
        while (c_str[old_c_len] != '\0') {
            ++old_c_len;
        }
        old_c_str = old_c_str_buf;
        for (size_t i = 0; i <= old_c_len; ++i) {
            old_c_str[i] = c_str[i];
        }
    }

    /* ----- call function under test ----- */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ----- post‑condition: functional correctness ----- */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        size_t c_len = 0;
        while (c_str[c_len] != '\0') {
            ++c_len;
        }
        if (str->len != c_len) {
            expected = false;
        } else {
            expected = true;
            for (size_t i = 0; i < str->len; ++i) {
                if (str->bytes[i] != (uint8_t)c_str[i]) {
                    expected = false;
                    break;
                }
            }
        }
    }
    __CPROVER_assert(result == expected, "functional correctness");

    /* ----- immutability of inputs ----- */
    if (str != NULL) {
        __CPROVER_assert(str->allocator == old_str->allocator, "allocator unchanged");
        __CPROVER_assert(str->len == old_str->len, "length unchanged");
        for (size_t i = 0; i < str->len; ++i) {
            __CPROVER_assert(str->bytes[i] == old_str->bytes[i], "bytes unchanged");
        }
    }
    if (c_str != NULL) {
        for (size_t i = 0; i <= old_c_len; ++i) {
            __CPROVER_assert(c_str[i] == old_c_str[i], "c_str unchanged");
        }
    }

    /* ----- validity invariants ----- */
    if (str != NULL) {
        __CPROVER_assert(aws_string_is_valid(str), "str valid");
    }
    if (c_str != NULL) {
        __CPROVER_assert(aws_c_string_is_valid(c_str), "c_str valid");
    }
}
