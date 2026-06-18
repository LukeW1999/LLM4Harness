#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <limits.h>
#include "aws/common/byte_buf.h"   /* declaration of aws_nospec_mask */

/* nondeterministic generators provided by the proof helpers */

void aws_nospec_mask_harness(void) {
    bool scenario = nondet_bool();

    if (scenario) {
        /* Branch where the condition is true → mask must be 0 */
        size_t index = nondet_size_t();
        size_t bound = nondet_size_t();

        /* Force at least one out‑of‑range condition to hold */
        __CPROVER_assume(index >= bound ||
                         bound > (SIZE_MAX / 2) ||
                         index > (SIZE_MAX / 2));

        size_t mask = aws_nospec_mask(index, bound);
        /* mask must be either 0 or all‑ones, and in this scenario it must be 0 */
        assert(mask == 0 || mask == SIZE_MAX);
        assert(mask == 0);
    } else {
        /* Branch where the condition is false → mask must be all‑ones */
        size_t index = nondet_size_t();
        size_t bound = nondet_size_t();

        /* Ensure all out‑of‑range conditions are false */
        __CPROVER_assume(index < bound);
        __CPROVER_assume(bound <= (SIZE_MAX / 2));
        __CPROVER_assume(index <= (SIZE_MAX / 2));

        size_t mask = aws_nospec_mask(index, bound);
        /* mask must be either 0 or all‑ones, and in this scenario it must be all‑ones */
        assert(mask == 0 || mask == SIZE_MAX);
        assert(mask == SIZE_MAX);
    }
}
