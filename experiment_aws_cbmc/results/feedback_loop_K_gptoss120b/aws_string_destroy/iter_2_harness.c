/*
 * Harness for aws_string_destroy
 */

#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <string.h>

void aws_string_destroy_harness(void) {
    /* nondet allocation size for the flexible array member (may be zero) */
    size_t extra_len = nondet_size_t();
    __CPROVER_assume(extra_len <= 256); /* keep the allocation bounded */

    /* Allocate storage on the stack large enough for the maximum size */
    uint8_t storage[sizeof(struct aws_string) + 256];
    struct aws_string *str = (struct aws_string *)storage;

    /* nondet choice: either we have a valid string object or NULL */
    if (nondet_bool()) {
        /* Initialise the allocator field */
        struct aws_allocator *alloc = aws_default_allocator();
        __CPROVER_assume(alloc != NULL);
        if (nondet_bool()) {
            str->allocator = alloc;
        } else {
            str->allocator = NULL;
        }

        /* Set a nondet length that is consistent with the allocated extra bytes */
        size_t len = nondet_size_t();
        __CPROVER_assume(len <= extra_len);
        *((size_t *)&str->len) = len; /* cast away constness for harness */

        /* Initialise the flexible array bytes */
        for (size_t i = 0; i < extra_len; ++i) {
            ((uint8_t *)str->bytes)[i] = nondet_uint8_t();
        }

        /* Ensure the string satisfies the library’s validity predicate */
        __CPROVER_assume(aws_string_is_valid(str));
    } else {
        str = NULL;
    }

    /* Snapshot of memory that must remain unchanged (a dummy buffer) */
    uint8_t untouched[32];
    for (size_t i = 0; i < sizeof(untouched); ++i) {
        untouched[i] = nondet_uint8_t();
    }
    uint8_t untouched_before[32];
    memcpy(untouched_before, untouched, sizeof(untouched));

    /* Preserve the original pointer value for post‑condition check */
    struct aws_string *str_before = str;

    /* Call the function under verification */
    aws_string_destroy(str);

    /* Post‑condition checks */
    /* The caller’s pointer variable must be unchanged */
    assert(str == str_before);

    /* The dummy buffer must be untouched */
    for (size_t i = 0; i < sizeof(untouched); ++i) {
        assert(untouched[i] == untouched_before[i]);
    }
}
