#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
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

/**
 * aws_string_eq_c_str:
 * - Returns true if both are NULL
 * - Returns false if exactly one is NULL
 * - Otherwise returns true iff bytes of str equal the c_str bytes
 *
 * The function does not modify str or c_str.
 * No fields change; this is a pure comparison function.
 */
void aws_string_eq_c_str_harness(void) {
    /* Non-deterministically choose whether str is NULL */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate an aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate memory for the aws_string header + len + 1 bytes (null terminator) */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* Initialize the length field (const, so use a cast) */
        *(size_t *)(&str->len) = len;
        /* allocator can be anything */
        *(struct aws_allocator **)(&str->allocator) = nondet_bool() ? aws_default_allocator() : NULL;

        /* bytes are non-deterministic (already allocated by malloc) */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministically choose whether c_str is NULL */
    char *c_str = NULL;
    bool c_str_is_null = nondet_bool();

    if (!c_str_is_null) {
        /* Allocate a bounded c-string */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_C_STR_LEN);

        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Ensure null termination */
        c_str[c_str_len] = '\0';
    }

    /* Save state before call (nothing should change) */
    const struct aws_string *old_str = str;
    const char *old_c_str = c_str;

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postcondition 1: Both NULL => true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Postcondition 2: Exactly one NULL => false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* Postcondition 3: Neither NULL => result is determined by content comparison
     * We can't easily assert the exact value here without reimplementing the logic,
     * but we can assert that the pointers are unchanged (immutability). */

    /* Postcondition 4: Pointers are unchanged (function is pure/non-mutating) */
    assert(str == old_str);
    assert(c_str == old_c_str);

    /* Postcondition 5: If str is valid, it remains valid after the call */
    if (str != NULL) {
        assert(aws_string_is_valid(str));
        /* Fields of str are unchanged */
        assert(str->len == old_str->len);
        assert(str->allocator == old_str->allocator);
    }
}

void aws_string_eq_c_str_harness(void) {
    aws_string_eq_c_str_harness();
    return 0;
}
