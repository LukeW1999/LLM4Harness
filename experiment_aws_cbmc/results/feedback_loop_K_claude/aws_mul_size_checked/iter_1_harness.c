#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;

    /* No precondition constraints — a and b are fully unconstrained */

    int result = aws_mul_size_checked(a, b, &r);

    /* Determine whether overflow would occur */
    bool overflow = (a != 0) && (b > SIZE_MAX / a);

    if (overflow) {
        /* Must return an error */
        assert(result != AWS_OP_SUCCESS);
    } else {
        /* Must return success and store correct product */
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
