#include <aws/common/string.h>
#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_bytes_harness(void) {
    /* nondet string pointer */
    struct aws_string *str;
    __CPROVER_assume(aws_string_is_valid(str));

    /* snapshot of fields that must remain unchanged */
    size_t   old_len   = str->len;
    struct aws_allocator *old_alloc = str->allocator;

    /* copy the byte contents (including the terminating NUL) */
    uint8_t *old_bytes = malloc(old_len + 1);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i < old_len + 1; ++i) {
        old_bytes[i] = str->bytes[i];
    }

    /* call the function under test */
    const uint8_t *result = aws_string_bytes(str);

    /* ----- post‑conditions ----- */

    /* 1. return value correctness */
    assert(result != NULL);
    assert(result == str->bytes);

    /* 2. structural invariants unchanged */
    assert(str->len == old_len);
    assert(str->allocator == old_alloc);

    /* 3. frame condition: bytes unchanged */
    for (size_t i = 0; i < old_len + 1; ++i) {
        assert(old_bytes[i] == str->bytes[i]);
    }

    free(old_bytes);
    return 0;
}
