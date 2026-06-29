#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r = 0;

    int result = aws_mul_size_checked(a, b, &r);

    /* Postcondition 1: Return value must be either AWS_OP_SUCCESS (0) or AWS_OP_ERR (-1) */
    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    /* Postcondition 2: Check overflow condition using __builtin_mul_overflow style check */
    if (a != 0 && b != 0 && b > SIZE_MAX / a) {
        assert(result == AWS_OP_ERR);
    } else if (a == 0 || b == 0) {
        assert(result == AWS_OP_SUCCESS);
        assert(r == 0);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
