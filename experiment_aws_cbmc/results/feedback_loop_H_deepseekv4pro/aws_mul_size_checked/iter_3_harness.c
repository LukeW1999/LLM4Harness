#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = nondet_size_t();
    r = old_r;

    int result = aws_mul_size_checked(a, b, &r);

    if (!__CPROVER_overflow_mul(a, b)) {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    } else {
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}
