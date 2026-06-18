#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;
    /* Bound inputs to small values to avoid state explosion */
    __CPROVER_assume(a <= 100 && b <= 100);

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
