#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();

    if (nondet_size_t()) {
        __CPROVER_assume(a <= 0xffff);
        __CPROVER_assume(b <= 0xffff);
    } else {
        __CPROVER_assume(a != 0);
        __CPROVER_assume(b > SIZE_MAX / a);
    }

    bool overflow = (a != 0) && (b > SIZE_MAX / a);

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (overflow) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
