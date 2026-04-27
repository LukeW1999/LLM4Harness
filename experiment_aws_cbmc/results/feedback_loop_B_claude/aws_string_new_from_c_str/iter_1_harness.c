#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/proof_allocators.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef MAX_STRING_LEN
#    define MAX_STRING_LEN 32
#endif

void aws_string_new_from_c_str_harness(void) {
    /* 1. Set up allocator */
    struct aws_allocator *allocator = can_fail_allocator();

    /* 2. Set up a bounded c_str input */
    /* We need a null-terminated string of bounded length */
    size_t str_len;
    __CPROVER_assume(str_len <= MAX_STRING_LEN);

    /* Allocate a buffer for the c_str with null terminator */
    char *c_str = malloc(str_len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null terminator */
    c_str[str_len] = '\0';
    /* The rest of the bytes are nondet (already nondet from malloc) */

    /* 3. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Assert postconditions */
    if (result != NULL) {
        /* On success: the returned aws_string must be valid */
        assert(aws_string_is_valid(result));

        /* The length must match strlen(c_str) */
        assert(result->len == str_len);

        /* The allocator must be set to the provided allocator */
        assert(result->allocator == allocator);

        /* The bytes must match the input c_str */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, str_len);

        /* The bytes array must be null-terminated */
        assert(result->bytes[result->len] == '\0');
    } else {
        /* On failure (allocation failed): result is NULL, nothing to assert
           about the string itself. The c_str input is unchanged. */
        assert(result == NULL);
        /* c_str content is unchanged - verified by the fact we can still read it */
        assert(strlen(c_str) == str_len);
    }
}
