#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    __CPROVER_assume(a <= 0xFF);
    __CPROVER_assume(b <= 0xFF);

    size_t result = aws_mul_size_saturating(a, b);

    if (b != 0 && a > SIZE_MAX / b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
