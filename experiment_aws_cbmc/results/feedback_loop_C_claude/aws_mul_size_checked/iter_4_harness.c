#include <aws/common/math.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;

    /* Bound the inputs to avoid state space explosion */
    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    int result = aws_mul_size_checked(a, b, &r);

    int would_overflow = (a != 0) && (b > (SIZE_MAX / a));

    if (result == AWS_OP_SUCCESS) {
        assert(!would_overflow);
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(would_overflow);
    }
}
