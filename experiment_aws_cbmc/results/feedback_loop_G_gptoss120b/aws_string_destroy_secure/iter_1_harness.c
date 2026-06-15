#include <aws/common/string.h>
#include <assert.h>
#include <stdlib.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_secure_harness(void) {
    /* 1. nondeterministically decide whether the pointer is NULL */
    bool ptr_is_null = nondet_bool();
    struct aws_string *str = NULL;

    size_t old_len = 0;
    uint8_t *old_bytes = NULL;
    struct aws_allocator *old_allocator = NULL;

    if (!ptr_is_null) {
        /* allocate a string with a bounded length */
        size_t max_len = MAX_BUFFER_SIZE;
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= max_len);

        /* allocate enough space for the struct plus the flexible array */
        str = malloc(sizeof(struct aws_string) + (len > 0 ? len - 1 : 0));
        __CPROVER_assume(str != NULL);

        /* nondeterministically choose an allocator (may be NULL) */
        str->allocator = nondet_bool() ? NULL : (struct aws_allocator *)malloc(1);
        /* set length */
        *((size_t *)&str->len) = len; /* const cast for initialization */

        /* initialize bytes with nondeterministic values */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string is valid according to the library predicate */
        __CPROVER_assume(aws_string_is_valid(str));

        /* save old state */
        old_len = len;
        old_allocator = str->allocator;
        if (old_len > 0) {
            old_bytes = malloc(old_len);
            __CPROVER_assume(old_bytes != NULL);
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = ((uint8_t *)str->bytes)[i];
            }
        }
    }

    /* 2. Call the function under test */
    aws_string_destroy_secure(str);

    /* 3. Post‑condition checks */
    if (ptr_is_null) {
        /* When the input pointer is NULL the function does nothing. */
        assert(str == NULL);
    } else {
        /* str is non‑NULL */
        if (old_allocator == NULL) {
            /* No allocator → memory is not released, the struct remains.
               The bytes must have been zeroed. */
            for (size_t i = 0; i < old_len; ++i) {
                assert(((uint8_t *)str->bytes)[i] == 0);
            }
            /* Length and allocator must be unchanged. */
            assert(str->len == old_len);
            assert(str->allocator == old_allocator);
            /* The string must still be valid. */
            assert(aws_string_is_valid(str));
        } else {
            /* Allocator non‑NULL → the string memory is released.
               The struct is no longer accessible; we make no further assertions. */
        }
    }

    /* 4. Clean up any auxiliary allocations */
    if (old_bytes) {
        free(old_bytes);
    }
    if (old_allocator && old_allocator != NULL) {
        free(old_allocator);
    }
    if (str && old_allocator == NULL) {
        free(str);
    }
}
