#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a, b, r;

    int overflow_flag;
    __CPROVER_assume(overflow_flag == 0 || overflow_flag == 1);

    if (overflow_flag) {
        /* overflow case: a > 0 and product would overflow */
        __CPROVER_assume(a > 0 && b > SIZE_MAX / a);
    } else {
        /* no overflow: a == 0 or product fits */
        __CPROVER_assume(a == 0 || b <= SIZE_MAX / a);
    }

    int result = aws_mul_size_checked(a, b, &r);

    if (overflow_flag) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
