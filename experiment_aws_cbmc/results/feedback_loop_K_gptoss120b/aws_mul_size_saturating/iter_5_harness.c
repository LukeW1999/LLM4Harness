#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <limits.h>

extern size_t nondet_size_t(void);

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    __CPROVER_assume(a <= 1000);
    __CPROVER_assume(b <= 1000);

    size_t result = aws_mul_size_saturating(a, b);

    size_t expected;
    if (a != 0 && b > SIZE_MAX / a) {
        expected = SIZE_MAX;
    } else {
        expected = a * b;
    }

    __CPROVER_assert(result == expected,
                     "aws_mul_size_saturating returns correct saturated product");
}
