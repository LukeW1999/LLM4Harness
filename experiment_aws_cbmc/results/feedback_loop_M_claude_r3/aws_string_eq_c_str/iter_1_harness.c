#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * Harness for aws_string_eq_c_str
 *
 * Function behavior:
 * - Returns true if str == NULL && c_str == NULL
 * - Returns false if exactly one of str or c_str is NULL
 * - Otherwise returns true iff the bytes of str equal the bytes of c_str
 *
 * Neither str nor c_str are modified by this function.
 */

void aws_string_eq_c_str_harness(void) {
    /* 1. Set up aws_string */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate an aws_string with bounded length */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_BUFFER_SIZE);

        /* Allocate memory for the aws_string header + bytes + null terminator */
        str = malloc(sizeof(struct aws_string) + len + 1);
        __CPROVER_assume(str != NULL);

        /* Initialize the fields (cast away const for initialization) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;
        /* bytes are nondet (already nondet from malloc) */
        /* Ensure null terminator */
        ((uint8_t *)str->bytes)[len] = '\0';

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Set up c_str */
    char *c_str = NULL;
    bool c_str_is_null = nondet_bool();

    if (!c_str_is_null) {
        size_t c_str_len = nondet_size_t();
        __CPROVER_assume(c_str_len <= MAX_BUFFER_SIZE);

        c_str = malloc(c_str_len + 1);
        __CPROVER_assume(c_str != NULL);
        /* Ensure null terminator */
        c_str[c_str_len] = '\0';
    }

    /* 3. Save old state */
    const struct aws_string *old_str = str;
    size_t old_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;

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

    /* 6. Assert that str is not modified */
    assert(str == old_str);
    if (str != NULL) {
        assert(str->len == old_len);
        assert(str->allocator == old_allocator);
        /* Validity invariant still holds */
        assert(aws_string_is_valid(str));
    }

    /* 7. The function returns a bool — no output pointer to check */
    /* result is either true or false, both are valid bool values */
    assert(result == true || result == false);
}
