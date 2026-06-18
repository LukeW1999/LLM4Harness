#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int overflow_flag = nondet_int();
    __CPROVER_assume(overflow_flag == 0 || overflow_flag == 1);

    if (overflow_flag) {
        __CPROVER_assume(__CPROVER_overflow_mul(a, b));
    } else {
        __CPROVER_assume(!__CPROVER_overflow_mul(a, b));
    }

    int result = aws_mul_size_checked(a, b, &r);

    if (overflow_flag) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
