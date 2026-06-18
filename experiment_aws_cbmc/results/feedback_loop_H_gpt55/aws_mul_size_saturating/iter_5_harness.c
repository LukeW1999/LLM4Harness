#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_saturating_harness(void) {
    size_t mode = nondet_size_t();
    __CPROVER_assume(mode <= 4);

    size_t a;
    size_t b;
    size_t expected;

    if (mode == 0) {
        a = 0;
        b = nondet_size_t();
        expected = 0;
    } else if (mode == 1) {
        a = nondet_size_t();
        b = 0;
        expected = 0;
    } else if (mode == 2) {
        a = nondet_size_t();
        b = nondet_size_t();
        __CPROVER_assume(a <= 255);
        __CPROVER_assume(b <= 255);
        expected = a * b;
    } else if (mode == 3) {
        a = (size_t)-1;
        b = 2;
        expected = (size_t)-1;
    } else {
        a = ((size_t)-1 / 2) + 1;
        b = 2;
        expected = (size_t)-1;
    }

    size_t result = aws_mul_size_saturating(a, b);
    assert(result == expected);
}
