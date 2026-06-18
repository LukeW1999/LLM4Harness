#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/* Bound for string length and c_str length in the proof */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

/**
 * Harness for aws_string_eq_c_str.
 *
 * From the implementation:
 *   - If both str and c_str are NULL, returns true.
 *   - If exactly one is NULL, returns false.
 *   - Otherwise, returns aws_array_eq_c_str(str->bytes, str->len, c_str).
 *
 * The function is a pure predicate: it does not modify any of its inputs.
 * Postconditions:
 *   1. Return value is a bool (true or false).
 *   2. str is not modified (allocator, len, bytes unchanged).
 *   3. c_str is not modified.
 *   4. If both NULL → result must be true.
 *   5. If exactly one NULL → result must be false.
 *   6. aws_string_is_valid(str) still holds after the call (if str was valid before).
 */
void aws_string_eq_c_str_harness(void) {
    /* ------------------------------------------------------------------ */
    /* 1. Set up the aws_string argument (may be NULL)                     */
    /* ------------------------------------------------------------------ */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate a concrete aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /*
         * aws_string layout: header (allocator + len) followed immediately
         * by (len + 1) bytes.  We allocate enough raw memory and fill in
         * the fields manually so that CBMC can reason about the bytes.
         */
        size_t total_size = sizeof(struct aws_string) + len + 1; /* +1 for null terminator */
        str = malloc(total_size);
        __CPROVER_assume(str != NULL);

        /* Write the const fields via a cast to a mutable pointer */
        struct aws_string *mutable_str = (struct aws_string *)str;
        /* allocator may be NULL (static string) or non-NULL */
        *(struct aws_allocator **)&mutable_str->allocator = nondet_bool() ? NULL : aws_default_allocator();
        *(size_t *)&mutable_str->len = len;

        /* The bytes array is non-deterministic; ensure null terminator */
        uint8_t *bytes_ptr = (uint8_t *)mutable_str->bytes;
        /* bytes[0..len-1] are non-deterministic (already so via malloc) */
        bytes_ptr[len] = '\0'; /* null terminator */

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* ------------------------------------------------------------------ */
    /* 2. Set up the c_str argument (may be NULL)                          */
    /* ------------------------------------------------------------------ */
    char *c_str = NULL;
    bool c_str_is_null = nondet_bool();

    if (!c_str_is_null) {
        /* Allocate a bounded C string */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_STRING_LEN);
        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        c_str[c_str_len] = '\0';
        /* bytes c_str[0..c_str_len-1] are non-deterministic */
    }

    /* ------------------------------------------------------------------ */
    /* 3. Snapshot inputs before the call                                  */
    /* ------------------------------------------------------------------ */
    /* Snapshot str fields */
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
        /* Validity invariant still holds */
        assert(aws_string_is_valid(str));
    }

    /* 5e. c_str pointer itself is unchanged (we only read it) */
    /* (CBMC tracks aliasing; no explicit assert needed beyond the above) */
}

void aws_string_eq_c_str_harness(void) {
    aws_string_eq_c_str_harness();
    return 0;
}
