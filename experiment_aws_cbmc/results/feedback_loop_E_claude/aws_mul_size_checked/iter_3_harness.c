#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness() {
    size_t a;
    size_t b;
    size_t r;
    int rval = aws_mul_size_checked(a, b, &r);
    if (rval == AWS_OP_SUCCESS) {
        __CPROVER_assert(r == a * b, "result correct on success");
    } else {
        __CPROVER_assert(a != 0 && b > (SIZE_MAX / a), "overflow detected correctly");
    }
}
