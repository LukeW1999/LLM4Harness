#include <aws/common/math.h>
#include <stdint.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = r;

    int ret = aws_mul_size_checked(a, b, &r);

    if (ret == AWS_OP_SUCCESS) {
        assert(a == 0 || b <= SIZE_MAX / a);
        assert(r == a * b);
    } else {
        assert(ret == AWS_OP_ERR);
        assert(!(a == 0 || b <= SIZE_MAX / a));
        assert(r == old_r);
    }
}
