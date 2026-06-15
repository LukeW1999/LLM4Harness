#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>

/* Bounding constant – adjust as needed for the verification run */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 64
#endif

void aws_string_new_from_c_str_harness(void) {
    /* 1. Non‑deterministic inputs */
    struct aws_allocator *allocator = aws_default_allocator();
    __CPROVER_assume(allocator != NULL);          /* allocator must be valid */

    size_t len = nondet_size_t();
    __CPROVER_assume(len <= MAX_STRING_LEN);

    char *c_str = (char *)malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Fill the buffer with non‑zero nondeterministic bytes and terminate it */
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
        __CPROVER_assume(c_str[i] != '\0');      /* ensure no interior NULs */
    }
    c_str[len] = '\0';

    /* 2. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 3. Post‑condition checks */
    if (result != NULL) {
        /* Changed / defined fields */
        assert(result->allocator == allocator);
        assert(result->len == len);

        /* Bytes must be an exact copy of the input string (excluding the terminator) */
        assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);

        /* A null terminator must exist immediately after the data bytes */
        assert(((const char *)result->bytes)[len] == '\0');

        /* Validity invariant */
        assert(aws_string_is_valid(result));
    } else {
        /* Failure path – result is NULL */
        assert(result == NULL);
    }

    /* Clean up the nondeterministic C‑string buffer */
    free(c_str);
}
