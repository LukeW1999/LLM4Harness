#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

static bool expected_eq_c_str(const struct aws_string *str, const char *c_str) {
    if (str == NULL && c_str == NULL) {
        return true;
    }
    if (str == NULL || c_str == NULL) {
        return false;
    }
    size_t i = 0;
    while (i < str->len && c_str[i] != '\0') {
        if ((unsigned char)str->bytes[i] != (unsigned char)c_str[i]) {
            return false;
        }
        i++;
    }
    if (i < str->len) {
        return false;
    }
    return c_str[i] == '\0';
}

void aws_string_eq_c_str_harness(void) {
    struct aws_string *str;
    const char *c_str;
    bool result;
    bool expected;

    /* nondeterministically decide whether str is NULL */
    if (nondet_bool()) {
        str = NULL;
    } else {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);
        str->allocator = NULL;
        str->len = len;
        for (size_t i = 0; i < len; ++i) {
            str->bytes[i] = nondet_uint8_t();
        }
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* nondeterministically decide whether c_str is NULL */
    if (nondet_bool()) {
        c_str = NULL;
    } else {
        size_t c_len = nondet_size_t();
        __CPROVER_assume(c_len <= MAX_STRING_LEN);
        char *buf = malloc(c_len + 1);
        __CPROVER_assume(buf != NULL);
        for (size_t i = 0; i < c_len; ++i) {
            buf[i] = nondet_char();
        }
        buf[c_len] = '\0';
        __CPROVER_assume(aws_c_string_is_valid(buf));
        c_str = buf;
    }

    /* Save old state of inputs */
    struct aws_string old_str;
    struct store_byte_from_buffer old_bytes;
    if (str != NULL) {
        old_str = *str;
        save_byte_from_array(str->bytes, str->len, &old_bytes);
    }
    const char *old_c_str = c_str;

    /* Call function under test */
    result = aws_string_eq_c_str(str, c_str);

    /* Compute expected result and assert correctness */
    expected = expected_eq_c_str(str, c_str);
    assert(result == expected);

    /* Assert that inputs are unchanged */
    if (str != NULL) {
        assert(str->allocator == old_str.allocator);
        assert(str->len == old_str.len);
        assert_bytes_match(str->bytes, old_bytes.arr, str->len);
    }
    if (c_str != NULL) {
        assert(c_str == old_c_str);
    }

    /* Re‑assert validity invariants after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
    if (c_str != NULL) {
        assert(aws_c_string_is_valid(c_str));
    }
}
