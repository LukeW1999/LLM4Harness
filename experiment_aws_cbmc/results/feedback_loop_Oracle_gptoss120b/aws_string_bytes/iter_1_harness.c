#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <aws/common/string.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"

/* Nondeterministic helpers for CBMC */
size_t nondet_size_t(void);
uint8_t nondet_uint8_t(void);

void aws_string_bytes_harness(void) {
    /* Allocate a nondeterministic aws_string */
    struct aws_string *str;
    size_t len = nondet_size_t();
    __CPROVER_assume(len <= 256);               /* bound the length */

    /* Allocate enough memory for the struct plus the flexible array */
    size_t total_size = sizeof(struct aws_string) + (len > 0 ? len - 1 : 0);
    str = malloc(total_size);
    __CPROVER_assume(str != NULL);

    /* Initialise fields (cast away constness for the test harness) */
    *((struct aws_allocator **)&str->allocator) = NULL;   /* allocator can be NULL */
    *((size_t *)&str->len) = len;                        /* set length */

    /* Fill the byte array with nondeterministic data */
    for (size_t i = 0; i < len; ++i) {
        ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
    }
    /* Ensure a terminating NUL byte as required by the implementation */
    ((uint8_t *)str->bytes)[len] = 0;

    /* Structural validity assumption */
    __CPROVER_assume(aws_string_is_valid(str));

    /* Snapshot of the observable state before the call */
    uint8_t *old_bytes = malloc(len);
    __CPROVER_assume(old_bytes != NULL);
    for (size_t i = 0; i < len; ++i) {
        old_bytes[i] = str->bytes[i];
    }
    struct aws_allocator *old_alloc = str->allocator;
    size_t old_len = str->len;

    /* Call the function under verification */
    const uint8_t *ret = aws_string_bytes(str);

    /* Post‑condition 1: return value correctness */
    assert(ret == str->bytes);
    assert(ret != NULL);

    /* Post‑condition 2: output buffer invariants (no change to data) */
    for (size_t i = 0; i < len; ++i) {
        assert(ret[i] == old_bytes[i]);
    }

    /* Post‑condition 3: frame conditions – no other memory modified */
    assert(str->allocator == old_alloc);
    assert(str->len == old_len);

    return 0;
}
