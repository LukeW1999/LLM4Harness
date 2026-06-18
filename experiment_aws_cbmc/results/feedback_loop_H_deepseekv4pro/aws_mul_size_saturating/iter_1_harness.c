#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>

void aws_mul_u64_checked_harness() {
    uint64_t a = nondet_uint64_t();
    uint64_t b = nondet_uint64_t();
    uint64_t r;
    uint64_t old_r = nondet_uint64_t();

    r = old_r;                     /* save initial value to detect changes on failure */

    int ret = aws_mul_u64_checked(a, b, &r);

    if (ret == AWS_OP_SUCCESS) {
        /* On success the product fits and is stored in *r */
        assert(r == a * b);
        /* Additional sanity: no overflow occurred (b == 0 is always safe) */
        if (b != 0) {
            assert(a <= UINT64_MAX / b);
        }
    } else {
        /* On failure the function returns error */
        assert(ret == AWS_OP_ERR);
        /* *r is untouched */
        assert(r == old_r);
        /* Overflow must have been possible: b cannot be zero and a*b > UINT64_MAX */
        assert(b != 0);
        assert(a > UINT64_MAX / b);
    }
}
