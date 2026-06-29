#include <aws/common/math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    /* Unconstrained inputs */
    size_t a;
    size_t b;

    /* Valid non-null output pointer */
    size_t r;
    size_t r_initial = r; /* capture initial value */

    /* Determine if overflow would occur */
    bool overflow = (b > 0) && (a > (SIZE_MAX - b));

    /* Call the function under test */
    int ret = aws_add_size_checked(a, b, &r);

    if (overflow) {
        /* Overflow case: must return AWS_OP_ERR */
        assert(ret == AWS_OP_ERR);
        /* r should not have been written (still initial value) */
        assert(r == r_initial);
    } else {
        /* No overflow case: must return AWS_OP_SUCCESS */
        assert(ret == AWS_OP_SUCCESS);
        /* Result must equal a + b */
        assert(r == a + b);
        /* Result must be >= both operands */
        assert(r >= a);
        assert(r >= b);
    }
}

void aws_add_size_checked_harness(void) {
    aws_add_size_checked_harness();
    return 0;
}
