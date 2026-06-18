#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 16
#endif

void aws_string_new_from_c_str_harness(void) {
    /* 1. Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 2. Create a non-deterministic c_str with bounded length */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_STRING_LEN);

    /* Allocate c_str with c_str_len bytes + null terminator */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator is present */
    c_str[c_str_len] = '\0';

    /* The actual length as seen by strlen must equal c_str_len.
       We need to ensure no embedded null bytes before c_str_len. */
    __CPROVER_assume(strlen(c_str) == c_str_len);

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions */
    if (result != NULL) {
        /* On success: the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* The length must match strlen(c_str) */
        assert(result->len == c_str_len);

        /* The allocator must be set to the provided allocator */
        assert(result->allocator == allocator);

        /* The bytes must match the input c_str */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_str_len);

        /* The string must be null-terminated after the data */
        assert(result->bytes[result->len] == '\0');

        /* Clean up */
        aws_string_destroy(result);
    }
}
