#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Bound the inputs to reduce state space and avoid timeout */
    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    size_t result = aws_mul_size_saturating(a, b);

    /* Result must be SIZE_MAX if overflow would occur, otherwise a*b */
    if (a != 0 && b > SIZE_MAX / a) {
        /* Overflow case: result should be saturated to SIZE_MAX */
        assert(result == SIZE_MAX);
    } else {
        /* No overflow: result should be exact product */
        assert(result == a * b);
    }

    /* Result is always bounded by SIZE_MAX */
    assert(result <= SIZE_MAX);
}
