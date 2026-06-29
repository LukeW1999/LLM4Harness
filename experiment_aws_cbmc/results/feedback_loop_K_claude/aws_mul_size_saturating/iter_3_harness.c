#include <aws/common/math.h>
#include <assert.h>
#include <stdint.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Bound inputs to avoid CBMC state space explosion */
    __CPROVER_assume(a <= 0xFFFF);
    __CPROVER_assume(b <= 0xFFFF);

    size_t result = aws_mul_size_saturating(a, b);

    /* If either operand is 0, result must be 0 */
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    /* Result must be either SIZE_MAX or a*b */
    if (a != 0 && b != 0) {
        if (result == SIZE_MAX) {
            /* Saturated - either real overflow or a*b == SIZE_MAX */
        } else {
            /* No saturation - must equal exact product */
            assert(result == a * b);
            assert(result / a == b);
        }
    }
}
