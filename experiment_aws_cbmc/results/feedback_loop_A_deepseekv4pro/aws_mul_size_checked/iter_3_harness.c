#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r = nondet_size_t();
    size_t old_r = r;

    if (a != 0 && b > SIZE_MAX / a) {
        assert(aws_mul_size_checked(a, b, &r) == AWS_OP_ERR);
        assert(r == old_r);
    } else {
        assert(aws_mul_size_checked(a, b, &r) == AWS_OP_SUCCESS);
        assert(r == a * b);
    }
}
