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

void harness(void) {
    /* 1. Non-deterministically decide whether str is NULL */
    struct aws_string *str = NULL;
    bool str_is_null;

    if (!str_is_null) {
        /* Allocate an aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate memory for the aws_string header + len bytes + 1 null terminator */
        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the length field (const, so use a cast) */
        *(size_t *)(&str->len) = len;

        /* Ensure the string is valid */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Non-deterministically decide whether c_str is NULL */
    char *c_str = NULL;
    bool c_str_is_null;

    if (!c_str_is_null) {
        /* Allocate a bounded C string */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_C_STR_LEN);

        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Ensure null terminator exists within bounds */
        c_str[c_str_len] = '\0';
    }

    /* 3. Save state before call */
    const struct aws_string *old_str = str;
    const char *old_c_str = c_str;

    /* 4. Call the function under test */
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

    /* 6. Assert that neither pointer was modified (pure function) */
    assert(str == old_str);
    assert(c_str == old_c_str);

    /* 7. If str is non-NULL, assert its validity is preserved */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
    }
}
