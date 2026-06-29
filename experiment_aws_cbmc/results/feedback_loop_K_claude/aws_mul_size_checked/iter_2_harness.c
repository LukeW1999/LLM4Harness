#include <aws/common/common.h>
#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;

    __CPROVER_assume(a <= 65536);
    __CPROVER_assume(b <= 65536);

    int result = aws_mul_size_checked(a, b, &r);

    bool overflow = (a != 0) && (b > SIZE_MAX / a);

    if (overflow) {
        assert(result != AWS_OP_SUCCESS);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
