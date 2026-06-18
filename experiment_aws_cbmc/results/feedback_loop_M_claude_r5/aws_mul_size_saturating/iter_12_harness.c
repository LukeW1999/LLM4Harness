#include <aws/common/math.h>
#include <stdint.h>
#include <stddef.h>

void harness(void) {
    size_t a;
    size_t b;

    size_t result = aws_mul_size_saturating(a, b);

    /* If no overflow, result == a * b */
    if (a != 0 && result / a != b) {
        /* overflow occurred, result should be SIZE_MAX */
        __CPROVER_assert(result == SIZE_MAX, "overflow saturates to SIZE_MAX");
    } else if (a == 0) {
        __CPROVER_assert(result == 0, "zero times anything is zero");
    } else {
        /* no overflow */
        __CPROVER_assert(result == a * b, "no overflow result correct");
    }
}
