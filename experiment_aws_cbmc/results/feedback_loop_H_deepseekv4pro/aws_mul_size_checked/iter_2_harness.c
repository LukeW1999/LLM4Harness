#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>

void aws_mul_size_checked_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t r;
    size_t old_r = nondet_size_t();
    r = old_r;

    int result = aws_mul_size_checked(a, b, &r);

    /* Determine if a * b overflows size_t */
    bool overflow = (a != 0) && (b > SIZE_MAX / a);

    if (!overflow) {
        /* Success path: result must be AWS_OP_SUCCESS and *r must hold the product */
        assert(result == AWS_OP_SUCCESS);
        assert(r == (size_t)(a * b));
    } else {
        /* Failure path: result must be AWS_OP_ERR and *r must remain unchanged */
        assert(result == AWS_OP_ERR);
        assert(r == old_r);
    }
}
