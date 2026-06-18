#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

#define MAX_STRING_LEN 64
#define MAX_CSTR_LEN   64

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    char *c_str;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        uint8_t *buf = (uint8_t *)malloc(len);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < len; ++i) {
            buf[i] = nondet_uint8_t();
        }
        struct aws_allocator *allocator = aws_default_allocator();
        str = aws_string_new_from_array(allocator, buf, len);
        __CPROVER_assume(str != NULL);
        free(buf);
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide whether c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len;
        __CPROVER_assume(c_len < MAX_CSTR_LEN);
        c_str = (char *)malloc(c_len + 1);
        __CPROVER_assume(c_str != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            c_str[i] = (char)nondet_uint8_t();
        }
        c_str[c_len] = '\0';
    }

    /* Preserve original state of str for later checks */
    struct aws_string old_str;
    uint8_t *old_bytes = NULL;
    if (str != NULL) {
        old_str.allocator = str->allocator;
        old_str.len = str->len;
        old_bytes = (uint8_t *)malloc(str->len + 1);
        __CPROVER_assume(old_bytes != NULL);
        for (size_t i = 0; i <= str->len; ++i) {
            old_bytes[i] = str->bytes[i];
        }
    }

    bool result = aws_string_eq_c_str(str, c_str);

    /* Compute expected result */
    bool expected;
    if (str == NULL && c_str == NULL) {
        expected = true;
    } else if (str == NULL || c_str == NULL) {
        expected = false;
    } else {
        expected = true;
        for (size_t i = 0; i < str->len; ++i) {
            if (c_str[i] == '\0' || (uint8_t)c_str[i] != str->bytes[i]) {
                expected = false;
                break;
            }
        }
        if (expected && c_str[str->len] != '\0') {
            expected = false;
        }
    }
    assert(result == expected);

    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_bytes, str->len + 1);
    }

    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }

    if (str != NULL) {
        aws_string_destroy(str);
        free(old_bytes);
    }
    if (c_str != NULL) {
        free(c_str);
    }
}
