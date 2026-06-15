/*
 * Harness for aws_string_new_from_c_str
 * Verifies that the function respects its contract when given a
 * non‑NULL allocator and a properly NUL‑terminated C string without
 * interior NUL bytes.
 */

#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <aws/common/string.h>
#include <aws/common/allocator.h>

#include "proof_helpers/make_common_data_structures.h"

#define MAX_STR_LEN 256

void aws_string_new_from_c_str_harness(void) {
    /* Allocate a default allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);

    /* Choose a nondeterministic string length within bounds */
    size_t str_len = nondet_size_t();
    __CPROVER_assume(str_len <= MAX_STR_LEN);

    /* Allocate memory for the C string (including space for the terminating NUL) */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill the string with nondeterministic non‑NUL bytes */
    for (size_t i = 0; i < str_len; ++i) {
        c_str[i] = nondet_char();
        __CPROVER_assume(c_str[i] != '\0');
    }
    /* Ensure proper NUL termination */
    c_str[str_len] = '\0';

    /* Preserve a copy of the original string for later comparison */
    char *c_str_orig = malloc(str_len + 1);
    __CPROVER_assume(c_str_orig != NULL);
    memcpy(c_str_orig, c_str, str_len + 1);

    /* Call the function under verification */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postcondition: result is either NULL or a valid aws_string */
    if (result != NULL) {
        assert(aws_string_is_valid(result));

        /* Allocator field must match the input allocator */
        assert(result->allocator == allocator);

        /* Length must equal the original C string length */
        assert(result->len == str_len);

        /* Bytes must match the original C string content (excluding the implicit NUL) */
        for (size_t i = 0; i < result->len; ++i) {
            assert(result->bytes[i] == (uint8_t)c_str_orig[i]);
        }

        /* Implicit NUL terminator after the data */
        assert(result->bytes[result->len] == '\0');
    }

    /* Frame condition: c_str and allocator must be unchanged */
    for (size_t i = 0; i < str_len + 1; ++i) {
        assert(c_str[i] == c_str_orig[i]);
    }

    /* Clean up */
    free(c_str);
    free(c_str_orig);
    if (result != NULL) {
        aws_string_destroy(result);
    }
}
