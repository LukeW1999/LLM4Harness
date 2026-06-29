#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STRING_LEN 16

void aws_byte_cursor_from_c_str_harness(void) {
    /* Non-deterministic choice: use NULL or a valid c-string */
    bool use_null;

    if (use_null) {
        /* Test with NULL input */
        const char *c_str = NULL;

        struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

        /* Postconditions for NULL input */
        assert(result.ptr == NULL);
        assert(result.len == 0);
        assert(aws_byte_cursor_is_valid(&result));
    } else {
        /* Test with a valid null-terminated string of bounded length */
        size_t str_len;
        __CPROVER_assume(str_len <= MAX_STRING_LEN);

        /* Allocate a string of str_len characters plus null terminator */
        char *c_str = malloc(str_len + 1);
        __CPROVER_assume(c_str != NULL);

        /* Ensure the string is properly null-terminated */
        /* CBMC will non-deterministically fill the content */
        c_str[str_len] = '\0';

        /* Ensure no embedded null bytes before str_len
         * (so strlen(c_str) == str_len) */
        for (size_t i = 0; i < str_len; i++) {
            __CPROVER_assume(c_str[i] != '\0');
        }

        /* Precondition: c_str is a valid non-NULL null-terminated string */
        __CPROVER_assume(c_str != NULL);

        struct aws_byte_cursor result = aws_byte_cursor_from_c_str(c_str);

        /* Postconditions for non-NULL input */
        assert(result.ptr == (uint8_t *)c_str);
        assert(result.len == str_len);
        assert(result.len == strlen(c_str));
        assert(aws_byte_cursor_is_valid(&result));

        /* Frame condition: original string is unchanged */
        assert(c_str[str_len] == '\0');

        free(c_str);
    }
}
