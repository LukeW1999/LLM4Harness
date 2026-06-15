#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "aws/common/common.h"

/* Nondeterministic helpers provided by CBMC */
size_t nondet_size_t(void);

/* Function under test */
bool aws_add_size_saturating(size_t a, size_t b, size_t *result);

void aws_add_size_saturating_harness(void) {
    /* 1. Nondeterministic inputs, bounded to keep the state space manageable */
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    /* 2. Result storage */
    size_t result = 0;

    /* 3. Call the function under test */
    bool overflow = aws_add_size_saturating(a, b, &result);

    /* 4. Expected behaviour */
    size_t sum = a + b;               /* natural addition (wrap‑around on overflow) */

    if (overflow) {
        /* On overflow the function must saturate to SIZE_MAX */
        assert(result == SIZE_MAX);
        /* The overflow condition must be true: the wrapped sum is smaller than one operand */
        assert(sum < a);
    } else {
        /* No overflow – result must equal the exact sum */
        assert(result == sum);
        /* The sum must not have wrapped */
        assert(sum >= a);
    }
}
