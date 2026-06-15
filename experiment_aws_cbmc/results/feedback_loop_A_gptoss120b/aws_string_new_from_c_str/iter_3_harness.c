/* CBMC harness for aws_string_new_from_c_str */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"
#include "proof_helpers/assertions.h"

#ifndef MAX_STRING_LEN
#define MAX_STRING_LEN 256
#endif

void aws_string_new_from_c_str_harness(void) {
    /* 1. Non‑deterministic inputs */
    struct aws_allocator *allocator;
    if (nondet_bool()) {
        /* valid allocator that may fail allocations */
        allocator = can_fail_allocator();
    } else {
        /* NULL allocator – forces failure path */
        allocator = NULL;
    }

    size_t len = nondet_size_t();
    __CPROVER_assume(len < MAX_STRING_LEN);

    /* Allocate a non‑deterministic C‑string of length `len` (excluding NUL) */
    char *c_str = malloc(len + 1);
    __CPROVER_assume(c_str != NULL);
    for (size_t i = 0; i < len; ++i) {
        c_str[i] = (char)nondet_uint8_t();
    }
    c_str[len] = '\0'; /* ensure NUL‑termination */

    /* 2. Save old state of inputs that must stay unchanged */
    struct aws_allocator *old_allocator = allocator;
    char *old_c_str = malloc(len + 1);
    __CPROVER_assume(old_c_str != NULL);
    memcpy(old_c_str, c_str, len + 1);

    /* 3. Call the function under test */
    struct aws_string *result = aws_string_new_from_c_str(allocator, c_str);

    /* 4. Post‑condition checks */
    if (old_allocator == NULL) {
        /* When allocator is NULL the function must return NULL */
        assert(result == NULL);
    } else {
        if (result != NULL) {
            /* Success path */
            assert(aws_string_is_valid(result));
            assert(result->allocator == old_allocator);
            assert(result->len == len);
            /* The string bytes must match the original C‑string (excluding the implicit NUL) */
            assert_bytes_match(result->bytes, (const uint8_t *)c_str, len);
            /* The implicit NUL byte after the data must be present */
            assert(result->bytes[len] == 0);
        } else {
            /* Allocation failure – result may be NULL */
            assert(result == NULL);
        }
    }

    /* 5. Unchanged inputs */
    assert(allocator == old_allocator);
    assert_bytes_match((const uint8_t *)c_str, (const uint8_t *)old_c_str, len + 1);

    /* 6. Clean‑up */
    free(c_str);
    free(old_c_str);
}
