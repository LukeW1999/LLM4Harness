#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;
    __CPROVER_assume(a < 10000 && b < 10000);

    size_t result = aws_mul_size_saturating(a, b);

    if (b != 0 && a > SIZE_MAX / b) {
        __CPROVER_assert(result == SIZE_MAX, "Result must be SIZE_MAX on overflow");
    } else {
        __CPROVER_assert(result == a * b, "Result must be exact product when no overflow");
    }
}
