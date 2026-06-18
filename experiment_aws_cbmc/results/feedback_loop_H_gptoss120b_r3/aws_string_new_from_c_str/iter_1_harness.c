#include <aws/common/byte_buf.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <proof_helpers/nondet.h>
#include <proof_helpers/utils.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

#define MAX_STRING_LEN 256

void aws_string_new_from_c_str_harness(void) {
    /* allocator: use the default allocator (always valid) */
    struct aws_allocator *allocator = aws_default_allocator();

    /* nondeterministically create a C string with a bounded length */
    size_t max_len = nondet_size_t();
    __CPROVER_assume(max_len <= MAX_STRING_LEN);

    char *c_str = malloc(max_len + 1);
    __CPROVER_assume(c_str != NULL);

    /* fill the buffer with nondeterministic bytes and ensure null‑termination */
    for (size_t i = 0; i < max_len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[max_len] = '\0';

    /* assume the string satisfies the library’s validity predicate */
    __CPROVER_assume(aws_c_string_is_valid(c_str));

    /* call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    if (result != NULL) {
        /* success: the returned string must be valid */
        assert(aws_string_is_valid(result));

        /* compute the length of the input C string (bounded by max_len) */
        size_t expected_len = 0;
        while (expected_len < max_len && c_str[expected_len] != '\0') {
            ++expected_len;
        }

        /* length field must match the input string length */
        assert(result->len == expected_len);

        /* the allocator stored in the string must be the one we passed */
        assert(result->allocator == allocator);

        /* the bytes of the string must exactly match the input bytes */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, expected_len);

        /* a null terminator must exist immediately after the data */
        assert(result->bytes[expected_len] == 0);
    } else {
        /* failure: no string was allocated, nothing else to assert */
        /* (the allocator and input string are unchanged by definition) */
    }

    /* clean up the nondeterministic input buffer */
    free(c_str);
}
