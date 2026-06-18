#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Bound for string length and c_str length in the proof */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

void aws_string_eq_c_str_harness(void) {
    /* ------------------------------------------------------------------ */
    /* 1. Set up the aws_string argument (may be NULL)                     */
    /* ------------------------------------------------------------------ */
    struct aws_string *str = NULL;
    bool str_is_null;

    if (!str_is_null) {
        /* Allocate a concrete aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);

        size_t total_size = sizeof(struct aws_string) + len + 1;
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        struct aws_string *mutable_str = (struct aws_string *)str;
        *(struct aws_allocator **)&mutable_str->allocator = aws_default_allocator();
        *(size_t *)&mutable_str->len = len;

        uint8_t *bytes_ptr = (uint8_t *)mutable_str->bytes;
        bytes_ptr[len] = '\0';

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ------------------------------------------------------------------ */
    /* 2. Set up the c_str argument (may be NULL)                          */
    /* ------------------------------------------------------------------ */
    char *c_str = NULL;
    bool c_str_is_null;

    if (!c_str_is_null) {
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_STRING_LEN);
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[c_str_len] = '\0';
    }

    /* ------------------------------------------------------------------ */
    /* 3. Snapshot inputs before the call                                  */
    /* ------------------------------------------------------------------ */
    struct aws_allocator *old_str_allocator = (str != NULL) ? str->allocator : NULL;
    size_t old_str_len = (str != NULL) ? str->len : 0;

    /* ------------------------------------------------------------------ */
    /* 4. Call the function under test                                      */
    /* ------------------------------------------------------------------ */
    bool result = aws_string_eq_c_str(str, c_str);

    /* ------------------------------------------------------------------ */
    /* 5. Postconditions                                                    */
    /* ------------------------------------------------------------------ */

    /* 5a. Both NULL → must return true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* 5b. Exactly one NULL → must return false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* 5c. Result is a valid bool */
    assert(result == true || result == false);

    /* 5d. str fields are unchanged (frame condition) */
    if (str != NULL) {
        assert(str->allocator == old_str_allocator);
        assert(str->len == old_str_len);
        assert(aws_string_is_valid(str));
    }
}
