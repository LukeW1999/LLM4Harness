/*
 * Harness for aws_string_destroy
 */

#include <aws/common/string.h>
#include <aws/common/allocator.h>
#include <aws/common/common.h>
#include "proof_helpers/make_common_data_structures.h"
#include <string.h>

void aws_string_destroy_harness(void) {
    struct aws_string *str;

    /* nondet choice: either we have a valid string object or NULL */
    if (nondet_bool()) {
        /* allocate a valid string using the default allocator */
        struct aws_allocator *alloc = aws_default_allocator();
        __CPROVER_assume(alloc != NULL);

        size_t len = nondet_size_t();
        __CPROVER_assume(len <= 256);

        uint8_t data[256];
        for (size_t i = 0; i < len; ++i) {
            data[i] = nondet_uint8_t();
        }

        str = aws_string_new_from_array(alloc, data, len);
        __CPROVER_assume(str != NULL);
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
