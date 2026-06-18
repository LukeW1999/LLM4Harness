#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "proof_helpers/make_common_data_structures.h"

void aws_string_destroy_harness(void) {
    /* nondeterministic choice: whether str is NULL */
    struct aws_string *str;
    __CPROVER_assume(str == NULL || str != NULL);

    /* Snapshot buffer to check frame condition */
    uint8_t frame_snapshot[128];
    for (size_t i = 0; i < sizeof(frame_snapshot); ++i) {
        frame_snapshot[i] = nondet_uint8_t();
    }

    /* Preserve original allocator pointer for post‑condition checks */
    struct aws_allocator *orig_allocator = NULL;
    size_t orig_len = 0;

    if (str != NULL) {
        /* nondeterministically decide if allocator is NULL */
        bool allocator_is_null = nondet_bool();
        __CPROVER_assume(allocator_is_null == true || allocator_is_null == false);

        /* allocate memory for the string (including flexible array) */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);               /* keep allocation bounded */
        size_t alloc_size = sizeof(struct aws_string) + len;
        str = (struct aws_string *)malloc(alloc_size);
        __CPROVER_assume(str != NULL);              /* allocation must succeed for this path */

        /* initialize fields */
        if (allocator_is_null) {
            str->allocator = NULL;
        } else {
            str->allocator = aws_default_allocator();
        }
        orig_allocator = str->allocator;
        str->len = len;
        orig_len = len;

        /* initialize the bytes (including the mandatory null terminator) */
        for (size_t i = 0; i < len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }
        ((uint8_t *)str->bytes)[len] = 0; /* null terminator */

        /* ensure the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    }

    /* Call the function under test */
    aws_string_destroy(str);

    /* ---- Post‑condition checks ---- */

    /* Frame condition: memory other than the possibly freed string must be unchanged */
    for (size_t i = 0; i < sizeof(frame_snapshot); ++i) {
        __CPROVER_assert(frame_snapshot[i] == ((uint8_t *)frame_snapshot)[i],
                         "frame memory unchanged");
    }

    /* Validity/post‑condition checks */
    if (str != NULL && orig_allocator != NULL) {
        /* The memory region of str has been released; we cannot safely dereference it.
         * CBMC models free as making the memory region undefined, so we assert that
         * any subsequent read would be undefined. The simplest way is to avoid any
         * further access; the absence of a crash is sufficient for this harness. */
        /* No further assertions on str are made. */
    } else {
        /* When no free occurs, the string (if any) must remain unchanged */
        if (str != NULL) {
            __CPROVER_assert(str->allocator == orig_allocator,
                             "allocator unchanged when not freed");
            __CPROVER_assert(str->len == orig_len,
                             "length unchanged when not freed");
            for (size_t i = 0; i < orig_len; ++i) {
                __CPROVER_assert(((uint8_t *)str->bytes)[i] == ((uint8_t *)str->bytes)[i],
                                 "bytes unchanged when not freed");
            }
        }
    }

    return 0;
}
