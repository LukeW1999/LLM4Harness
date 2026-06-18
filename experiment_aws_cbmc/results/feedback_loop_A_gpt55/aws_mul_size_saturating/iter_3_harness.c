#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t scenario = nondet_size_t();
    size_t max_size = (size_t)-1;

    __CPROVER_assume(scenario <= 3);

    if (scenario == 0) {
        __CPROVER_assume(a == 0);

        size_t result = aws_mul_size_saturating(a, b);
        assert(result == 0);
    } else if (scenario == 1) {
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b == 0);

        size_t result = aws_mul_size_saturating(a, b);
        assert(result == 0);
    } else if (scenario == 2) {
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b != 0);
        __CPROVER_assume(b > max_size / a);

        size_t result = aws_mul_size_saturating(a, b);
        assert(result == max_size);
    } else {
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b != 0);
        __CPROVER_assume(b <= max_size / a);

        size_t result = aws_mul_size_saturating(a, b);
        assert(result == a * b);
    }
}
