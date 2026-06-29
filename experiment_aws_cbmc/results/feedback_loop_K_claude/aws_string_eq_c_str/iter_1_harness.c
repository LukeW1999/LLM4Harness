#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* Bound for string length to keep verification tractable */
#define MAX_STRING_LEN 8

/*
 * Harness for aws_string_eq_c_str
 */
void aws_string_eq_c_str_harness(void) {
    /* Nondeterministically choose whether str is NULL */
    struct aws_string *str;
    bool str_is_null;
    __CPROVER_assume(str_is_null == true || str_is_null == false);

    if (str_is_null) {
        str = NULL;
    } else {
        /* Create a valid aws_string with bounded length */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        /* Allocate memory for aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + str_len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the length field (bytes field is already nondeterministic) */
        *((size_t *)&str->len) = str_len;
        *((struct aws_allocator **)&str->allocator) = aws_default_allocator();

        /* Ensure the null terminator is set after the data bytes */
        ((uint8_t *)str->bytes)[str_len] = '\0';
    }

    /* Nondeterministically choose whether c_str is NULL */
    char *c_str;
    bool c_str_is_null;
    __CPROVER_assume(c_str_is_null == true || c_str_is_null == false);

    if (c_str_is_null) {
        c_str = NULL;
    } else {
        /* Create a valid null-terminated C string with bounded length */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_STRING_LEN);

        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Ensure null terminator is present */
        c_str[c_str_len] = '\0';
        /* Ensure no embedded nulls before the terminator */
        for (size_t i = 0; i < c_str_len; i++) {
            __CPROVER_assume(c_str[i] != '\0');
        }
    }

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postcondition: both NULL => true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Postcondition: exactly one NULL => false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* Postcondition: both non-NULL => result reflects byte equality */
    if (str != NULL && c_str != NULL) {
        /* Verify the result is a valid boolean */
        assert(result == true || result == false);

        /* If result is true, verify lengths match and bytes are equal */
        if (result == true) {
            size_t c_len = 0;
            while (c_str[c_len] != '\0') {
                c_len++;
            }
            assert(str->len == c_len);
            for (size_t i = 0; i < str->len; i++) {
                assert(str->bytes[i] == (uint8_t)c_str[i]);
            }
        }

        /* If result is false, verify that strings differ */
        if (result == false) {
            size_t c_len = 0;
            while (c_str[c_len] != '\0') {
                c_len++;
            }
            bool differ = (str->len != c_len);
            if (!differ) {
                for (size_t i = 0; i < str->len; i++) {
                    if (str->bytes[i] != (uint8_t)c_str[i]) {
                        differ = true;
                        break;
                    }
                }
            }
            assert(differ);
        }
    }

    /* Frame condition: str->len is unchanged (checked implicitly since we don't modify it) */
    /* Frame condition: c_str content is unchanged (checked implicitly) */
}
