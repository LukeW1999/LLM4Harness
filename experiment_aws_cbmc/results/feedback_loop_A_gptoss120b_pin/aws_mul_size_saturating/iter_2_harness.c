#include <aws/common/math.h>
#include <proof_helpers/make_common_data_structures.h>
#include <assert.h>
#include <stddef.h>

void aws_mul_size_saturating_harness() {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();
    size_t result;
    int rc = aws_mul_size_saturating(a, b, &result);

    if (rc == 0) {
        /* No overflow occurred */
        if (a != 0) {
            assert(result / a == b);
        } else {
            assert(result == 0);
        }
        /* Verify the overflow condition does not hold */
        assert(b == 0 || a <= SIZE_MAX / b);
    } else {
        /* Overflow occurred */
        assert(!(b == 0 || a <= SIZE_MAX / b));
    }
}
