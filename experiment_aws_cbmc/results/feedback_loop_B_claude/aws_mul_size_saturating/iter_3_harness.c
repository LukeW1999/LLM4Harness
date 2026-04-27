#include <aws/common/common.h>
#include <aws/common/math.h>
#include <stdint.h>
#include <stdlib.h>

void aws_mul_size_saturating_harness(void) {
    size_t a;
    size_t b;

    /* Non-deterministic inputs - no constraints needed */
    size_t result = aws_mul_size_saturating(a, b);

    /* Check that result is never larger than SIZE_MAX (trivially true for size_t) */
    assert(result <= SIZE_MAX);

    /* Check: if a == 0 or b == 0, result must be 0 */
    if (a == 0 || b == 0) {
        assert(result == 0);
    }

    /* Check: if a == 1, result should be b */
    if (a == 1) {
        assert(result == b);
    }

    /* Check: if b == 1, result should be a */
    if (b == 1) {
        assert(result == a);
    }
}
