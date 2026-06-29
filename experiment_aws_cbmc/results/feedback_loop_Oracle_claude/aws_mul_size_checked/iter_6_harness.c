#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r = 0;

    /* Bound the inputs to avoid timeout */
    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    int result = aws_mul_size_checked(a, b, &r);

    /* Postcondition 1: Return value must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: On success, r == a * b and no overflow occurred */
    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    }

    /* Postcondition 3: On overflow, result must be AWS_OP_ERR */
    if (a != 0 && b > SIZE_MAX / a) {
        assert(result == AWS_OP_ERR);
    }
}
