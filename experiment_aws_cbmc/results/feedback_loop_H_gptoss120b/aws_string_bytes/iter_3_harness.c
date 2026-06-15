#include <aws/common/string.h>
#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include "proof_helpers/make_common_data_structures.h"

#define MAX_STRING_LEN 256

void aws_string_bytes_harness(void) {
    /* Allocate space for the struct plus a maximum byte array (including null terminator) */
    struct aws_string *str = malloc(sizeof(struct aws_string) + MAX_STRING_LEN + 1);
    __CPROVER_assume(str != NULL);

    /* Non‑deterministic length, bounded so that a null terminator fits */
    str->len = nondet_size_t();
    __CPROVER_assume(str->len <= MAX_STRING_LEN);

    /* This string is static (no allocator) */
    str->allocator = NULL;

    /* Initialise the byte contents */
    uint8_t *bytes = (uint8_t *)str->bytes;
    for (size_t i = 0; i < str->len; ++i) {
        bytes[i] = nondet_uint8_t();
    }
    bytes[str->len] = 0; /* null terminator */

    /* Assume the string satisfies the library’s validity predicate */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Save a copy of the original immutable state */
    struct aws_string old = *str;
    uint8_t old_bytes[MAX_STRING_LEN + 1];
    for (size_t i = 0; i <= str->len; ++i) {
        old_bytes[i] = bytes[i];
    }

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Post‑conditions */
    assert(result == str->bytes);                     /* returned pointer */
    assert(str->allocator == old.allocator);          /* allocator unchanged */
    assert(str->len == old.len);                      /* length unchanged */
    for (size_t i = 0; i <= str->len; ++i) {          /* bytes unchanged */
        assert(result[i] == old_bytes[i]);
    }
    assert(aws_string_is_valid(str));                 /* still valid */
}
