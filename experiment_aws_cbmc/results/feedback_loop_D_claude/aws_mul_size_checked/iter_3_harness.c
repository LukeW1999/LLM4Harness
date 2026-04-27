#include <aws/common/math.h>
#include <stdint.h>
#include <assert.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r;

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    }
}
