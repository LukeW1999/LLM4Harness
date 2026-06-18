#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t scenario = nondet_size_t();
    size_t max_size = (size_t)-1;

    __CPROVER_assume(scenario <= 3);

    if (scenario == 0) {
        a = 0;

        size_t result = aws_mul_size_saturating(a, b);
        __CPROVER_assert(result == 0, "multiplication by zero saturates to zero");
    } else if (scenario == 1) {
        __CPROVER_assume(a != 0);
        b = 0;

        size_t result = aws_mul_size_saturating(a, b);
        __CPROVER_assert(result == 0, "multiplication by zero saturates to zero");
    } else if (scenario == 2) {
        a = max_size;
        __CPROVER_assume(b > 1);

        size_t result = aws_mul_size_saturating(a, b);
        __CPROVER_assert(result == max_size, "overflow saturates to max size");
    } else {
        __CPROVER_assume(a <= 65535);
        __CPROVER_assume(b <= 65535);

        size_t result = aws_mul_size_saturating(a, b);
        __CPROVER_assert(result == a * b, "non-overflow multiplication returns product");
    }
}
