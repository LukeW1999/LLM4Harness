#include <aws/common/string.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* Allocate a string with enough space for the flexible array member */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN);
    __CPROVER_assume(str != NULL);

    /* Non‑deterministic length, bounded */
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_STRING_LEN);

    /* Allocator is not used by aws_string_bytes; set to NULL (static string) */
    str->allocator = NULL;

    /* Initialise the byte contents (including the required null terminator) */
    uint8_t *bytes = (uint8_t *)str->bytes;
    for (size_t i = 0; i < str->len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[str->len] = 0; /* null terminator */

    /* Assume the string satisfies the library’s validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save old immutable state for later comparison */
    struct aws_string old = *str;                     /* copy struct fields */
    uint8_t old_bytes[MAX_STRING_LEN + 1];
    for (size_t i = 0; i < str->len + 1; ++i) {
        old_bytes[i] = bytes[i];
    }

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* -------------------------------------------------------------------- */
    /* Post‑conditions */

    /* 1. The returned pointer must be the address of the string’s byte array */
    assert(result == str->bytes);

    /* 2. No fields of the struct may have changed */
    assert(str->allocator == old.allocator);
    assert(str->len == old.len);

    /* 3. The byte contents must be unchanged */
    for (size_t i = 0; i < str->len + 1; ++i) {
        assert(result[i] == old_bytes[i]);
    }

    /* 4. The string must still be valid after the call */
    assert(aws_string_is_valid(str));
}
