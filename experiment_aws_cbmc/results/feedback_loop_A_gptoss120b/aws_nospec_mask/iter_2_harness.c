#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "aws/common/byte_buf.h"   /* declaration of aws_nospec_mask */

/* nondeterministic generators provided by the proof helpers */
size_t nondet_size_t(void);
bool nondet_bool(void);

void aws_nospec_mask_harness(void) {
    bool scenario = nondet_bool();

    if (scenario) {
        /* Branch where the condition is true → mask must be 0 */
        size_t index = nondet_size_t();
        size_t bound = nondet_size_t();

        /* Force at least one of the out‑of‑range conditions to hold */
        __CPROVER_assume(
            (index >= bound) ||
            (bound > (SIZE_MAX / 2)) ||
            (index > (SIZE_MAX / 2))
        );

        size_t mask = aws_nospec_mask(index, bound);
        assert(mask == 0);
    } else {
        /* Branch where the condition is false → mask must be all‑ones */
        size_t index = nondet_size_t();
        size_t bound = nondet_size_t();

        /* Force all out‑of‑range conditions to be false */
        __CPROVER_assume(index < bound);
        __CPROVER_assume(bound <= (SIZE_MAX / 2));
        __CPROVER_assume(index <= (SIZE_MAX / 2));

        size_t mask = aws_nospec_mask(index, bound);
        assert(mask == UINTPTR_MAX);
    }
}
