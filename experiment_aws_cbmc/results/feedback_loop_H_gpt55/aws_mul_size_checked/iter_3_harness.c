#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();

    bool overflow = __CPROVER_overflow_mult(a, b);

    int result = aws_mul_size_checked(a, b, &r);

    assert(result == AWS_OP_SUCCESS || result == AWS_OP_ERR);

    if (overflow) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
