#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;
    /* Bound the input space to make verification tractable */
    __CPROVER_assume(a <= 1000 && b <= 1000);

    size_t old_r = r;

    if (a != 0 && b > SIZE_MAX / a) {
        if (aws_mul_size_checked(a, b, &r) == AWS_OP_ERR) {
            assert(r == old_r);
        }
    } else {
        if (aws_mul_size_checked(a, b, &r) == AWS_OP_SUCCESS) {
            assert(r == a * b);
        }
    }
}
