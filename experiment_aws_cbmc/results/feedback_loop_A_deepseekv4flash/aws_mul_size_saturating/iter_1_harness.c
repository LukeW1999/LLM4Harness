#include <aws/common/math.h>
#include <aws/common/common.h>
#include <proof_helpers/make_common_data_structures.h>
#include <limits.h>

void aws_mul_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = r;

    int result = aws_mul_u64_checked(a, b, &r);

    if (result == AWS_OP_SUCCESS) {
        assert(r == a * b);
    } else {
        assert(r == old_r);
    }

    /* Verify overflow detection */
    if (a != 0 && b > UINT64_MAX / a) {
        assert(result == AWS_OP_ERR);
    } else {
        assert(result == AWS_OP_SUCCESS);
    }
}
