#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 16
#endif

void aws_string_new_from_c_str_harness(void) {
    /* 1. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Create a bounded c_str input */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STRING_LEN);

    /* Allocate str_len + 1 bytes for the c_str (including null terminator) */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Ensure the string is null-terminated at position str_len */
    for (size_t i = 0; i < str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[str_len] = '\0';

    /* 3. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions */
    if (result != NULL) {
        /* On success: the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* The length must equal strlen(c_str) == str_len */
        assert(result->len == str_len);

        /* The null terminator must be present after the data */
        assert(result->bytes[str_len] == '\0');

        /* The allocator must be set (since we used a real allocator) */
        assert(result->allocator == allocator);

        /* Clean up */
        aws_string_destroy(result);
    }
}
