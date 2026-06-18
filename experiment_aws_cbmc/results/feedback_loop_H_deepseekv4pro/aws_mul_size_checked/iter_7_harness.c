#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;

    int overflow_flag = nondet_int();
    __CPROVER_assume(overflow_flag == 0 || overflow_flag == 1);

    if (overflow_flag) {
        __CPROVER_assume(a != 0 && b > SIZE_MAX / a);
    } else {
        __CPROVER_assume(!(a != 0 && b > SIZE_MAX / a));
    }

    int result = aws_mul_size_checked(a, b, &r);

    if (overflow_flag) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
