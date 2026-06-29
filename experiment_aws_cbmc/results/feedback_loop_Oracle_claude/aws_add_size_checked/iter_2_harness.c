#include <aws/common/math.h>
#include <aws/common/common.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_add_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;
    size_t r_init;

    /* Initialize r to a known value to check frame conditions */
    r_init = r;

    int result = aws_add_size_checked(a, b, &r);

    /* 1. Return value correctness */
    /* Result must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Check overflow condition */
    bool would_overflow = (b > 0) && (a > (SIZE_MAX - b));

    if (would_overflow) {
        /* If overflow would occur, must return error */
        assert(result == AWS_OP_ERR);
    } else {
        /* If no overflow, must return success */
        assert(result == AWS_OP_SUCCESS);

        /* 2. Output buffer invariants: r must equal a + b */
        assert(r == a + b);

        /* r must be >= a and >= b (since no overflow) */
        assert(r >= a);
        assert(r >= b);
    }

    /* 3. Frame conditions: if error, r should not have been modified */
    if (result == AWS_OP_ERR) {
        /* On error, the output should not be written (r stays as r_init) */
        assert(r == r_init);
    }
}
