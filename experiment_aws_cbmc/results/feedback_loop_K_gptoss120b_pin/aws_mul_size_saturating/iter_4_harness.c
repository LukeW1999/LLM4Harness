#include <aws/common/math.h>
#include <stdbool.h>
#include <limits.h>
#include <proof_helpers/make_common_data_structures.h>

size_t nondet_size_t(void);

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t r = aws_mul_size_saturating(a, b);

    bool overflow;
    size_t expected;

    if (a == 0 || b == 0) {
        overflow = false;
        expected = 0;
    } else {
        overflow = (a > SIZE_MAX / b);
        if (overflow) {
            expected = SIZE_MAX;
        } else {
            expected = a * b;
        }
    }

    __CPROVER_assert(r == expected, "aws_mul_size_saturating result matches expected");
}
