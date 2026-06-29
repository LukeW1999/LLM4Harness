#include <aws/common/string.h>
#include <aws/common/common.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_string_new_from_c_str_harness(void) {
    /* Setup: use aws_default_allocator */
    struct aws_allocator *allocator = aws_default_allocator();

    /* Create a nondeterministic c_str with bounded length for CBMC tractability */
    size_t max_len = 16;
    size_t len;
    __CPROVER_assume(len <= max_len);

    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    /* Ensure null termination */
    c_str[len] = '\0';
    /* Ensure no embedded null bytes so strlen(c_str) == len */
    for (size_t i = 0; i < len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }

    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postcondition 1: If result is non-NULL, it must be a valid aws_string */
    if (result != NULL) {
        /* Validity: aws_string_is_valid checks allocator, len, and bytes */
        assert(aws_string_is_valid(result));

        /* Postcondition 2: The allocator stored in the result matches the one passed in */
        assert(result->allocator == allocator);

        /* Postcondition 3: The length of the result matches strlen(c_str) */
        assert(result->len == strlen(c_str));

        /* Postcondition 4: The bytes are null-terminated (null byte after data) */
        assert(result->bytes[result->len] == '\0');

        /* Postcondition 5: The content of the result matches the input c_str */
        assert(memcmp(aws_string_bytes(result), c_str, result->len) == 0);

        /* Postcondition 6: The length is consistent with the input string length */
        assert(result->len == len);

        /* Cleanup */
        aws_string_destroy(result);
    }

    free(c_str);
}
