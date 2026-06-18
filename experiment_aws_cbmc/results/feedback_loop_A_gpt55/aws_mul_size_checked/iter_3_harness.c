#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t result = nondet_size_t();
    size_t old_result = result;

    if (nondet_size_t()) {
        a = nondet_size_t();
        b = nondet_size_t();

        __CPROVER_assume(a <= (size_t)65535);
        __CPROVER_assume(b <= (size_t)65535);

        int rv = aws_mul_size_checked(a, b, &result);

        assert(rv == AWS_OP_SUCCESS);
        assert(result == a * b);
    } else {
        a = (size_t)-1;
        b = (size_t)2;

        int rv = aws_mul_size_checked(a, b, &result);

        assert(rv == AWS_OP_ERR);
        assert(result == old_result);
    }
}
