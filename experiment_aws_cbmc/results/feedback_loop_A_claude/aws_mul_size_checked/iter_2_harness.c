#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>

size_t nondet_size_t(void);

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        /* On success: r contains the correct product */
        assert(r == a * b);
        /* No overflow: if a != 0, then r / a == b */
        if (a != 0) {
            assert(r / a == b);
        }
        /* Verify no overflow occurred */
        if (a != 0) {
            assert(b <= SIZE_MAX / a);
        }
    } else {
        /* On failure: overflow must have occurred */
        if (a != 0 && b != 0) {
            assert(a > SIZE_MAX / b);
        } else {
            /* If either is zero, multiplication cannot overflow, so this path
               should not be reached - but we don't assert false here to avoid
               spurious failures if the implementation is strict */
            assert(0 == 1); /* zero inputs should never cause overflow */
        }
    }
}
