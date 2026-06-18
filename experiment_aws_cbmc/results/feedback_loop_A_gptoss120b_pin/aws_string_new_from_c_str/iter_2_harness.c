#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_new_from_c_str_harness(void) {
    /* 1. Allocate and nondeterministically initialize a C string */
    size_t len;
    __CPROVER_assume(len < MAX_STRING_LEN);

    char *c_str = (char *)malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill the string with nondeterministic non‑zero bytes */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[len] = '\0'; /* null‑terminate */

    /* Assume the C string is valid according to the library predicate */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* 2. Use the default allocator (never NULL for dynamic allocation) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* 3. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Post‑condition checks */
    if (result != NULL) {
        /* The returned string must be valid */
        assert(aws_string_is_valid(result));

        /* Length must match the original C string length */
        assert(result->len == len);

        /* Bytes must match the original content */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* Null terminator must be present immediately after the data */
        assert(result->bytes[len] == '\0');

        /* Allocator field must be the one we passed in */
        assert(result->allocator == allocator);
    }

    /* 5. Clean up */
    free(c_str);
    if (result != NULL) {
        aws_string_destroy(result);
    }
}
