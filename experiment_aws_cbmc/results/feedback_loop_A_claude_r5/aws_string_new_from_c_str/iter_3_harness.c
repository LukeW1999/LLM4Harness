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

    /* 2. Create a bounded c_str input */
    size_t c_str_len;
    __CPROVER_assume(c_str_len <= MAX_STRING_LEN);

    /* Allocate c_str with c_str_len bytes + null terminator */
    char *c_str = malloc(c_str_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Make the content of c_str nondet but ensure null terminator is present */
    /* and no null bytes before position c_str_len */
    for (size_t i = 0; i < c_str_len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[c_str_len] = '\0';

    /* 3. Call function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions */
    if (result != NULL) {
        /* On success: the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* The length must match strlen(c_str) which equals c_str_len
           since we ensured no null bytes before c_str_len */
        assert(result->len == c_str_len);

        /* The allocator must be set to the provided allocator */
        assert(result->allocator == allocator);

        /* The bytes must match the input c_str */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, c_str_len);

        /* The string must be null-terminated (null byte after data) */
        assert(result->bytes[result->len] == '\0');

        /* Clean up */
        aws_string_destroy(result);
    }
}
