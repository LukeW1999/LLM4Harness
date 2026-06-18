#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t mode = nondet_size_t();
    __CPROVER_assume(mode <= 3);

    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    if (mode == 0) {
        a = 0;
    } else if (mode == 1) {
        b = 0;
    } else if (mode == 2) {
        __CPROVER_assume(a <= 65535);
        __CPROVER_assume(b <= 65535);
    } else {
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b != 0);
        __CPROVER_assume(a > SIZE_MAX / b);
    }

    size_t result = aws_mul_size_saturating(a, b);

    if (a == 0 || b == 0) {
        assert(result == 0);
    } else if (a > SIZE_MAX / b) {
        assert(result == SIZE_MAX);
    } else {
        assert(result == a * b);
    }
}
