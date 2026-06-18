#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

/* nondet helpers */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    /* Create a nondeterministic C string (null‑terminated) */
    size_t max_len = 64;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= max_len);

    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[len] = '\0';

    /* Allocate an aws_string from the C string */
    struct aws_string *str = aws_string_new_from_c_str(aws_default_allocator(), c_str);
    free(c_str);
    __CPROVER_assume(str != NULL);
    __CPROVER_assume(aws_string_is_valid(str));

    /* Snapshot state before the call */
    const uint8_t *old_bytes = str->bytes;
    size_t old_len = str->len;
    uint8_t *old_copy = malloc(old_len);
    __CPROVER_assume(old_copy != NULL);
    for (size_t i = 0; i < old_len; ++i) {
        old_copy[i] = old_bytes[i];
    }

    /* Call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* Post‑condition 1: return value points to the string's byte array */
    assert(result == str->bytes);

    /* Post‑condition 2: length and contents are unchanged */
    assert(str->len == old_len);
    for (size_t i = 0; i < old_len; ++i) {
        assert(result[i] == old_copy[i]);
    }

    /* Post‑condition 3: no other memory of the string object was modified */
    /* (bytes pointer and length already checked; allocator is const) */

    /* Clean up */
    free(old_copy);
    aws_string_destroy(str);
    return 0;
}
