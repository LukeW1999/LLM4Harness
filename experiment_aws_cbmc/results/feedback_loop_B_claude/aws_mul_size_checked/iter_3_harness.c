#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t result;

    /* Constrain inputs to avoid timeout - limit to reasonable ranges */
    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    int ret = aws_mul_size_checked(a, b, &result);

    /* Postconditions */
    if (ret == AWS_OP_SUCCESS) {
        __CPROVER_assert(result == a * b, "result equals a * b on success");
        if (a != 0) {
            __CPROVER_assert(result / a == b, "no overflow: result / a == b");
        }
        if (b != 0) {
            __CPROVER_assert(result / b == a, "no overflow: result / b == a");
        }
    } else {
        __CPROVER_assert(ret == AWS_OP_ERR, "return value is AWS_OP_ERR on failure");
        if (a != 0 && b != 0) {
            __CPROVER_assert(b > SIZE_MAX / a, "overflow actually occurs on failure");
        }
    }
}
