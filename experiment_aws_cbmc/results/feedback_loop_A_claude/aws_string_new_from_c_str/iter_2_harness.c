#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 32
#endif

void aws_string_new_from_c_str_harness(void) {
    /* 1. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Set up a bounded c_str with nondet length */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate a buffer of len+1 bytes (for null terminator) */
    char *c_str_buf = (char *)malloc(len + 1);
    __CPROVER_assume(c_str_buf != NULL);

    /* Ensure no null bytes in the first len bytes, and null-terminate */
    for (size_t i = 0; i < len; i++) {
        __CPROVER_assume(c_str_buf[i] != '\0');
    }
    c_str_buf[len] = '\0';

    const char *c_str = (const char *)c_str_buf;

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions */
    if (result != NULL) {
        /* Success path: result is a valid aws_string */
        assert(aws_string_is_valid(result));

        /* Length must match strlen(c_str) which equals len */
        assert(result->len == len);

        /* Allocator must be set to the provided allocator */
        assert(result->allocator == allocator);

        /* Null terminator must be present after the data */
        assert(result->bytes[result->len] == '\0');
    }
    /* else: NULL return is valid on allocation failure, no assertions needed */
}
