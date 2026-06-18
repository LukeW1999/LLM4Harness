#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

void aws_string_new_from_c_str_harness() {
    /* 1. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Create a bounded c_str input */
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_BUFFER_SIZE);

    /* Allocate a buffer of len+1 bytes for the c_str (including null terminator) */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Make the content nondet but ensure null termination */
    c_str[len] = '\0';
    /* The bytes before the null terminator can be anything (nondet by default) */
    /* Ensure no embedded nulls so strlen(c_str) == len */
    for (size_t i = 0; i < len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions */
    if (result != NULL) {
        /* On success: the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* The length must match strlen(c_str) == len */
        assert(result->len == len);

        /* The allocator must be set (since we used a real allocator) */
        assert(result->allocator == allocator);

        /* The bytes must match the input c_str */
        assert_bytes_match(aws_string_bytes(result), (const uint8_t *)c_str, len);

        /* The string must be null-terminated after the data */
        assert(result->bytes[len] == '\0');

        /* Clean up */
        aws_string_destroy(result);
    }
    /* On failure (result == NULL): nothing to assert about the string,
       the function simply returns NULL */

    free(c_str);
}
