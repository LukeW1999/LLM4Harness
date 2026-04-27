#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(a != 0 && b != 0 && b > SIZE_MAX / a);
    }
}
