#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
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

        /* set the length field */
        *((size_t *)&str->len) = len;   /* cast away const for initialization */

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
    const uint8_t        *old_bytes     = NULL;

    if (str) {
        old_allocator = str->allocator;
        old_len       = str->len;
        old_bytes     = str->bytes;
    }

    /* 3. Call the function under test */
    aws_string_destroy(str);

    /* 4. Post‑condition checks */

    /* If the string was NULL or its allocator was NULL, the function does nothing.
       In that case the observable fields must remain unchanged. */
    if (!str || old_allocator == NULL) {
        if (str) {
            /* allocator field unchanged */
            assert(str->allocator == old_allocator);
            /* length field unchanged */
            assert(str->len == old_len);
            /* byte contents unchanged */
            assert_bytes_match(str->bytes, old_bytes, old_len);
        }
    } else {
        /* When allocator is non‑NULL the memory is released.
           The harness must not dereference `str` after the call.
           No further assertions about the contents are required. */
    }

    /* 5. In all cases the string (if it still exists) must satisfy the validity invariant. */
    if (str) {
        assert(aws_string_is_valid(str));
    }
}
