#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

    /* Snapshot for frame condition checks */
    struct aws_string *str_snapshot = NULL;
    uint8_t str_snapshot_storage[sizeof(struct aws_string) + MAX_STRING_LEN];
    if (str != NULL) {
        str_snapshot = (struct aws_string *)str_snapshot_storage;
        str_snapshot->allocator = str->allocator;
        str_snapshot->len = str->len;
        for (size_t i = 0; i < str->len; ++i) {
            str_snapshot->bytes[i] = str->bytes[i];
        }
        if (str->len < MAX_STRING_LEN) {
            str_snapshot->bytes[str->len] = str->bytes[str->len];
        }
    }

    char cstr_snapshot[MAX_CSTR_LEN + 1];
    if (c_str != NULL) {
        size_t i = 0;
        while (c_str[i] != '\0') ++i;
        for (size_t j = 0; j <= i; ++j) {
            cstr_snapshot[j] = c_str[j];
        }
    }

    /* Call function under verification */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Compute expected result */
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

    __CPROVER_assert(result == expected, "result matches expected");

    /* Frame condition: str unchanged */
    if (str != NULL) {
        __CPROVER_assert(str->allocator == str_snapshot->allocator, "allocator unchanged");
        __CPROVER_assert(str->len == str_snapshot->len, "len unchanged");
        for (size_t i = 0; i < str->len; ++i) {
            __CPROVER_assert(str->bytes[i] == str_snapshot->bytes[i], "bytes unchanged");
        }
        if (str->len < MAX_STRING_LEN) {
            __CPROVER_assert(str->bytes[str->len] == str_snapshot->bytes[str->len], "null terminator unchanged");
        }
    }

    /* Frame condition: c_str unchanged */
    if (c_str != NULL) {
        size_t i = 0;
        while (c_str[i] != '\0') ++i;
        for (size_t j = 0; j <= i; ++j) {
            __CPROVER_assert(c_str[j] == cstr_snapshot[j], "c_str unchanged");
        }
    }
}
