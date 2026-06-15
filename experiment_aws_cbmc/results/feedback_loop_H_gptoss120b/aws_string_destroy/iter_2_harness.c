#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <aws/common/string.h>
#include "proof_helpers/make_common_data_structures.h"

/* Bounding constant for the flexible array member */
#ifndef MAX_STRING_LEN
#   define MAX_STRING_LEN MAX_BUFFER_SIZE
#endif

void aws_string_destroy_harness(void) {
    /* 1. Declare a possibly NULL aws_string pointer */
    struct aws_string *str = NULL;

    /* nondeterministically decide whether we allocate a string */
    if (nondet_bool()) {
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= MAX_STRING_LEN);

        /* allocate space for the header plus the flexible array */
        str = malloc(sizeof(struct aws_string) + len);
        __CPROVER_assume(str != NULL); /* allocation succeeded */

        /* nondeterministically decide whether the allocator field is NULL */
        if (nondet_bool()) {
            str->allocator = NULL;
        } else {
            /* allocate a dummy allocator object (its contents are irrelevant for this harness) */
            str->allocator = malloc(sizeof(struct aws_allocator));
            __CPROVER_assume(str->allocator != NULL);
        }

        /* set the length field (cast away const for initialization) */
        *((size_t *)&str->len) = len;

        /* initialise the bytes (including the implicit null terminator) */
        for (size_t i = 0; i < len + 1; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* assume the string satisfies the library's validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* 2. Save old state (only the fields we can safely copy) */
    struct aws_allocator *old_allocator = NULL;
    size_t                old_len       = 0;
    uint8_t              *old_bytes     = NULL;

    if (str) {
        old_allocator = str->allocator;
        old_len       = str->len;
        /* allocate a copy of the bytes to compare later (only needed if allocator is NULL) */
        if (old_len > 0) {
            old_bytes = malloc(old_len);
            __CPROVER_assume(old_bytes != NULL);
            for (size_t i = 0; i < old_len; ++i) {
                old_bytes[i] = str->bytes[i];
            }
        }
    }

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */
    if (str && old_allocator == NULL) {
        /* allocator was NULL, function should have done nothing */
        assert(str->allocator == old_allocator);
        assert(str->len == old_len);
        for (size_t i = 0; i < old_len; ++i) {
            assert(str->bytes[i] == old_bytes[i]);
        }
        /* the string must still satisfy the validity invariant */
        assert(aws_string_is_valid(str));
    } else if (str && old_allocator != NULL) {
        /* allocator non‑NULL, memory has been released; we must not dereference `str` */
        /* No further assertions about `str` are possible */
    } else {
        /* str was NULL; nothing to check */
    }

    /* clean up the copy of bytes if it was allocated */
    if (old_bytes) {
        free(old_bytes);
    }
}
