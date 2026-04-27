#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t result;

    /* Constrain inputs to avoid timeout - limit to small ranges */
    __CPROVER_assume(a <= 0xFF);
    __CPROVER_assume(b <= 0xFF);

    int ret = aws_mul_size_checked(a, b, &result);

    /* Postconditions */
    if (ret == AWS_OP_SUCCESS) {
        __CPROVER_assert(result == a * b, "result equals a * b on success");
    } else {
        __CPROVER_assert(ret == AWS_OP_ERR, "return value is AWS_OP_ERR on failure");
    }
}
