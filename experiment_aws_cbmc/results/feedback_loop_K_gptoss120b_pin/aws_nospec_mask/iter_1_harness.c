#include <aws/common/byte_buf.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* nondeterministic size_t generator */
size_t nondet_size_t(void);
void *nondet_void_ptr(void);

void aws_nospec_mask_harness(void) {
    /* nondeterministic inputs */
    size_t index = nondet_size_t();
    size_t bound = nondet_size_t();

    /* No preconditions to assume for this function */

    /* call the function under verification */
    size_t ret = aws_nospec_mask(index, bound);

    /* Postcondition: return value must be either 0 or UINTPTR_MAX */
    __CPROVER_assert(ret == 0 || ret == UINTPTR_MAX,
                     "aws_nospec_mask: return value must be 0 or UINTPTR_MAX");

    /* Logical correctness */
    __CPROVER_assert(
        ((index < bound) && (index <= (SIZE_MAX/2)) && (bound <= (SIZE_MAX/2))) ?
            (ret == UINTPTR_MAX) :
            (ret == 0),
        "aws_nospec_mask: logical correctness");

    return 0;
}

/* Definitions for nondeterministic generators (provided by CBMC) */
size_t nondet_size_t(void) {
    size_t x;
    return x;
}
