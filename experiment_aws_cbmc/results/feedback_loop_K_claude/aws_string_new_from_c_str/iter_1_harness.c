#include <aws/common/common.h>
#include <aws/common/string.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Bound the string length for CBMC tractability */
#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 8
#endif

void aws_string_new_from_c_str_harness(void) {
    /* Set up allocator */
    struct aws_allocator *allocator = aws_default_allocator();
    assert(allocator != NULL);

    /* Create a bounded non-deterministic null-terminated C string */
    size_t len;
    __CPROVER_assume(len <= MAX_STRING_LEN);

    /* Allocate c_str with room for null terminator */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);

    /* Make contents non-deterministic but ensure null termination */
    /* CBMC will fill bytes non-deterministically */
    /* Ensure no embedded nulls so strlen(c_str) == len */
    for (size_t i = 0; i < len; i++) {
        __CPROVER_assume(c_str[i] != '\0');
    }
    c_str[len] = '\0';

    /* Verify preconditions */
    assert(c_str != NULL);
    assert(strlen(c_str) == len);

    /* Save a copy of c_str to verify it is not modified */
    char *c_str_copy = malloc(len + 1);
    __CPROVER_assume(c_str_copy != NULL);
    memcpy(c_str_copy, c_str, len + 1);

    /* Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* Postconditions */
    if (result != NULL) {
        /* Validity: aws_string_is_valid */
        assert(aws_string_is_valid(result));

        /* Allocator preserved */
        assert(result->allocator == allocator);

        /* Length matches strlen(c_str) */
        assert(result->len == len);

        /* Null termination of result */
        assert(aws_string_bytes(result)[result->len] == '\0');

        /* Content matches original c_str */
        assert(memcmp(aws_string_bytes(result), c_str, len) == 0);

        /* Frame: c_str was not modified */
        assert(memcmp(c_str, c_str_copy, len + 1) == 0);

        /* Clean up */
        aws_string_destroy(result);
    }

    /* Frame: c_str was not modified regardless of allocation success */
    assert(memcmp(c_str, c_str_copy, len + 1) == 0);

    free(c_str);
    free(c_str_copy);

    return 0;
}
