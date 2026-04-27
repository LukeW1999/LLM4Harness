#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

#ifndef MAX_C_STR_LEN
#    define MAX_C_STR_LEN 8
#endif

void aws_string_eq_c_str_harness(void) {
    /* 1. Non-deterministically decide whether str is NULL */
    struct aws_string *str;
    bool str_is_null;
    __CPROVER_assume(str_is_null == true || str_is_null == false);

    if (str_is_null) {
        str = NULL;
    } else {
        /* Allocate a valid aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        /* Allocate memory for the aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);
        /* Initialize the length field (const, so use a cast) */
        *(size_t *)(&str->len) = len;
        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Non-deterministically decide whether c_str is NULL */
    bool c_str_is_null;
    __CPROVER_assume(c_str_is_null == true || c_str_is_null == false);
    const char *c_str;

    if (c_str_is_null) {
        c_str = NULL;
    } else {
        /* Allocate a bounded c-string */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_C_STR_LEN);
        char *buf = malloc(c_str_len + 1);
        __CPROVER_assume(buf != NULL);
        /* Ensure null terminator exists */
        buf[c_str_len] = '\0';
        c_str = buf;
    }

    /* 3. Save old state */
    size_t old_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;

    /* 4. Call function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* 5. Assert postconditions */

    /* Both NULL → must return true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Exactly one NULL → must return false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* 6. Assert str fields are unchanged (immutability) */
    if (str != NULL) {
        assert(str->len == old_len);
        assert(str->allocator == old_allocator);
        /* Validity invariant preserved */
        assert(aws_string_is_valid(str));
    }

    /* 7. Result is a valid boolean */
    assert(result == true || result == false);
}
