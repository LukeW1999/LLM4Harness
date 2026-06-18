#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Bound for string length and c_str length */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

void aws_string_eq_c_str_harness(void) {
    /* 1. Set up aws_string (possibly NULL) */
    struct aws_string *str = NULL;
    bool str_is_null;
    __CPROVER_assume(str_is_null == true || str_is_null == false);

    if (!str_is_null) {
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        /* Allocate enough memory for the struct plus len bytes plus null terminator */
        str = malloc(sizeof(*str) + len + 1);
        __CPROVER_assume(str != NULL);
        *(size_t *)(&str->len) = len;
        /* Ensure null terminator is present after the data */
        ((uint8_t *)str->bytes)[len] = '\0';
    }

    /* 2. Set up c_str (possibly NULL) */
    char *c_str = NULL;
    bool c_str_is_null;
    __CPROVER_assume(c_str_is_null == true || c_str_is_null == false);

    if (!c_str_is_null) {
        /* Allocate a non-deterministic c_str of bounded length */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_STRING_LEN);
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Ensure null terminator */
        c_str[c_str_len] = '\0';
    }

    /* 3. Save state before call */
    const struct aws_string *old_str = str;
    size_t old_len = (str != NULL) ? str->len : 0;

    /* 4. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 5. Assert postconditions */

    /* Case: both NULL → must return true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Case: exactly one NULL → must return false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* 6. Assert that str is not modified (immutability) */
    if (str != NULL) {
        assert(str->len == old_len);
        assert(str == old_str);
    }

    /* 7. The function returns a bool — result must be true or false */
    assert(result == true || result == false);
}
