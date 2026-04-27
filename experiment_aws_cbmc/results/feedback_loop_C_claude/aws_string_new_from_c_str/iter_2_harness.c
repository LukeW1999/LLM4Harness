#include <aws/common/string.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

/* Bound on c_str length to keep verification tractable */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 16
#endif

void aws_string_new_from_c_str_harness(void) {
    /* Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create a bounded, null-terminated c_str */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STRING_LEN);

    /* Allocate a buffer of str_len + 1 bytes (for null terminator) */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill with arbitrary bytes, ensure null terminator */
    c_str[str_len] = '\0';

    /* Save the length before the call */
    size_t expected_len = strlen(c_str);

    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        /* SUCCESS PATH assertions */

        /* The string must be valid */
        assert(aws_string_is_valid(result));

        /* Length must match strlen of the input */
        assert(result->len == expected_len);

        /* Allocator must match */
        assert(result->allocator == allocator);

        /* The string must be null-terminated after the data */
        assert(result->bytes[result->len] == '\0');
    }
    /* FAILURE PATH: result == NULL, nothing else to check */
}
