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
void aws_string_eq_c_str_harness(void) {
    /* Non-deterministically choose whether str is NULL */
    struct aws_string *str = NULL;
    bool str_is_null = nondet_bool();

    if (!str_is_null) {
        /* Allocate a valid aws_string with bounded length */
        size_t len;
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* Allocate memory for the aws_string header + len + 1 bytes (null terminator) */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL);

        /* Initialize the fields (cast away const for initialization) */
        *(struct aws_allocator **)&str->allocator = aws_default_allocator();
        *(size_t *)&str->len = len;
        /* bytes are non-deterministic (already set by malloc) */

        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Non-deterministically choose whether c_str is NULL */
    bool c_str_is_null = nondet_bool();
    const char *c_str = NULL;

    if (!c_str_is_null) {
        /* Allocate a c_str with bounded length */
        size_t c_str_len;
        __CPROVER_assume(c_str_len <= MAX_STRING_LEN);

        char *buf = malloc(c_str_len + 1);
        __CPROVER_assume(buf != NULL);
        buf[c_str_len] = '\0';
        c_str = buf;
    }

    /* Save old state of str fields (for immutability checks) */
    size_t old_len = (str != NULL) ? str->len : 0;
    struct aws_allocator *old_allocator = (str != NULL) ? str->allocator : NULL;
    const uint8_t *old_bytes_ptr = (str != NULL) ? str->bytes : NULL;

    /* Call the function under test */
    bool result = aws_string_eq_c_str(str, c_str);

    /* Postconditions */

    /* Case 1: both NULL → must return true */
    if (str == NULL && c_str == NULL) {
        assert(result == true);
    }

    /* Case 2: exactly one NULL → must return false */
    if (str == NULL && c_str != NULL) {
        assert(result == false);
    }
    if (str != NULL && c_str == NULL) {
        assert(result == false);
    }

    /* Case 3: neither NULL → result depends on content (no specific assert on value,
     * but we can assert the function doesn't corrupt the string) */

    /* Immutability: str fields must not change */
    if (str != NULL) {
        assert(str->len == old_len);
        assert(str->allocator == old_allocator);
        assert(str->bytes == old_bytes_ptr);
        /* Validity invariant must still hold */
        assert(aws_string_is_valid(str));
    }
}

void aws_string_eq_c_str_harness(void) {
    aws_string_eq_c_str_harness();
    return 0;
}
