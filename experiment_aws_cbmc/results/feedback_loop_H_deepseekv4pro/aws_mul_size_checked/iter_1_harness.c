#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = nondet_uint64_t();
    r = old_r;

    int result = aws_mul_u64_checked(a, b, &r);

    /* Determine if a * b overflows uint64_t */
    bool overflow = (a != 0) && (b > UINT64_MAX / a);

    if (!overflow) {
        /* Success path: result must be AWS_OP_SUCCESS and *r must hold the product */
        assert(result == AWS_OP_SUCCESS);
        assert(r == a * b);
    } else {
        /* Failure path: result must be AWS_OP_ERR and *r must remain unchanged */
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}
