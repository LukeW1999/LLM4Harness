#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();

    bool overflow_case = nondet_bool();

    if (overflow_case) {
        a = (size_t)-1;
        __CPROVER_assume(b > 1);
    } else {
        __CPROVER_assume(a <= 0xffff);
        __CPROVER_assume(b <= 0xffff);
    }

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (overflow_case) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
