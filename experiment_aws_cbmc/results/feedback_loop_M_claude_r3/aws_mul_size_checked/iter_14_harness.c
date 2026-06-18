#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void aws_mul_size_checked_harness(void) {
    size_t a;
    size_t b;
    size_t r = 0;

    /* Tightly bound the inputs to avoid timeout */
    __CPROVER_assume(a <= 0xFF);
    __CPROVER_assume(b <= 0xFF);

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(r == 0);
    }
}
