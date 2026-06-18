#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* Bound for string length and c_str length */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

/**
 * aws_string_eq_c_str:
 * - Returns true if both are NULL
 * - Returns false if exactly one is NULL
 * - Otherwise returns true iff bytes of str equal the bytes of c_str
 *
 * The function does not modify str or c_str.
 */

/* Helper to allocate a valid aws_string non-deterministically */
struct aws_string *make_arbitrary_aws_string_nondet_len_with_max(size_t max_len) {
    size_t len;
    __CPROVER_assume(len <= max_len);
    /* Allocate header + len bytes + 1 null terminator */
    struct aws_string *str = malloc(sizeof(struct aws_string) + len + 1);
    __CPROVER_assume(str != NULL);
    /* Set the length field (const, so we use a cast trick) */
    *(size_t *)&str->len = len;
    /* allocator can be anything (including NULL for static strings) */
    /* bytes are non-deterministic, but we need the null terminator */
    /* The bytes field is const uint8_t[1], actual data follows in memory */
    /* We just need the memory to be readable */
    return str;
}

void aws_string_eq_c_str_harness(void) {
    /* 1. Set up aws_string (possibly NULL) */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);
        /* Allocate enough memory for the struct plus len bytes plus null terminator */
        str = malloc(sizeof(*str) + len + 1);
        __CPROVER_assume(str != NULL);
        *(size_t *)(&str->len) = len;
        /* bytes are non-deterministic (already set by malloc non-det) */
        /* Ensure null terminator is present after the data */
        ((uint8_t *)str->bytes)[len] = '\0';
    }

    /* 2. Set up c_str (possibly NULL) */
    char *c_str = NULL;
    bool c_str_is_null = nondet_bool();

    if (!c_str_is_null) {
        /* Allocate a non-deterministic c_str of bounded length */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_STRING_LEN);
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Ensure null terminator */
        c_str[c_str_len] = '\0';
    }

    /* 3. Save state before call (str is immutable, just save pointer) */
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
        /* The pointer itself should not have changed */
        assert(str == old_str);
    }

    /* 7. The function returns a bool — result must be true or false */
    assert(result == true || result == false);
}

void aws_string_eq_c_str_harness(void) {
    aws_string_eq_c_str_harness();
    return 0;
}
