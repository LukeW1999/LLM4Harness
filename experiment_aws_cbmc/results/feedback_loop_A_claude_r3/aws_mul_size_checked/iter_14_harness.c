#include <aws/common/math.h>
#include <aws/common/error.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r = 0;

    int result = aws_mul_size_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        __CPROVER_assert(r == a * b, "result equals a * b");
    } else {
        __CPROVER_assert(result == AWS_OP_ERR, "error code is AWS_OP_ERR");
    }
}
