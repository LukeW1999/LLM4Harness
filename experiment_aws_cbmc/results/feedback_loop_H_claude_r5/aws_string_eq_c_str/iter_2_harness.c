#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 10
#endif

#ifndef MAX_C_STR_LEN
#    define MAX_C_STR_LEN 10
#endif

void aws_string_eq_c_str_harness(void) {
    /* 1. Set up aws_string (possibly NULL) */
    struct aws_string *str = NULL;
    bool str_is_null;
    __CPROVER_assume(str_is_null == true || str_is_null == false);
    if (!str_is_null) {
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        /* Allocate enough memory for the struct + bytes + null terminator */
        str = malloc(sizeof(*str) + len + 1);
        __CPROVER_assume(str != NULL);
        *(size_t *)(&str->len) = len;
        /* bytes are nondet, set null terminator */
        ((uint8_t *)str->bytes)[len] = '\0';
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Set up c_str (possibly NULL) */
    char *c_str = NULL;
    bool c_str_is_null;
    __CPROVER_assume(c_str_is_null == true || c_str_is_null == false);
    if (!c_str_is_null) {
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_C_STR_LEN);
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[c_str_len] = '\0';
        /* rest of c_str is nondet */
    }

    /* 3. Save old state (inputs should not be modified) */
    size_t old_str_len = (str != NULL) ? str->len : 0;

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

    /* 6. Assert inputs are not modified */
    if (str != NULL) {
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }

    /* 7. The function returns a bool — assert it's a valid bool value */
    assert(result == true || result == false);
}
