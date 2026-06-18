#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

/* Bound on the c_str length to keep the state space manageable */
#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 8
#endif

void aws_string_new_from_c_str_harness(void) {
    /* 1. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Create a bounded, null-terminated C string non-deterministically */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_STRING_LEN);

    /* Allocate c_str_len + 1 bytes so we can null-terminate */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Make the content non-deterministic but ensure null termination */
    c_str[c_str_len] = '\0';

    /* Ensure there are no embedded null bytes before c_str_len,
       so that strlen(c_str) == c_str_len */
    for (size_t i = 0; i < c_str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    /* 3. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Postconditions */

    if (result != NULL) {
        /* SUCCESS PATH */

        /* The returned string must be valid */
        assert(aws_string_is_valid(result));

        /* The length must equal strlen(c_str) which equals c_str_len */
        assert(result->len == c_str_len);

        /* The bytes must match the input c_str */
        assert_bytes_match((const uint8_t *)c_str, result->bytes, c_str_len);

        /* There must be a null terminator immediately after the data */
        assert(result->bytes[result->len] == '\0');

        /* The allocator field must be set to the allocator passed in */
        assert(result->allocator == allocator);

        /* The input c_str must be unchanged (frame condition) */
        assert(c_str[c_str_len] == '\0');

        /* Clean up */
        aws_string_destroy(result);
    } else {
        /* FAILURE PATH */
        /* On failure, result is NULL — nothing else to assert about result */
        /* The input c_str must be unchanged (frame condition) */
        assert(c_str[c_str_len] == '\0');
    }

    /* The allocator pointer itself must be unchanged */
    assert(allocator == aws_default_allocator());
}
