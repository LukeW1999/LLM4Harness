#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    /* Limit the range to keep verification tractable */
    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        __CPROVER_assert(result == 0, "result is zero when a or b is zero");
    } else {
        if (a <= SIZE_MAX / b) {
            __CPROVER_assert(result == a * b, "no overflow multiplication");
        } else {
            __CPROVER_assert(result == SIZE_MAX, "saturation on overflow");
        }
    }
}
