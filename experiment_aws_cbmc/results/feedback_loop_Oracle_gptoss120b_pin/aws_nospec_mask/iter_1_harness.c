#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* CBMC nondeterministic size_t generator */
size_t nondet_size_t(void);

void aws_nospec_mask_harness(void) {
    /* nondeterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* preserve original values for frame condition checks */
    size_t index_orig = index;
    size_t bound_orig = bound;

    /* call the function under verification */
    size_t mask = aws_nospec_mask(index, bound);

    /* postcondition: mask correctness */
    if (index_orig >= bound_orig ||
        bound_orig > (SIZE_MAX / 2) ||
        index_orig > (SIZE_MAX / 2)) {
        __CPROVER_assert(mask == 0,
            "aws_nospec_mask should return 0 when out of bounds or too large");
    } else {
        __CPROVER_assert(mask == UINTPTR_MAX,
            "aws_nospec_mask should return UINTPTR_MAX when in bounds");
    }

    /* frame condition: inputs unchanged */
    __CPROVER_assert(index == index_orig,
        "aws_nospec_mask must not modify index argument");
    __CPROVER_assert(bound == bound_orig,
        "aws_nospec_mask must not modify bound argument");

    return 0;
}
