#include <aws/common/math.h>
#include <assert.h>
#include <stdbool.h>
#include <proof_helpers/make_common_data_structures.h>

void aws_mul_size_checked_harness(void) {
    size_t a = nondet_size_t();
    size_t b = nondet_size_t();

    size_t result = 0;
    size_t old_result = result; /* keep a copy for overflow case */

    bool overflow = aws_mul_size_checked(a, b, &result);

    if (!overflow) {
        /* No overflow: result must equal the mathematical product */
        assert(result == a * b);
        /* Additionally, the multiplication must satisfy the overflow‑free condition */
        if (a != 0) {
            assert(b <= ((size_t)-1) / a);
        }
    } else {
        /* Overflow: the function must not modify the output value */
        assert(result == old_result);
        /* And the overflow condition must hold */
        if (a != 0) {
            assert(b > ((size_t)-1) / a);
        } else {
            /* a == 0 cannot overflow, so this case should be unreachable */
            assert(0);
        }
    }
}
