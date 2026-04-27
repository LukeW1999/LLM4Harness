#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
        if (a != 0) {
            assert(b <= SIZE_MAX / a);
        }
    } else {
        if (a != 0 && b != 0) {
            assert(a > SIZE_MAX / b);
        }
    }
}
